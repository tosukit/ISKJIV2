#ifndef ROSINTERFACE_H
#define ROSINTERFACE_H

#include <Arduino.h>

// USB-serial link to the Raspberry Pi (which runs the ROS 2 bridge node).
// Protocol: one ASCII line per command, "<drive>,<excavation>\n", each -100..100.

void rosInit();
void rosUpdate();               // call every loop(); non-blocking

float getDriveCommand();        // 0 if commands are stale
float getExcavationCommand();   // 0 if commands are stale
bool  rosCommandsActive();      // true while valid commands keep arriving

#endif

