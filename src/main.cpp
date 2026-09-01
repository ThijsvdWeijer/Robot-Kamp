#include <Arduino.h>

#include "arm/arm_base.h"
#include "arm/arm_elbow.h"
#include "arm/arm_gripper.h"
#include "arm/arm_shoulder.h"
#include "arm/arm_wrist.h"
#include "controller/controller_input.h"
#include "drive/drive.h"
#include "safety/failsafe.h"

namespace {

ControllerInput previousInput{};
bool havePreviousInput = false;

// Joint angles are tracked separately from previousInput: while a
// direction is held, the ControllerInput fields stay constant but the
// angle keeps advancing every tick, so angle changes need their own
// print trigger.
float previousShoulderAngle = 0.0f;
float previousElbowAngle = 0.0f;
float previousWristAngle = 0.0f;
float previousGripperAngle = 0.0f;
bool haveJointAngles = false;

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
        "steer=%4d | t=%lu | "
        "shoulder=%.1f elbow=%.1f wrist=%.1f gripper=%.1f\n",
        input.connected, input.driveBackward, input.driveForward,
        input.armBaseLeft, input.armBaseRight,
        input.shoulderUp, input.shoulderDown, input.gripperOpen, input.gripperClose,
        input.elbowUp, input.elbowDown, input.wristRotateA, input.wristRotateB,
        input.steerAxis, static_cast<unsigned long>(input.lastUpdateMs),
        arm_shoulder_angle(), arm_elbow_angle(), arm_wrist_angle(), arm_gripper_angle());
}

}  

void setup() {
    Serial.begin(115200);
    controller_init();
    arm_base_init();
    drive_init();
    arm_shoulder_init();
    arm_elbow_init();
    arm_wrist_init();
    arm_gripper_init();
}

void loop() {
    ControllerInput input = controller_update();

    if (failsafe_check(input)) {
        drive_update(input.driveForward, input.driveBackward, input.steerAxis);
        arm_base_update(input.armBaseLeft, input.armBaseRight);
        arm_shoulder_update(input.shoulderUp, input.shoulderDown);
        arm_elbow_update(input.elbowUp, input.elbowDown);
        arm_wrist_update(input.wristRotateA, input.wristRotateB);
        arm_gripper_update(input.gripperOpen, input.gripperClose);
    } else {
        failsafe_trigger();
    }

    float shoulderAngle = arm_shoulder_angle();
    float elbowAngle = arm_elbow_angle();
    float wristAngle = arm_wrist_angle();
    float gripperAngle = arm_gripper_angle();

    bool anglesChanged = !haveJointAngles ||
                          shoulderAngle != previousShoulderAngle ||
                          elbowAngle != previousElbowAngle ||
                          wristAngle != previousWristAngle ||
                          gripperAngle != previousGripperAngle;

    if (!havePreviousInput || inputChanged(input, previousInput) || anglesChanged) {
        printInput(input);
        previousInput = input;
        havePreviousInput = true;

        previousShoulderAngle = shoulderAngle;
        previousElbowAngle = elbowAngle;
        previousWristAngle = wristAngle;
        previousGripperAngle = gripperAngle;
        haveJointAngles = true;
    }

    delay(10);
}
