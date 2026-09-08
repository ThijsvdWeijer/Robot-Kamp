#include "cannon_feeder.h"

#include <Arduino.h>
#include <ESP32Servo.h>

#include "config/pins.h"

namespace {

// Full sweep endpoints for the feeder servo.
constexpr int FEEDER_REST_ANGLE = 0;    // loaded / ready position
constexpr int FEEDER_PUSH_ANGLE = 180;  // fully pushed position

// Placeholder, needs physical tuning: how long to hold at the pushed
// position before returning, giving the ball time to fully clear the
// pusher.
constexpr uint32_t FEEDER_PUSH_HOLD_MS = 150;

// Placeholder, needs physical tuning: how long to wait after returning to
// rest before accepting the next fire command, giving the ball time to
// roll clear before the pusher sweeps back through.
constexpr uint32_t FEEDER_RETURN_HOLD_MS = 150;

// Placeholder, needs physical tuning: optional delay (ms) between each
// one-degree step of a sweep, for slowing the servo down later. 0 (the
// current default) means no artificial slowing — moveServoTo() below does
// a single direct write() with no visible slowdown.
constexpr int FEEDER_MOVE_DELAY_MS = 0;

enum class FeederState { Idle, Pushing, Returning };

Servo feederServo;
FeederState state = FeederState::Idle;
uint32_t stateEnteredMs = 0;

// Single choke point for commanding the feeder servo to a target angle.
// Right now this is always a direct, full-speed write(). If
// FEEDER_MOVE_DELAY_MS is ever set above 0, this is the one place to add a
// stepped sweep (advancing one degree per loop() tick via millis(), not a
// blocking delay() loop, to stay consistent with the rest of this module's
// non-blocking design) instead of jumping straight to the target angle.
void moveServoTo(int angle) {
    if (FEEDER_MOVE_DELAY_MS > 0) {
        // Placeholder: stepped slow-sweep behavior not implemented yet.
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
