#include "cannon_aim.h"

#include <Arduino.h>
#include <ESP32Servo.h>

#include "config/pins.h"

namespace {

constexpr float AIM_DOWN_ANGLE = 0.0f;
constexpr float AIM_UP_ANGLE = 130.0f;

Servo aimServo;
float currentAngle = AIM_DOWN_ANGLE;

void moveTo(float angle) {
  if (angle == currentAngle) {
    return;
  }
  aimServo.write(static_cast<int>(angle));
  currentAngle = angle;
  Serial.printf("[CANNON] Aim -> %.0f graden\n", currentAngle);
}

}  // namespace

void cannon_aim_init() {
  aimServo.attach(CANNON_AIM_SERVO_PIN);
  currentAngle = AIM_DOWN_ANGLE;
  aimServo.write(static_cast<int>(currentAngle));
}

void cannon_aim_update(bool upPressed, bool downPressed) {
  if (upPressed == downPressed) {
    return;  // niets of allebei -> blijf staan
  }
  moveTo(upPressed ? AIM_UP_ANGLE : AIM_DOWN_ANGLE);
}

float cannon_aim_angle() {
  return currentAngle;
}