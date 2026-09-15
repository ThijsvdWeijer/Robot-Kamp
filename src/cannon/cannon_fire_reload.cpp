#include "cannon_fire_internal.h"

#include <Arduino.h>

// Only valid from CannonState::Fired (enforced by the switch in
// cannon_fire_update(), the sole caller of this function). Transitions
// Fired -> Reloading. Per the mechanical sequence, setting the spool
// happens in this same call (the SettingSpool step is transient, not
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