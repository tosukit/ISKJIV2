#include "GigaPWM.h"
#include <mbed.h>
#include "pinDefinitions.h"

static const uint32_t MAX_FREQUENCY_HZ = 1000000UL;  

GigaPWM::GigaPWM()
    : _pwm(nullptr), _pin(0), _resolution(0), _frequency(0), _periodUs(0), _top(0)
{
}

GigaPWM::~GigaPWM()
{
    end();
}

bool GigaPWM::begin(uint8_t pin, uint32_t frequency, uint8_t resolutionBits)
{
    if (frequency == 0 || frequency > MAX_FREQUENCY_HZ ||
        resolutionBits < 1 || resolutionBits > 16)
    {
        return false;
    }

    end();

    PinName name = digitalPinToPinName(pin);
    if (name == NC)
    {
        return false;
    }

    
    uint32_t periodUs = (1000000UL + frequency / 2UL) / frequency;
    if (periodUs < 1)
    {
        periodUs = 1;
    }

    _pwm = new mbed::PwmOut(name);
    _pwm->period_us((int)periodUs);
    _pwm->write(0.0f);

    _pin = pin;
    _periodUs = periodUs;
    _frequency = 1000000UL / periodUs;    
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

        
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
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
    if (!(percent >= 0.0f))   
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

