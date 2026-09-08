#pragma once

// Shoulder joint. Thin wrapper around a SteppedServo on SHOULDER_SERVO_PIN
// — this module takes plain booleans, not ControllerInput or Bluepad32
// types, and knows nothing about any other joint.

// Attaches the shoulder servo. Call once from setup().
void arm_shoulder_init();

// Call once per loop() iteration. up increases the angle, down decreases
// it; both or neither holds steady.
void arm_shoulder_update(bool up, bool down);

// Returns the shoulder's currently tracked angle.
float arm_shoulder_angle();
