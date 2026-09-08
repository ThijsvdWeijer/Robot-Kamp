#pragma once



// Attaches the base ESC and arms it. Call once from setup().
void arm_base_init();

// Call once per loop() iteration. If spinLeft is true, drives the ESC at a
// fixed reverse speed; if spinRight is true, drives at a fixed forward
// speed. If both are true (shouldn't normally happen) or neither is true,
// holds neutral. Never leaves the ESC in an undefined state.
void arm_base_update(bool spinLeft, bool spinRight);

// Forces the ESC to neutral immediately. Exposed for the eventual failsafe
// module to call.
void arm_base_stop();
