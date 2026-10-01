#include "GigaPWM.h"

#include <mbed.h>
#include "pinDefinitions.h"

GigaPWM::GigaPWM()
    : _pwm(nullptr), _pin(0), _resolution(0), _frequency(0), _top(0)
{
}

GigaPWM::~GigaPWM()
{
    end();
}

bool GigaPWM::begin(uint8_t pin, uint32_t frequency, uint8_t resolutionBits)
{
    if (frequency == 0 || resolutionBits < 1 || resolutionBits > 16)
    {
        return false;
    }

    end();

    PinName name = digitalPinToPinName(pin);
    if (name == NC)
    {
        return false;
    }

    _pwm = new mbed::PwmOut(name);
    _pwm->period_us(1000000UL / frequency);
    _pwm->write(0.0f);

    _pin = pin;
    _frequency = frequency;
    _resolution = resolutionBits;
    _top = (1UL << resolutionBits) - 1UL;
    return true;
}

void GigaPWM::end()
{
    if (_pwm != nullptr)
    {
        _pwm->write(0.0f);
        delete _pwm;
        _pwm = nullptr;
    }
}

bool GigaPWM::setDuty(uint32_t duty)
{
    if (_pwm == nullptr)
    {
        return false;
    }
    if (duty > _top)
    {
        duty = _top;
    }
    _pwm->write((float)duty / (float)_top);
    return true;
}

bool GigaPWM::setPercent(float percent)
{
    if (_pwm == nullptr)
    {
        return false;
    }
    if (!(percent >= 0.0f))   // also catches NaN
    {
        percent = 0.0f;
    }
    if (percent > 100.0f)
    {
        percent = 100.0f;
    }
    _pwm->write(percent / 100.0f);
    return true;
}
