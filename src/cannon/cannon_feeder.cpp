#include "cannon_feeder.h"

#include <Arduino.h>
#include <ESP32Servo.h>

#include "config/pins.h"

namespace {

constexpr int FEEDER_REST_ANGLE = 90;  // loaded / ready position
constexpr int FEEDER_PUSH_ANGLE = 140;  // fully pushed position

Servo feederServo;
int currentAngle = FEEDER_REST_ANGLE;

void moveTo(int angle) {
    if (angle == currentAngle) {
        return;
    }
    feederServo.write(angle);
    currentAngle = angle;
}

}  // namespace

void cannon_feeder_init() {
    feederServo.attach(FEEDER_SERVO_PIN);
    currentAngle = FEEDER_REST_ANGLE;
    feederServo.write(currentAngle);
}

void cannon_feeder_update(bool pushPressed, bool returnPressed) {
    if (pushPressed == returnPressed) {
        return;  // neither or both pressed -> hold current position
    }
    moveTo(pushPressed ? FEEDER_PUSH_ANGLE : FEEDER_REST_ANGLE);
}

bool cannon_feeder_is_idle() {
    return currentAngle == FEEDER_REST_ANGLE;
}

void cannon_feeder_stop() {
    moveTo(FEEDER_REST_ANGLE);
}
