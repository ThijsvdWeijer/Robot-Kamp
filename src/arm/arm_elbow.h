#pragma once

// Elbow joint. Thin wrapper around a SteppedServo on ELBOW_SERVO_PIN — this
// module takes plain booleans, not ControllerInput or Bluepad32 types, and
// knows nothing about any other joint.

// Attaches the elbow servo. Call once from setup().
void arm_elbow_init();

// Call once per loop() iteration. up increases the angle, down decreases
// it; both or neither holds steady.
void arm_elbow_update(bool up, bool down);

// Returns the elbow's currently tracked angle.
float arm_elbow_angle();
