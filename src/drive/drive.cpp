#include "drive.h"

#include <ESP32Servo.h>

#include "config/pins.h"

namespace {

constexpr int PWM_NEUTRAL_US = 1500;
constexpr int PWM_MIN_US = 1000;
constexpr int PWM_MAX_US = 2000;

// Max pulse deviation from neutral, in microseconds -- kept separate per
// actuator so capping the car's top speed doesn't also cap steering throw.
// Lower THROTTLE_HALF_RANGE_US to limit the car's max speed.
constexpr int THROTTLE_HALF_RANGE_US = 500;
constexpr int STEER_HALF_RANGE_US = 500;

constexpr int TRIGGER_MAX = 1023;

constexpr int STEER_AXIS_MAX = 512;


constexpr int STEER_DEADZONE = 20;     // raw left-stick X units
constexpr int THROTTLE_DEADZONE = 20;  // raw net-throttle units (forward - backward)

constexpr int ESC_ARM_DELAY_MS = 1000;

Servo esc;
Servo steeringServo;

int clampPulse(int pulseUs) {
    if (pulseUs < PWM_MIN_US) return PWM_MIN_US;
    if (pulseUs > PWM_MAX_US) return PWM_MAX_US;
    return pulseUs;
}

int applyDeadzone(int value, int deadzone) {
    return (value > -deadzone && value < deadzone) ? 0 : value;
}

}  // namespace

void drive_init() {
    esc.attach(ESC_PIN);
    steeringServo.attach(STEERING_PIN);

    esc.writeMicroseconds(PWM_NEUTRAL_US);
    steeringServo.writeMicroseconds(PWM_NEUTRAL_US);
    delay(ESC_ARM_DELAY_MS);
}

void drive_update(int driveForward, int driveBackward, int steerAxis) {
    int netThrottle = applyDeadzone(driveForward - driveBackward, THROTTLE_DEADZONE);
    int throttlePulse = PWM_NEUTRAL_US + (netThrottle * THROTTLE_HALF_RANGE_US) / TRIGGER_MAX;
    esc.writeMicroseconds(clampPulse(throttlePulse));

    int filteredSteerAxis = applyDeadzone(steerAxis, STEER_DEADZONE);
    int steerPulse = PWM_NEUTRAL_US - (filteredSteerAxis * STEER_HALF_RANGE_US) / STEER_AXIS_MAX;
    steeringServo.writeMicroseconds(clampPulse(steerPulse));
}

void drive_stop() {
    esc.writeMicroseconds(PWM_NEUTRAL_US);
    steeringServo.writeMicroseconds(PWM_NEUTRAL_US);
}