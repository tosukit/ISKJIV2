#ifndef GIGAPWM_H
#define GIGAPWM_H

#include <Arduino.h>

namespace mbed { class PwmOut; }

class GigaPWM
{
public:
    GigaPWM();
    ~GigaPWM();

    GigaPWM(const GigaPWM&) = delete;
    GigaPWM& operator=(const GigaPWM&) = delete;

    bool begin(uint8_t pin, uint32_t frequency, uint8_t resolutionBits = 12);
    void end();

    bool setDuty(uint32_t duty);      
    bool setPercent(float percent);   
    uint32_t getTop() const { return _top; }
    bool isRunning() const { return _pwm != nullptr; }

private:
    mbed::PwmOut* _pwm;
    uint8_t  _pin;
    uint8_t  _resolution;
    uint32_t _frequency;
    uint32_t _top;
};

#endif

