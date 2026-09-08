#pragma once

#include <stdint.h>

#include "controller/controller_input.h"

// -----------------------------------------------------------------------------
// failsafe
//
// Single gatekeeper for whether it's safe to act on the current controller
// input. None of the other modules (drive, arm_base, cannon_aim,
// cannon_feeder) know or care whether their input is trustworthy — they
// just do what they're told each time their update() function is called.
// This module decides, once per loop() iteration, which case applies.
// -----------------------------------------------------------------------------

// How stale controller data (input.lastUpdateMs) is allowed to get before
// failsafe triggers.
constexpr uint32_t FAILSAFE_TIMEOUT_MS = 300;

// Returns true if it is SAFE to act on this input (no failsafe condition is
// met), false if failsafe should trigger. Pure decision function — it only
// evaluates and returns, it never calls any stop/hold function itself, so
// it stays easy to unit-test and main.cpp stays in control of what
// actually happens.
bool failsafe_check(const ControllerInput& input);

// Forces every actuator into its safe state. Call this instead of the
// individual *_update() calls whenever failsafe_check() returns false.
void failsafe_trigger();
