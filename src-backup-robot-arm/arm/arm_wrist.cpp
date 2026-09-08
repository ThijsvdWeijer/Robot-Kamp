#include "arm_wrist.h"

#include "config/pins.h"
#include "util/stepped_servo.h"

namespace {
SteppedServo wristServo;
}  // namespace

void arm_wrist_init() {
    wristServo.attach(WRIST_SERVO_PIN, 1.10f, 90.0f);
}

void arm_wrist_update(bool rotateA, bool rotateB) {
    // rotateA -> increase, rotateB -> decrease; arbitrary but consistent.
    wristServo.update(rotateA, rotateB);
}

float arm_wrist_angle() {
    return wristServo.currentAngle();
}
