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

    // No special-case handling for a disconnect mid-reload: cannon_fire is
    // driven like every other actuator here (neutral inputs), so an
    // in-progress reload sequence keeps ticking forward via this call
    // rather than being aborted or reset. Revisit if that's not desired.
    cannon_fire_update(false, false);

    static uint32_t lastWarnMs = 0;
    uint32_t now = millis();
    if (now - lastWarnMs > 1000) {
        lastWarnMs = now;
        Serial.println("[FAILSAFE] No valid controller input -> neutral");
    }
}
