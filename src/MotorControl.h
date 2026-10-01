#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Arduino.h>

bool motorInit();      
void setDriveSpeed(float speed);
void setExcavationSpeed(float speed);

void motorUpdate();

void stopDrive();
void stopExcavation();
void stopAllMotors();

#endif

