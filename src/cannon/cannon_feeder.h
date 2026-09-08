#pragma once

// -----------------------------------------------------------------------------
// cannon_feeder
//
// Drives the ball feeder servo through one push-and-return cycle per fire
// command: rest (0 deg) -> pushed (180 deg) -> rest. This is a non-blocking
// state machine driven entirely by cannon_feeder_update() every loop()
// iteration — it uses millis() to time its hold periods, never delay(), so
// nothing else in the program stalls while a shot is in progress.
//
// This module has no knowledge of Bluepad32 or ControllerInput beyond the
// single fireRequested boolean passed into cannon_feeder_update() — it has
// no idea what button that came from.
// -----------------------------------------------------------------------------

// Attaches the feeder servo on FEEDER_SERVO_PIN, moves it to the rest
// position, and resets the state machine to idle. Call once from setup().
void cannon_feeder_init();

// Call once per loop() iteration. fireRequested means "a fire command has
// arrived this tick" — it's only acted on while the state machine is idle.
// While a cycle is already in progress, fireRequested is ignored, so a
// held or repeated button press can't interrupt an in-flight cycle.
void cannon_feeder_update(bool fireRequested);

// True only while the state machine is idle (servo at rest, ready to
// accept the next fire command).
bool cannon_feeder_is_idle();

// Forces the feeder servo back to rest and the state machine to idle
// immediately, regardless of what it was doing (including mid-push). For
// the failsafe module to call: unlike the normal push/return cycle, this
// does not wait out the hold timers, since "controller just went away, and
// this thing is fed by a physical trigger link with no other way to
// silence it" overrides the usual "don't interrupt a throw" caution.
void cannon_feeder_stop();
