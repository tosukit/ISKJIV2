#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Arduino.h>

bool motorInit();      // returns false if a PWM pin could not be started

// Set the TARGET speed, -100 .. +100 (%). The actual output ramps toward it.
void setDriveSpeed(float speed);
void setExcavationSpeed(float speed);

// Call every loop(): applies the ramp and updates PWM/DIR pins.
void motorUpdate();

// Immediate stop (no ramp)
void stopDrive();
void stopExcavation();
void stopAllMotors();

#endif

