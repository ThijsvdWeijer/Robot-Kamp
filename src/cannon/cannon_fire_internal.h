#pragma once

#include <ESP32Servo.h>
#include <stdint.h>

#include "cannon_fire.h"

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

extern Servo servo1;  // firing pin
extern Servo servo2;  // firing support
extern Servo servo3;  // spool control
extern Servo servo4;  // motor switch

extern CannonState state;
extern ReloadStep reloadStep;


extern uint32_t stepStartTime;

void setFiringServos(int angle1, int angle2);
void setSpoolServo(int angle);
void setMotorSwitchServo(int angle);

void fire();

void startReload();
void updateReload();
