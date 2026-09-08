#pragma once

// Wrist joint. Thin wrapper around a SteppedServo on WRIST_SERVO_PIN (the
// reused Traxxas 2056 — mechanically still a standard 0-180° servo, so no
// special-casing needed) — this module takes plain booleans, not
// ControllerInput or Bluepad32 types, and knows nothing about any other
// joint.

// Attaches the wrist servo. Call once from setup().
void arm_wrist_init();

// Call once per loop() iteration. rotateA increases the angle, rotateB
// decreases it (an internal implementation choice — this module doesn't
// know or care that these correspond to square/circle on the controller);
// both or neither holds steady.
void arm_wrist_update(bool rotateA, bool rotateB);

// Returns the wrist's currently tracked angle.
float arm_wrist_angle();
