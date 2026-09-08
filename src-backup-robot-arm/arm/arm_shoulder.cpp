#include "arm_shoulder.h"

#include "config/pins.h"
#include "util/stepped_servo.h"

namespace {
SteppedServo shoulderServo;
}  // namespace

void arm_shoulder_init() {
    shoulderServo.attach(SHOULDER_SERVO_PIN, 1.10f, 90.0f);
}

void arm_shoulder_update(bool up, bool down) {
    shoulderServo.update(up, down);
}

float arm_shoulder_angle() {
    return shoulderServo.currentAngle();
}
