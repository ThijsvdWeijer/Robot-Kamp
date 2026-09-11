#include "stepped_servo.h"

void SteppedServo::attach(int pin, float stepDegreesPerTick, float startAngle) {
    stepDegreesPerTick_ = stepDegreesPerTick;
    angle_ = constrain(startAngle, 0.0f, 180.0f);

    servo_.attach(pin);
    servo_.write(angle_);
}

void SteppedServo::update(bool increasePressed, bool decreasePressed) {
    if (increasePressed && !decreasePressed) {
        angle_ += stepDegreesPerTick_;
    } else if (decreasePressed && !increasePressed) {
        angle_ -= stepDegreesPerTick_;
    }
    
    angle_ = constrain(angle_, 0.0f, 180.0f);
    servo_.write(angle_);
}

float SteppedServo::currentAngle() const {
    return angle_;
}

void SteppedServo::setAngle(float angle) {
    angle_ = constrain(angle, 0.0f, 180.0f);
    servo_.write(angle_);
}
