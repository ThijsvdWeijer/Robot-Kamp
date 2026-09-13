#include "arm_base.h"

#include <ESP32Servo.h>

#include "config/pins.h"

namespace {

// Standard RC PWM range for this ESC, in microseconds.
constexpr int ESC_NEUTRAL_US = 1500;

// Fixed spin speeds. Modest values, not full-scale (1000/2000us), until the
// arm is physically wired and these can be tuned against real load.
constexpr int ESC_SPIN_LEFT_US = 1380;
constexpr int ESC_SPIN_RIGHT_US = 1620;

// How long to hold neutral at startup before the ESC is considered armed.
constexpr int ESC_ARM_DELAY_MS = 1000;

Servo baseEsc;

}

void arm_base_init() {
    baseEsc.attach(ARM_BASE_ESC_PIN);

    baseEsc.writeMicroseconds(ESC_NEUTRAL_US);
    delay(ESC_ARM_DELAY_MS);
}

void arm_base_update(bool spinLeft, bool spinRight) {
    if (spinLeft == spinRight) {

        baseEsc.writeMicroseconds(ESC_NEUTRAL_US);
    } else if (spinLeft) {
        baseEsc.writeMicroseconds(ESC_SPIN_LEFT_US);
    } else {
        baseEsc.writeMicroseconds(ESC_SPIN_RIGHT_US);
    }
}

void arm_base_stop() {
    baseEsc.writeMicroseconds(ESC_NEUTRAL_US);
}
