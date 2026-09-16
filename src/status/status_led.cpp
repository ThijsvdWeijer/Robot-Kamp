#include "status_led.h"

#include <Arduino.h>

#include "config/pins.h"

namespace {

// Flip to true if the LED is wired common-anode (colors look inverted, or
// the LED is lit when it should be off).
constexpr bool COMMON_ANODE = false;

void writeChannel(int pin, uint8_t value) {
    analogWrite(pin, COMMON_ANODE ? 255 - value : value);
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
    writeChannel(STATUS_LED_RED_PIN, r);
    writeChannel(STATUS_LED_GREEN_PIN, g);
    writeChannel(STATUS_LED_BLUE_PIN, b);
}

}  // namespace

void status_led_init() {
    pinMode(STATUS_LED_RED_PIN, OUTPUT);
    pinMode(STATUS_LED_GREEN_PIN, OUTPUT);
    pinMode(STATUS_LED_BLUE_PIN, OUTPUT);

    status_led_update(CannonState::Loaded);
}

void status_led_update(CannonState state) {
    switch (state) {
        case CannonState::Loaded:
            setColor(0, 255, 0);  // green -- ready to fire
            break;

        case CannonState::Reloading:
            setColor(255, 140, 0);  // orange -- reloading
            break;

        case CannonState::Fired:
            setColor(255, 0, 0);  // red -- fired
            break;
    }
}
