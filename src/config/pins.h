#pragma once

// Drive pins
constexpr int ESC_PIN = 26;       // Traxxas XL-5 ESC signal (PWM)
constexpr int STEERING_PIN = 27;  // Traxxas 2056 steering servo signal (PWM)

// Base pins
constexpr int ARM_BASE_ESC_PIN = 13;  // Titan 12T base motor ESC (PWM) -- rotates the cannon

// Cannon pins
constexpr int FEEDER_SERVO_PIN = 14;        // MG90S ball feeder servo
constexpr int AIM_SERVO_PIN = 16;           // MG90S barrel elevation servo
constexpr int CANNON_POWER_SERVO_PIN = 17;  // spare MG90S, flips the manual ON/OFF toggle switch
