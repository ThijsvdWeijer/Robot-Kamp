#pragma once

// Cannon barrel elevation. Thin wrapper around a SteppedServo on
// AIM_SERVO_PIN — this module takes plain booleans, not ControllerInput or
// Bluepad32 types, and knows nothing about anything else in the project.

// Attaches the aim servo. Call once from setup().
void cannon_aim_init();

// Call once per loop() iteration. aimUp increases the angle, aimDown
// decreases it; both or neither holds steady.
void cannon_aim_update(bool aimUp, bool aimDown);

// Returns the barrel's currently tracked elevation angle.
float cannon_aim_angle();
