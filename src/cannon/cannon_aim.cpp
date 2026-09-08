#include "cannon_aim.h"

#include "config/pins.h"
#include "util/stepped_servo.h"

namespace {
SteppedServo aimServo;
}  // namespace

void cannon_aim_init() {
    aimServo.attach(AIM_SERVO_PIN, 1.5f, 90.0f);
}

void cannon_aim_update(bool aimUp, bool aimDown) {
    aimServo.update(aimUp, aimDown);
}

float cannon_aim_angle() {
    return aimServo.currentAngle();
}
