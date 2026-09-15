#pragma once

#include <ESP32Servo.h>


class SteppedServo {
public:

    void attach(int pin, float stepDegreesPerTick, float startAngle = 90.0f);


    void update(bool increasePressed, bool decreasePressed);

    float currentAngle() const;

    void setAngle(float angle);

private:
    Servo servo_;
    float angle_ = 90.0f;
    float stepDegreesPerTick_ = 0.0f;
};
