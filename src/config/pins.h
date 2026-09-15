#pragma once

// Drive pins
constexpr int ESC_PIN = 26;       // Traxxas XL-5 ESC signal (PWM)
constexpr int STEERING_PIN = 27;  // Traxxas 2056 steering servo signal (PWM)

// Base pins
constexpr int ARM_BASE_ESC_PIN = 13;  // Titan 12T base motor ESC (PWM) -- rotates the cannon

// Cannon fire/reload mechanism pins (4x MG90S)
constexpr int FIRING_PIN_SERVO_PIN = 14;      // Servo 1 -- firing pin, DOWN (locked) / UP (fired)
constexpr int FIRING_SUPPORT_SERVO_PIN = 16;  // Servo 2 -- firing support, always in lockstep with Servo 1
constexpr int SPOOL_SERVO_PIN = 17;           // Servo 3 -- spool control, SET / RELEASED
constexpr int MOTOR_SWITCH_SERVO_PIN = 25;    // Servo 4 -- flips the LEGO motor's direction switch, OFF / ON