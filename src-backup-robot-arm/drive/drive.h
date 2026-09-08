#pragma once

// -----------------------------------------------------------------------------
// drive
//
// Drives the car's throttle/brake (Traxxas XL-5 ESC) and steering (Traxxas
// 2056 servo). Takes plain analog values, not ControllerInput or Bluepad32
// types — it has no idea driveForward/driveBackward/steerAxis came from
// R2/L2/left-stick on a gamepad, or from anything else.
// -----------------------------------------------------------------------------

// Attaches the ESC and steering servo, and arms the ESC. Call once from
// setup().
void drive_init();

// Call once per loop() iteration.
//   driveForward  - R2 trigger depth, 0..1023 (throttle)
//   driveBackward - L2 trigger depth, 0..1023 (brake/reverse)
//   steerAxis     - left stick X, raw Bluepad32 range, roughly -511..512
//
// driveForward and driveBackward are combined into a net signed throttle
// (forward - backward) before being written to the ESC, so holding both at
// once nets out to whichever is stronger rather than leaving the ESC in an
// undefined state.
void drive_update(int driveForward, int driveBackward, int steerAxis);

// Forces the ESC and steering servo to neutral/center immediately. Exposed
// for the eventual failsafe module to call.
void drive_stop();
