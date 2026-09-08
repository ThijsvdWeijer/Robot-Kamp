#pragma once

#include <ESP32Servo.h>

// -----------------------------------------------------------------------------
// SteppedServo
//
// Shared behavior for a servo joint that holds an angle (0-180°) and is
// nudged up or down by a step size each tick, rather than driven to an
// absolute position. This is the common pattern behind the shoulder, elbow,
// wrist, and gripper joints: one input increases the angle, another
// decreases it, and holding neither (or both) just leaves it where it is.
//
// This class knows nothing about Bluepad32, ControllerInput, or which
// physical joint it represents — just a pin, a step size, and a tracked
// angle. It's a generic building block, not tied to any specific joint.
//
// Intended usage: each joint module (arm_shoulder.cpp, arm_elbow.cpp, ...)
// creates one static SteppedServo instance, calls attach() once from its
// own init function with that joint's pin and step size, and calls update()
// once per loop() passing whichever two ControllerInput booleans correspond
// to that joint's up/down direction.
// -----------------------------------------------------------------------------
class SteppedServo {
public:
    // Attaches the servo to the given pin, sets the tracked angle to
    // startAngle, and immediately writes it to the physical servo.
    // stepDegreesPerTick is how many degrees the angle moves per update()
    // call while a direction is held.
    void attach(int pin, float stepDegreesPerTick, float startAngle = 90.0f);

    // Call once per loop() iteration. Increases the angle by
    // stepDegreesPerTick if increasePressed is true (and decreasePressed is
    // not), decreases it if decreasePressed is true (and increasePressed is
    // not). If both are true or neither is true, the angle holds steady.
    // The result is always clamped to [0, 180] and written to the servo.
    void update(bool increasePressed, bool decreasePressed);

    // Returns the currently tracked angle.
    float currentAngle() const;

    // Directly sets the tracked angle (clamped to [0, 180]) and writes it
    // to the servo immediately.
    void setAngle(float angle);

private:
    Servo servo_;
    float angle_ = 90.0f;
    float stepDegreesPerTick_ = 0.0f;
};
