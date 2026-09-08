#include "failsafe.h"

#include <Arduino.h>

#include "arm/arm_base.h"
#include "arm/arm_elbow.h"
#include "arm/arm_gripper.h"
#include "arm/arm_shoulder.h"
#include "arm/arm_wrist.h"
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
  
  
    arm_shoulder_update(false, false);
    arm_elbow_update(false, false);
    arm_wrist_update(false, false);
    arm_gripper_update(false, false);
  
    static uint32_t lastWarnMs = 0;
    uint32_t now = millis();
    if (now - lastWarnMs > 1000) {
        lastWarnMs = now;
        Serial.println("[FAILSAFE] No valid controller input -> neutral");
    }
} 