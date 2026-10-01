#ifndef ROSINTERFACE_H
#define ROSINTERFACE_H

#include <Arduino.h>



void rosInit();
void rosUpdate();               

float getDriveCommand();        
float getExcavationCommand();   
bool  rosCommandsActive();      

#endif

