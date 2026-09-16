#pragma once

#include <stdint.h>

struct ControllerInput {
    bool connected;

    int driveBackward;  // L2 (brake)
    int driveForward;   // R2 (throttle)

    bool armBaseLeft;   // L1
    bool armBaseRight;  // R1

    bool fireRequested;        // Cross / X -- fires the cannon
    bool reloadRequested;      // Triangle -- runs the reload sequence

    // Left stick X axis raw range is -511..512.
    int steerAxis;

    uint32_t lastUpdateMs;
};


void controller_init();


ControllerInput controller_update();
