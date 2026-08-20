#include <Arduino.h>

#include "arm/arm_base.h"
#include "controller/controller_input.h"
#include "drive/drive.h"

namespace {

ControllerInput previousInput{};
bool havePreviousInput = false;

bool inputChanged(const ControllerInput& a, const ControllerInput& b) {
    
    return a.connected != b.connected ||
           a.driveBackward != b.driveBackward ||
           a.driveForward != b.driveForward ||
           a.armBaseLeft != b.armBaseLeft ||
           a.armBaseRight != b.armBaseRight ||
           a.shoulderUp != b.shoulderUp ||
           a.shoulderDown != b.shoulderDown ||
           a.gripperOpen != b.gripperOpen ||
           a.gripperClose != b.gripperClose ||
           a.elbowUp != b.elbowUp ||
           a.elbowDown != b.elbowDown ||
           a.wristRotateA != b.wristRotateA ||
           a.wristRotateB != b.wristRotateB ||
           a.steerAxis != b.steerAxis;
}

void printInput(const ControllerInput& input) {
    Serial.printf(
        "connected=%d | L2=%4d R2=%4d | L1=%d R1=%d | "
        "dpad U=%d D=%d L=%d R=%d | tri=%d cross=%d sq=%d cir=%d | "
        "steer=%4d | t=%lu\n",
        input.connected, input.driveBackward, input.driveForward,
        input.armBaseLeft, input.armBaseRight,
        input.shoulderUp, input.shoulderDown, input.gripperOpen, input.gripperClose,
        input.elbowUp, input.elbowDown, input.wristRotateA, input.wristRotateB,
        input.steerAxis, static_cast<unsigned long>(input.lastUpdateMs));
}

}  

void setup() {
    Serial.begin(115200);
    controller_init();
    arm_base_init();
    drive_init();
}

void loop() {
    ControllerInput input = controller_update();
    arm_base_update(input.armBaseLeft, input.armBaseRight);
    drive_update(input.driveForward, input.driveBackward, input.steerAxis);

    if (!havePreviousInput || inputChanged(input, previousInput)) {
        printInput(input);
        previousInput = input;
        havePreviousInput = true;
    }

    delay(10);
}
