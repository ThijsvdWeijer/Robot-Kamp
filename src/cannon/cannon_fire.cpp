#include "cannon_fire.h"

#include <Arduino.h>
#include <ESP32Servo.h>

#include "config/pins.h"

// !!! DO NOT TRUST THIS FIRMWARE UNTIL EVERY -1 BELOW HAS BEEN REPLACED !!!
// Every value below is a fake placeholder. Servo positions and the reload
// motor time MUST be found by hand, on the physical mechanism, before the
// fire/reload sequence is allowed to run for real. Running with any -1
// still in place will send a servo to angle -1 (undefined/dangerous) or
// run the reload motor for 0ms effectively skipping the pull.

// ===============================
// SERVO POSITIONS (degrees, 0-180) — TUNE THESE BY HAND
// ===============================
int SERVO1_DOWN = 180;
int SERVO1_UP   = 60;

int SERVO2_DOWN = 140;
int SERVO2_UP   = 180;

int SERVO3_SET      = 130;
int SERVO3_RELEASED = 0;

int SERVO4_OFF = 90;
int SERVO4_ON  = 180;


// ===============================
// TIMING (milliseconds) — TUNE THIS BY HAND
// ===============================
int RELOAD_MOTOR_TIME_MS = 4500;

// Optional settle delay between locking and cutting motor power, in case
// physical testing shows the servos need a moment to fully seat before
// it's safe to stop the motor. Defaults to 0 (no delay).
int LOCK_SETTLE_TIME_MS = 1000;

// Delay between setting the spool (Servo 3) and switching the motor on
// (Servo 4), so the spool is fully seated before the motor starts pulling.
int SPOOL_SETTLE_TIME_MS = 1000;

// Delay between switching the motor off (Servo 4) and releasing the spool
// (Servo 3) back to rest, so the mechanism settles before the spool lets go.
int MOTOR_OFF_SETTLE_TIME_MS = 1000;

namespace {

Servo servo1;  // firing pin
Servo servo2;  // firing support
Servo servo3;  // spool control
Servo servo4;  // motor switch

CannonState state = CannonState::Loaded;
ReloadStep reloadStep = ReloadStep::SettingSpool;

// Reused across reload sub-steps: start-of-step timestamp for whichever
// step is currently timed (MotorRunning against RELOAD_MOTOR_TIME_MS,
// Locking against LOCK_SETTLE_TIME_MS).
uint32_t stepStartTime = 0;

void setFiringServos(int angle1, int angle2) {
    servo1.write(angle1);
    servo2.write(angle2);
}

void setSpoolServo(int angle) {
    servo3.write(angle);
}

void setMotorSwitchServo(int angle) {
    servo4.write(angle);
}

void fire() {
    setFiringServos(SERVO1_UP, SERVO2_UP);
    state = CannonState::Fired;
}

// Transitions Fired -> Reloading. Per the mechanical sequence, setting the
// spool happens in this same call (the SettingSpool step is transient, not
// something later loop() iterations wait in) -- SpoolSettling onward is
// timed across loop() iterations, in updateReload() below.
void startReload() {
    state = CannonState::Reloading;
    reloadStep = ReloadStep::SettingSpool;

    setSpoolServo(SERVO3_SET);

    reloadStep = ReloadStep::SpoolSettling;
    stepStartTime = millis();
}

void updateReload() {
    switch (reloadStep) {
        case ReloadStep::SettingSpool:
            // Transient: entered and left within startReload() above.
            // Not reached from here during normal operation.
            break;

        case ReloadStep::SpoolSettling:
            if (millis() - stepStartTime >= static_cast<uint32_t>(SPOOL_SETTLE_TIME_MS)) {
                setMotorSwitchServo(SERVO4_ON);
                reloadStep = ReloadStep::MotorRunning;
                stepStartTime = millis();
            }
            break;

        case ReloadStep::MotorRunning:
            if (millis() - stepStartTime >= static_cast<uint32_t>(RELOAD_MOTOR_TIME_MS)) {
                setFiringServos(SERVO1_DOWN, SERVO2_DOWN);
                reloadStep = ReloadStep::Locking;
                stepStartTime = millis();
            }
            break;

        case ReloadStep::Locking:
            // The only path to MotorStopping (and therefore the only path
            // to ever switching Servo4 OFF) is through here. There is no
            // other transition anywhere in this state machine that sets
            // reloadStep to MotorStopping.
            if (millis() - stepStartTime >= static_cast<uint32_t>(LOCK_SETTLE_TIME_MS)) {
                reloadStep = ReloadStep::MotorStopping;
            }
            break;

        case ReloadStep::MotorStopping:
            setMotorSwitchServo(SERVO4_OFF);
            reloadStep = ReloadStep::MotorOffSettling;
            stepStartTime = millis();
            break;

        case ReloadStep::MotorOffSettling:
            if (millis() - stepStartTime >= static_cast<uint32_t>(MOTOR_OFF_SETTLE_TIME_MS)) {
                reloadStep = ReloadStep::ReleasingSpool;
            }
            break;

        case ReloadStep::ReleasingSpool:
            setSpoolServo(SERVO3_RELEASED);
            state = CannonState::Loaded;
            break;
    }
}

}  // namespace

void cannon_fire_init() {
    servo1.attach(FIRING_PIN_SERVO_PIN);
    servo2.attach(FIRING_SUPPORT_SERVO_PIN);
    servo3.attach(SPOOL_SERVO_PIN);
    servo4.attach(MOTOR_SWITCH_SERVO_PIN);

    state = CannonState::Loaded;
    reloadStep = ReloadStep::SettingSpool;

    setFiringServos(SERVO1_DOWN, SERVO2_DOWN);
    setSpoolServo(SERVO3_RELEASED);
    setMotorSwitchServo(SERVO4_OFF);
}

void cannon_fire_update(bool fireRequested, bool reloadRequested) {
    switch (state) {
        case CannonState::Loaded:
            if (fireRequested) {
                fire();
            }
            break;

        case CannonState::Fired:
            if (reloadRequested) {
                startReload();
            }
            break;

        case CannonState::Reloading:
            // Both fireRequested and reloadRequested are ignored here: the
            // sequence must run to completion uninterrupted.
            updateReload();
            break;
    }
}

CannonState cannon_fire_get_state() {
    return state;
}

ReloadStep cannon_fire_get_reload_step() {
    return reloadStep;
}
