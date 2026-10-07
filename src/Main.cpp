#include <Arduino.h>
#include <mbed.h>
#include "MotorControl.h"
#include "ROSInterface.h"

static const uint32_t WATCHDOG_MS = 1000;

void setup()
{
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

#ifdef DEVICE_WATCHDOG
    
    mbed::Watchdog::get_instance().start(WATCHDOG_MS);
#endif
}

void loop()
{
#ifdef DEVICE_WATCHDOG
    mbed::Watchdog::get_instance().kick();
#endif

    rosUpdate();

    if (rosCommandsActive())
    {
        setDriveSpeed(getDriveCommand());
        setExcavationSpeed(getExcavationCommand());
        motorUpdate();
    }
    else
    {
        
        stopAllMotors();
        motorUpdate();   
    }

    delay(5);
}

