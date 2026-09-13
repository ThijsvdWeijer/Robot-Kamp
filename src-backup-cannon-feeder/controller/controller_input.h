#pragma once

#include <stdint.h>

struct ControllerInput {
    bool connected;

    // L2 / R2 trigger depth range is 0 (not pressed) to 1023 (fully pressed).
    int driveBackward;  // L2 (brake)
    int driveForward;   // R2 (throttle)

    bool armBaseLeft;   // L1
    bool armBaseRight;  // R1

    bool feederPush;           // Circle -- jogs the feeder servo toward the push position
    bool feederReturn;         // Cross / X -- jogs the feeder servo toward the rest position
    bool aimUp;                // D-pad up
    bool aimDown;              // D-pad down
    bool cannonPowerButton;    // Triangle -- toggles the cannon's manual ON/OFF switch

    // Left stick X axis raw range is -511..512.
    int steerAxis;

    uint32_t lastUpdateMs;
};


void controller_init();


ControllerInput controller_update();
