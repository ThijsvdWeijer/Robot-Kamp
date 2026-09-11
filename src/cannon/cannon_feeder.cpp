#include "cannon_feeder.h"

#include <Arduino.h>
#include <ESP32Servo.h>

#include "config/pins.h"

namespace {

constexpr int FEEDER_REST_ANGLE = 0;    // loaded / ready position
constexpr int FEEDER_PUSH_ANGLE = 180;  // fully pushed position


constexpr uint32_t FEEDER_PUSH_HOLD_MS = 150;

constexpr uint32_t FEEDER_RETURN_HOLD_MS = 150;


constexpr int FEEDER_MOVE_DELAY_MS = 0;

enum class FeederState { Idle, Pushing, Returning };

Servo feederServo;
FeederState state = FeederState::Idle;
uint32_t stateEnteredMs = 0;


void moveServoTo(int angle) {
    if (FEEDER_MOVE_DELAY_MS > 0) {
    }
    feederServo.write(angle);
}

void enterState(FeederState newState) {
    state = newState;
    stateEnteredMs = millis();
}

}  // namespace

void cannon_feeder_init() {
    feederServo.attach(FEEDER_SERVO_PIN);
    moveServoTo(FEEDER_REST_ANGLE);
    enterState(FeederState::Idle);
}

void cannon_feeder_update(bool fireRequested) {
    switch (state) {
        case FeederState::Idle:
            if (fireRequested) {
                moveServoTo(FEEDER_PUSH_ANGLE);
                Serial.println("[CANNON] Firing -> push");
                enterState(FeederState::Pushing);
            }
            break;

        case FeederState::Pushing:
            if (millis() - stateEnteredMs >= FEEDER_PUSH_HOLD_MS) {
                moveServoTo(FEEDER_REST_ANGLE);
                Serial.println("[CANNON] Firing -> return");
                enterState(FeederState::Returning);
            }
            break;

        case FeederState::Returning:
            if (millis() - stateEnteredMs >= FEEDER_RETURN_HOLD_MS) {
                Serial.println("[CANNON] Firing -> idle, ready");
                enterState(FeederState::Idle);
            }
            break;
    }
}

bool cannon_feeder_is_idle() {
    return state == FeederState::Idle;
}

void cannon_feeder_stop() {
    if (state != FeederState::Idle) {
        Serial.println("[CANNON] Failsafe -> forced idle, ready");
    }
    moveServoTo(FEEDER_REST_ANGLE);
    enterState(FeederState::Idle);
}
