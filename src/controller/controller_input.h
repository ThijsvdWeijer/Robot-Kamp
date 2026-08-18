#pragma once

#include <stdint.h>

// -----------------------------------------------------------------------------
// ControllerInput
//
// This struct is the ONLY thing that leaves the controller module. Every
// other module (drive, arm, safety, ...) reads controller state exclusively
// through it — none of them may include Bluepad32.h or touch a
// ControllerPtr. That keeps Bluepad32 confined to controller_input.cpp: if
// the controller library is ever swapped out, only that one .cpp file needs
// to change, not every module that cares about button state.
// -----------------------------------------------------------------------------
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
