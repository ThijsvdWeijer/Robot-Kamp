#include "cannon_fire_internal.h"

#include <Arduino.h>

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