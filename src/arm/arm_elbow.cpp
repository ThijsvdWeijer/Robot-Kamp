#include "arm_elbow.h"

#include "config/pins.h"
#include "util/stepped_servo.h"

namespace {
SteppedServo elbowServo;
}  // namespace

void arm_elbow_init() {
    elbowServo.attach(ELBOW_SERVO_PIN, 1.10f, 90.0f);
}

void arm_elbow_update(bool up, bool down) {
    elbowServo.update(up, down);
}

float arm_elbow_angle() {
    return elbowServo.currentAngle();
}
