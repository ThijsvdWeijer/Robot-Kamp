#include <Arduino.h>
#include <ESP32Servo.h>

// -----------------------------------------------------------------------------
// Hardware verification test for a single MG90S servo. Flash this per
// servo/pin and watch it repeat the move sequence continuously, before
// trusting that servo/pin in the real arm_shoulder/arm_elbow/arm_gripper
// modules. This is standalone bench-test code — no dependency on
// controller_input, drive, arm_*, stepped_servo, or failsafe — and isn't
// meant to stay wired into the main build permanently.
// -----------------------------------------------------------------------------

// Change this before each run to test a different pin/servo.
#define SERVO_PIN 13

namespace {

// Same 1000-2000us / 0-180deg convention used by the other servos in this
// project.
constexpr int PWM_MIN_US = 1000;
constexpr int PWM_MAX_US = 2000;

constexpr int STEP_DELAY_MS = 15;     // delay between each degree during the sweep
constexpr int HOLD_DELAY_MS = 1000;   // pause at each fixed test position
constexpr int SWEEP_LOG_INTERVAL_DEG = 10;

Servo testServo;

void moveTo(int angle, const char* label) {
    testServo.write(angle);
    Serial.println(label);
}

}  // namespace

void runSequence() {
    moveTo(0, "0 degrees");
    delay(HOLD_DELAY_MS);

    moveTo(90, "90 degrees");
    delay(HOLD_DELAY_MS);

    moveTo(180, "180 degrees");
    delay(HOLD_DELAY_MS);

    Serial.println("Sweeping 0 -> 180...");
    for (int angle = 0; angle <= 180; angle++) {
        testServo.write(angle);
        if (angle % SWEEP_LOG_INTERVAL_DEG == 0) {
            Serial.printf("angle=%d\n", angle);
        }
        delay(STEP_DELAY_MS);
    }

    Serial.println("Sweeping 180 -> 0...");
    for (int angle = 180; angle >= 0; angle--) {
        testServo.write(angle);
        if (angle % SWEEP_LOG_INTERVAL_DEG == 0) {
            Serial.printf("angle=%d\n", angle);
        }
        delay(STEP_DELAY_MS);
    }

    moveTo(90, "90 degrees (centered)");
    delay(HOLD_DELAY_MS);
}

void setup() {
    Serial.begin(115200);
    delay(200);  // give the serial monitor a moment to connect

    Serial.printf("Servo bench test starting on GPIO %d\n", SERVO_PIN);
    Serial.println("Sequence: 0 -> 90 -> 180 (1s hold each), then a slow sweep 0->180->0, repeating forever.");

    testServo.setPeriodHertz(50);
    testServo.attach(SERVO_PIN, PWM_MIN_US, PWM_MAX_US);
}

void loop() {
    runSequence();
}
