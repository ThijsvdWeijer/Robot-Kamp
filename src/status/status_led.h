#pragma once

#include "cannon/cannon_fire.h"

void status_led_init();

// Sets the RGB LED color for the given cannon state:
// Loaded -> green, Reloading -> orange, Fired -> red.
void status_led_update(CannonState state);
