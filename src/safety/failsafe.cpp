#include "failsafe.h"

#include <Arduino.h>

#include "base/arm_base.h"
#include "cannon/cannon_aim.h"
#include "cannon/cannon_feeder.h"
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
    // Velocity-based actuators have no "current position" to preserve — the
    // only safe state for them is stopped, so they're forced to neutral
    // outright.
    drive_stop();
    arm_base_stop();

    // Position-based: hold wherever it currently is rather than snap to a
    // hardcoded angle, which could slam the barrel into something.
    cannon_aim_update(false, false);

    // The cannon's launcher itself is armed by a physical link with no
    // software off switch, so the feeder is the only thing standing
    // between "controller lost" and "it keeps firing." Force it back to
    // rest immediately, even mid-push, rather than letting an in-flight
    // cycle finish on its own — no new ball may get fed while unsafe no
    // matter what state it was in when the signal dropped.
    cannon_feeder_stop();

    static uint32_t lastWarnMs = 0;
    uint32_t now = millis();
    if (now - lastWarnMs > 1000) {
        lastWarnMs = now;
        Serial.println("[FAILSAFE] No valid controller input -> neutral");
    }
}
