#include "cannon_power.h"

#include <Arduino.h>
#include <ESP32Servo.h>

#include "config/pins.h"

namespace {

constexpr int POWER_OFF_ANGLE = 0;
constexpr int POWER_ON_ANGLE = 180;

Servo powerServo;
bool isOn = false;
bool wasButtonPressed = false;

void moveTo(bool on) {
    powerServo.write(on ? POWER_ON_ANGLE : POWER_OFF_ANGLE);
    isOn = on;
}

}  // namespace

void cannon_power_init() {
    powerServo.attach(CANNON_POWER_SERVO_PIN);
    moveTo(false);
    wasButtonPressed = false;
}

void cannon_power_update(bool powerButtonPressed) {
    bool risingEdge = powerButtonPressed && !wasButtonPressed;
    wasButtonPressed = powerButtonPressed;

    if (risingEdge) {
        moveTo(!isOn);
        Serial.println(isOn ? "[CANNON] Power -> ON" : "[CANNON] Power -> OFF");
    }
}

bool cannon_power_is_on() {
    return isOn;
}

void cannon_power_stop() {
    if (isOn) {
        Serial.println("[CANNON] Failsafe -> power OFF");
    }
    moveTo(false);
}
