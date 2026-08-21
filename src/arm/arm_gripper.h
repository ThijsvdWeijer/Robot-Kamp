#pragma once

// Gripper joint. Thin wrapper around a SteppedServo on GRIPPER_SERVO_PIN —
// this module takes plain booleans, not ControllerInput or Bluepad32
// types, and knows nothing about any other joint.

// Attaches the gripper servo. Call once from setup().
void arm_gripper_init();

// Call once per loop() iteration. open increases the angle, close
// decreases it (an internal implementation choice — this module doesn't
// know or care that these correspond to D-pad left/right on the
// controller); both or neither holds steady.
void arm_gripper_update(bool open, bool close);

// Returns the gripper's currently tracked angle.
float arm_gripper_angle();
