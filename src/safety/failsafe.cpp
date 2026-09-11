#include "failsafe.h"

#include <Arduino.h>

#include "base/arm_base.h"
#include "cannon/cannon_aim.h"
#include "cannon/cannon_feeder.h"
#include "cannon/cannon_power.h"
#include "drive/drive.h"

bool failsafe_check(const ControllerInput& input) {
    if (!input.connected) {
        return false;
    }
    if (millis() - input.lastUpdateMs > FAILSAFE_TIMEOUT_MS) {
        return false;
    }
    return true;
}

void failsafe_trigger() {

    drive_stop();
    arm_base_stop();

    cannon_aim_update(false, false);

    cannon_power_stop();
    cannon_feeder_stop();

    static uint32_t lastWarnMs = 0;
    uint32_t now = millis();
    if (now - lastWarnMs > 1000) {
        lastWarnMs = now;
        Serial.println("[FAILSAFE] No valid controller input -> neutral");
    }
}
