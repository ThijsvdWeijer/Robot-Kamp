#include <Arduino.h>

#include "base/arm_base.h"
#include "cannon/cannon_fire.h"
#include "controller/controller_input.h"
#include "drive/drive.h"
#include "safety/failsafe.h"
#include "status/status_led.h"

namespace {

const char* cannonStateName(CannonState state) {
    switch (state) {
        case CannonState::Loaded:    return "Loaded";
        case CannonState::Fired:     return "Fired";
        case CannonState::Reloading: return "Reloading";
    }
    return "?";
}

const char* reloadStepName(ReloadStep step) {
    switch (step) {
        case ReloadStep::SettingSpool:    return "SettingSpool";
        case ReloadStep::SpoolSettling:   return "SpoolSettling";
        case ReloadStep::MotorRunning:    return "MotorRunning";
        case ReloadStep::Locking:         return "Locking";
        case ReloadStep::MotorStopping:   return "MotorStopping";
        case ReloadStep::MotorOffSettling: return "MotorOffSettling";
        case ReloadStep::ReleasingSpool:  return "ReleasingSpool";
    }
    return "?";
}

ControllerInput previousInput{};
bool havePreviousInput = false;

CannonState previousCannonState = CannonState::Loaded;
ReloadStep previousReloadStep = ReloadStep::SettingSpool;
bool haveCannonState = false;

bool inputChanged(const ControllerInput& a, const ControllerInput& b) {
    return a.connected != b.connected ||
           a.driveBackward != b.driveBackward ||
           a.driveForward != b.driveForward ||
           a.armBaseLeft != b.armBaseLeft ||
           a.armBaseRight != b.armBaseRight ||
           a.fireRequested != b.fireRequested ||
           a.reloadRequested != b.reloadRequested ||
           a.steerAxis != b.steerAxis;
}

void printInput(const ControllerInput& input) {
    Serial.printf(
        "connected=%d | L2=%4d R2=%4d | L1=%d R1=%d | fire=%d reload=%d | "
        "steer=%4d | t=%lu\n",
        input.connected, input.driveBackward, input.driveForward,
        input.armBaseLeft, input.armBaseRight,
        input.fireRequested, input.reloadRequested,
        input.steerAxis, static_cast<unsigned long>(input.lastUpdateMs));
}

}  // namespace

void setup() {
    Serial.begin(115200);
    controller_init();
    drive_init();
    arm_base_init();
    cannon_fire_init();
    status_led_init();
}

void loop() {
    ControllerInput input = controller_update();

    bool safeToAct = failsafe_check(input);

    if (safeToAct) {
        drive_update(input.driveForward, input.driveBackward, input.steerAxis);
        arm_base_update(input.armBaseLeft, input.armBaseRight);
        cannon_fire_update(input.fireRequested, input.reloadRequested);
    } else {
        failsafe_trigger();
    }

    CannonState cannonState = cannon_fire_get_state();
    ReloadStep reloadStep = cannon_fire_get_reload_step();
    status_led_update(cannonState);

    bool stateChanged = !havePreviousInput || inputChanged(input, previousInput) ||
                         !haveCannonState || cannonState != previousCannonState ||
                         reloadStep != previousReloadStep;

    if (stateChanged) {
        printInput(input);
        Serial.printf("[CANNON] state=%s reloadStep=%s\n",
                       cannonStateName(cannonState), reloadStepName(reloadStep));

        previousInput = input;
        havePreviousInput = true;
        previousCannonState = cannonState;
        previousReloadStep = reloadStep;
        haveCannonState = true;
    }

    delay(10);
}
