#pragma once

#include <ESP32Servo.h>
#include <stdint.h>

#include "cannon_fire.h"

// Shared internals for the cannon_fire module, split across:
//   cannon_fire.cpp        - init, update dispatch, state getters,
//                             calibration values, servo/state definitions
//   cannon_fire_fire.cpp   - fire()
//   cannon_fire_reload.cpp - startReload(), updateReload()
// Not part of the public API in cannon_fire.h -- do not include this
// outside the cannon_fire module.

// Calibration values, defined and tuned in cannon_fire.cpp.
extern int SERVO1_DOWN;
extern int SERVO1_UP;
extern int SERVO2_DOWN;
extern int SERVO2_UP;
extern int SERVO3_SET;
extern int SERVO3_RELEASED;
extern int SERVO4_OFF;
extern int SERVO4_ON;
extern int RELOAD_MOTOR_TIME_MS;
extern int LOCK_SETTLE_TIME_MS;
extern int SPOOL_SETTLE_TIME_MS;
extern int MOTOR_OFF_SETTLE_TIME_MS;

// Servo objects, defined and attached in cannon_fire.cpp.
extern Servo servo1;  // firing pin
extern Servo servo2;  // firing support
extern Servo servo3;  // spool control
extern Servo servo4;  // motor switch

// State, defined in cannon_fire.cpp.
extern CannonState state;
extern ReloadStep reloadStep;

// Reused across reload sub-steps: start-of-step timestamp for whichever
// step is currently timed (MotorRunning against RELOAD_MOTOR_TIME_MS,
// Locking against LOCK_SETTLE_TIME_MS).
extern uint32_t stepStartTime;

// Shared write helpers, defined in cannon_fire.cpp.
void setFiringServos(int angle1, int angle2);
void setSpoolServo(int angle);
void setMotorSwitchServo(int angle);

// Defined in cannon_fire_fire.cpp.
void fire();

// Defined in cannon_fire_reload.cpp.
void startReload();
void updateReload();
