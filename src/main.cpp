#include <Arduino.h>

#include "base/arm_base.h"
#include "cannon/cannon_aim.h"
#include "cannon/cannon_feeder.h"
#include "cannon/cannon_power.h"
#include "controller/controller_input.h"
#include "drive/drive.h"
#include "safety/failsafe.h"

namespace {

ControllerInput previousInput{};
bool havePreviousInput = false;

float previousAimAngle = 0.0f;
bool haveAimAngle = false;

bool feederWasIdle = true;
bool haveFeederState = false;

bool powerWasOn = false;
bool havePowerState = false;

bool inputChanged(const ControllerInput& a, const ControllerInput& b) {
    return a.connected != b.connected ||
           a.driveBackward != b.driveBackward ||
           a.driveForward != b.driveForward ||
           a.armBaseLeft != b.armBaseLeft ||
           a.armBaseRight != b.armBaseRight ||
           a.fireRequested != b.fireRequested ||
           a.aimUp != b.aimUp ||
           a.aimDown != b.aimDown ||
           a.cannonPowerButton != b.cannonPowerButton ||
           a.steerAxis != b.steerAxis;
}

void printInput(const ControllerInput& input, float aimAngle) {
    Serial.printf(
        "connected=%d | L2=%4d R2=%4d | L1=%d R1=%d | fire=%d aimU=%d aimD=%d pwrBtn=%d | "
        "steer=%4d | t=%lu | aim=%.1f | feederIdle=%d | cannonPower=%d\n",
        input.connected, input.driveBackward, input.driveForward,
        input.armBaseLeft, input.armBaseRight,
        input.fireRequested, input.aimUp, input.aimDown, input.cannonPowerButton,
        input.steerAxis, static_cast<unsigned long>(input.lastUpdateMs),
        aimAngle, cannon_feeder_is_idle(), cannon_power_is_on());
}

}  // namespace

void setup() {
    Serial.begin(115200);
    controller_init();
    drive_init();
    arm_base_init();
    cannon_aim_init();
    cannon_feeder_init();
    cannon_power_init();
}

void loop() {
    ControllerInput input = controller_update();

    bool safeToAct = failsafe_check(input);

    if (safeToAct) {
        drive_update(input.driveForward, input.driveBackward, input.steerAxis);
        arm_base_update(input.armBaseLeft, input.armBaseRight);
        cannon_aim_update(input.aimUp, input.aimDown);
        cannon_feeder_update(input.fireRequested);
        cannon_power_update(input.cannonPowerButton);
    } else {
        // failsafe_trigger() forces the switch OFF and the feeder to rest
        // immediately (even mid-push) rather than letting anything finish
        // on its own, since the cannon's launcher has no software off
        // switch other than this servo-flipped toggle.
        failsafe_trigger();
    }

    float aimAngle = cannon_aim_angle();
    bool feederIdle = cannon_feeder_is_idle();
    bool powerIsOn = cannon_power_is_on();

    bool stateChanged = !havePreviousInput || inputChanged(input, previousInput) ||
                         !haveAimAngle || aimAngle != previousAimAngle ||
                         !haveFeederState || feederIdle != feederWasIdle ||
                         !havePowerState || powerIsOn != powerWasOn;

    if (stateChanged) {
        printInput(input, aimAngle);
        previousInput = input;
        havePreviousInput = true;
        previousAimAngle = aimAngle;
        haveAimAngle = true;
        feederWasIdle = feederIdle;
        haveFeederState = true;
        powerWasOn = powerIsOn;
        havePowerState = true;
    }

    delay(10);
}
