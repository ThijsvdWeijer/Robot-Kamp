#include "arm_gripper.h"

#include "config/pins.h"
#include "util/stepped_servo.h"

namespace {
SteppedServo gripperServo;
}  // namespace

void arm_gripper_init() {
    // Unlike the other joints, the gripper's 0/180 endpoints represent
    // closed/open rather than a symmetric center, so it starts at 0.0
    // (fully closed) instead of 90.0.
    gripperServo.attach(GRIPPER_SERVO_PIN, 1.10f, 0.0f);
}

void arm_gripper_update(bool open, bool close) {
    // open -> increase, close -> decrease; arbitrary but consistent.
    gripperServo.update(open, close);
}

float arm_gripper_angle() {
    return gripperServo.currentAngle();
}
