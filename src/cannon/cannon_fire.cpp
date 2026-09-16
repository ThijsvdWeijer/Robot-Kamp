#include "cannon_fire.h"

#include <Arduino.h>
#include <ESP32Servo.h>

#include "config/pins.h"

int SERVO1_DOWN = 140;
int SERVO1_UP   = 180;

int SERVO2_DOWN = 180;
int SERVO2_UP   = 70;

int SERVO3_SET      = 130;
int SERVO3_RELEASED = 0;

int SERVO4_OFF = 180;
int SERVO4_ON  = 165;


int RELOAD_MOTOR_TIME_MS = 4500;

int LOCK_SETTLE_TIME_MS = 1000;

int SPOOL_SETTLE_TIME_MS = 1000;

int MOTOR_OFF_SETTLE_TIME_MS = 1000;

namespace {

Servo servo1;  // firing pin
Servo servo2;  // firing support
Servo servo3;  // spool control
Servo servo4;  // motor switch

CannonState state = CannonState::Loaded;
ReloadStep reloadStep = ReloadStep::SettingSpool;

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

}  

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
