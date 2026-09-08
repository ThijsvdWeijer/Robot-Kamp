#include "controller_input.h"

#include <Bluepad32.h>


namespace {

ControllerPtr connectedController = nullptr;

void onConnectedController(ControllerPtr ctl) {
    if (connectedController == nullptr) {
        connectedController = ctl;
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    if (connectedController == ctl) {
        connectedController = nullptr;
    }
}

}

void controller_init() {
    BP32.setup(&onConnectedController, &onDisconnectedController);
}

ControllerInput controller_update() {
    BP32.update();

    // Zero-initializing here gives every field its safe/neutral default
    // (false / 0) up front, so the "not connected" return path below doesn't
    // need to set each field individually.
    ControllerInput input{};
    input.lastUpdateMs = millis();

    if (connectedController == nullptr || !connectedController->isConnected()) {
        return input;
    }

    ControllerPtr ctl = connectedController;

    input.connected = true;

    input.driveBackward = ctl->brake();     // L2, raw range 0..1023
    input.driveForward = ctl->throttle();   // R2, raw range 0..1023

    input.armBaseLeft = ctl->l1();
    input.armBaseRight = ctl->r1();

    // Bluepad32 names face buttons by pad position (a/b/x/y), not by the
    // PlayStation glyph printed on them. On a DualSense: a() = Cross.
    input.fireRequested = ctl->a();  // Cross / X

    input.aimUp = ctl->dpad() & DPAD_UP;
    input.aimDown = ctl->dpad() & DPAD_DOWN;

    input.steerAxis = ctl->axisX();  // raw range roughly -511..512

    return input;
}