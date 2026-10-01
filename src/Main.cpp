#include <Arduino.h>

#include "MotorControl.h"
#include "ROSInterface.h"

void setup()
{
    // Motors first, so outputs are in a known (stopped) state immediately.
    bool pwmOk = motorInit();

    rosInit();

    if (pwmOk)
    {
        Serial.println("ISKJI motor controller started");
    }
    else
    {
        Serial.println("ERROR: PWM init failed - check pin numbers");
    }
}

void loop()
{
    rosUpdate();

    setDriveSpeed(getDriveCommand());
    setExcavationSpeed(getExcavationCommand());

    motorUpdate();

    delay(5);
}

