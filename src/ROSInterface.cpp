#include "ROSInterface.h"
#include <math.h>
#include <stdlib.h>

static const uint32_t CMD_TIMEOUT_MS = 500;

static float driveCommand = 0.0f;
static float excavationCommand = 0.0f;

static char   lineBuf[64];
static size_t lineLen = 0;
static bool   lineOverflow = false;

static uint32_t lastRxMs = 0;
static bool     active = false;

void rosInit()
{
    Serial.begin(115200);

    uint32_t t0 = millis();
    while (!Serial && (millis() - t0) < 2000)
    {
        delay(10);
    }
}

static bool parseLine(const char* s, float& a, float& b)
{
    char* end;

    a = strtof(s, &end);
    if (end == s || *end != ',')
    {
        return false;
    }

    const char* p = end + 1;
    b = strtof(p, &end);
    if (end == p)
    {
        return false;
    }

    while (*end == ' ' || *end == '\r')
    {
        end++;
    }
    if (*end != '\0')
    {
        return false;
    }

    return isfinite(a) && isfinite(b);
}

void rosUpdate()
{
   
    while (Serial.available() > 0)
    {
        int c = Serial.read();

        if (c == '\n')
        {
            lineBuf[lineLen] = '\0';

            float d, e;
            if (!lineOverflow && lineLen > 0 && parseLine(lineBuf, d, e))
            {
                driveCommand      = constrain(d, -100.0f, 100.0f);
                excavationCommand = constrain(e, -100.0f, 100.0f);
                lastRxMs = millis();
                active = true;
            }

            lineLen = 0;
            lineOverflow = false;
        }
        else if (lineLen < sizeof(lineBuf) - 1)
        {
            lineBuf[lineLen++] = (char)c;
        }
        else
        {
            lineOverflow = true;
        }
    }


    if (active && (millis() - lastRxMs) > CMD_TIMEOUT_MS)
    {
        driveCommand = 0.0f;
        excavationCommand = 0.0f;
        active = false;
    }
}

float getDriveCommand()      { return driveCommand; }
float getExcavationCommand() { return excavationCommand; }
bool  rosCommandsActive()    { return active; }


