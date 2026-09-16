#include "failsafe.h"

#include <Arduino.h>

#include "base/arm_base.h"
#include "cannon/cannon_fire.h"
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

    cannon_fire_update(false, false);

    static uint32_t lastWarnMs = 0;
    uint32_t now = millis();
    if (now - lastWarnMs > 1000) {
        lastWarnMs = now;
        Serial.println("[FAILSAFE] No valid controller input -> neutral");
    }
}
