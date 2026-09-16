#include <Arduino.h>
#include <ESP32Servo.h>
#include <Bluepad32.h>

#include "config/pins.h"

// -----------------------------------------------------------------------------
// Per-servo button jog test. Drive each of the 4 cannon servos independently
// with one controller face button per servo:
//   Cross / X -> Servo 1 (firing pin)
//   Circle    -> Servo 2 (firing support)
//   Square    -> Servo 3 (spool control)
//   Triangle  -> Servo 4 (motor switch)
// Each press toggles that servo between position A and position B (A on the
// first press, B on the next, A again after that, ...). Standalone bench
// tool -- no dependency on controller_input, drive, arm_*, stepped_servo,
// cannon_fire, or failsafe.
// -----------------------------------------------------------------------------

namespace {

struct JogServo {
    const char* name;
    int pin;
    int angleA;
    int angleB;
    Servo servo;
    bool atB;               // left unset in the initializer list below -> value-initialized to false
    bool buttonWasPressed;  // same
};

// angleA/angleB reuse the real calibration values from cannon_fire.cpp
// (DOWN/UP, SET/RELEASED, OFF/ON) so this test exercises each servo's actual
// working range. Adjust here if you just want a generic sweep instead.
JogServo servos[4] = {
    {"Servo 1 (firing pin)",     FIRING_PIN_SERVO_PIN,     140, 180},
    {"Servo 2 (firing support)", FIRING_SUPPORT_SERVO_PIN, 70, 180},
    {"Servo 3 (spool control)",  SPOOL_SERVO_PIN,          130, 0},
    {"Servo 4 (motor switch)",   MOTOR_SWITCH_SERVO_PIN,   165, 180},
};

ControllerPtr connectedController = nullptr;

void onConnectedController(ControllerPtr ctl) {
    if (connectedController == nullptr) {
        connectedController = ctl;
        Serial.println("Controller connected.");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    if (connectedController == ctl) {
        connectedController = nullptr;
        Serial.println("Controller disconnected.");
    }
}

void toggleServo(JogServo& s) {
    s.atB = !s.atB;
    int angle = s.atB ? s.angleB : s.angleA;
    s.servo.write(angle);
    Serial.printf("%s -> %d degrees (position %s)\n", s.name, angle, s.atB ? "B" : "A");
}

// Edge-triggers on press only, so holding the button doesn't retoggle.
void handleButton(bool pressed, JogServo& s) {
    if (pressed && !s.buttonWasPressed) {
        toggleServo(s);
    }
    s.buttonWasPressed = pressed;
}

}  // namespace

void setup() {
    Serial.begin(115200);
    delay(200);  // give the serial monitor a moment to connect

    Serial.println("Servo jog test starting.");
    Serial.println("Buttons: Cross/X=Servo1  Circle=Servo2  Square=Servo3  Triangle=Servo4");
    Serial.println("Each press toggles that servo between position A and position B.");

    for (JogServo& s : servos) {
        // No explicit pulse-width range: matches cannon_fire.cpp's plain
        // attach(pin) so the angle-to-pulse mapping -- and therefore the
        // DOWN/UP, SET/RELEASED, OFF/ON calibration angles -- is identical
        // to the real firmware.
        s.servo.attach(s.pin);
        s.servo.write(s.angleA);
    }

    BP32.setup(&onConnectedController, &onDisconnectedController);
}

void loop() {
    BP32.update();

    if (connectedController != nullptr && connectedController->isConnected()) {
        ControllerPtr ctl = connectedController;

        // Bluepad32 names face buttons by pad position (a/b/x/y), not the
        // PlayStation glyph printed on them. On a DualSense: a=Cross,
        // b=Circle, x=Square, y=Triangle.
        handleButton(ctl->a(), servos[0]);  // Cross / X
        handleButton(ctl->b(), servos[1]);  // Circle
        handleButton(ctl->x(), servos[2]);  // Square
        handleButton(ctl->y(), servos[3]);  // Triangle
    }

    delay(10);
}
