#pragma once

#include <stdint.h>

struct ControllerInput {
    bool connected;

    // L2 / R2 trigger depth range is 0 (not pressed) to 1023 (fully pressed).
    int driveBackward;  // L2 (brake)
    int driveForward;   // R2 (throttle)

    bool armBaseLeft;   // L1
    bool armBaseRight;  // R1

    bool shoulderUp;    // D-pad up
    bool shoulderDown;  // D-pad down
    bool gripperOpen;   // D-pad left
    bool gripperClose;  // D-pad right

    bool elbowUp;        // Triangle
    bool elbowDown;      // Cross
    bool wristRotateA;   // Square
    bool wristRotateB;   // Circle

    // Left stick X axis raw range is -511..512.
    int steerAxis;

    uint32_t lastUpdateMs;
};


void controller_init();


ControllerInput controller_update();
