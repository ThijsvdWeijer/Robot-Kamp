#include "drive.h"

#include <ESP32Servo.h>

#include "config/pins.h"

namespace {

// Standard RC PWM range for both the ESC and steering servo, in microseconds.
constexpr int PWM_NEUTRAL_US = 1500;
constexpr int PWM_MIN_US = 1000;
constexpr int PWM_MAX_US = 2000;
constexpr int PWM_HALF_RANGE_US = 500;

// Raw trigger depth range Bluepad32 reports for L2/R2.
constexpr int TRIGGER_MAX = 1023;

// Raw left-stick X magnitude Bluepad32 reports (range is roughly -511..512;
// 512 is used as the scaling denominator for both directions).
constexpr int STEER_AXIS_MAX = 512;

// How long to hold neutral at startup before the ESC is considered armed.
constexpr int ESC_ARM_DELAY_MS = 1000;

Servo esc;
Servo steeringServo;

int clampPulse(int pulseUs) {
    if (pulseUs < PWM_MIN_US) return PWM_MIN_US;
    if (pulseUs > PWM_MAX_US) return PWM_MAX_US;
    return pulseUs;
}

}  // namespace

void drive_init() {
    esc.attach(ESC_PIN);
    steeringServo.attach(STEERING_PIN);

    // The XL-5 needs a steady neutral signal held briefly before it will
    // arm. Steering has no such requirement, but centering it here too
    // avoids leaving it at an undefined initial position.
    esc.writeMicroseconds(PWM_NEUTRAL_US);
    steeringServo.writeMicroseconds(PWM_NEUTRAL_US);
    delay(ESC_ARM_DELAY_MS);
}

void drive_update(int driveForward, int driveBackward, int steerAxis) {
    int netThrottle = driveForward - driveBackward;  // -1023..1023
    int throttlePulse = PWM_NEUTRAL_US + (netThrottle * PWM_HALF_RANGE_US) / TRIGGER_MAX;
    esc.writeMicroseconds(clampPulse(throttlePulse));

    // Inverted: raw steerAxis increases rightward on the stick but the
    // physical steering servo turns the opposite way for that pulse
    // direction, so the sign is flipped here to match.
    int steerPulse = PWM_NEUTRAL_US - (steerAxis * PWM_HALF_RANGE_US) / STEER_AXIS_MAX;
    steeringServo.writeMicroseconds(clampPulse(steerPulse));
}

void drive_stop() {
    esc.writeMicroseconds(PWM_NEUTRAL_US);
    steeringServo.writeMicroseconds(PWM_NEUTRAL_US);
}
