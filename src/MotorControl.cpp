#include "MotorControl.h"
#include "GigaPWM.h"

static const uint8_t DRIVE_PWM_PIN = 9;
static const uint8_t DRIVE_DIR_PIN = 22;

static const uint8_t EXC_PWM_PIN = 10;
static const uint8_t EXC_DIR_PIN = 23;
static const uint32_t PWM_FREQ_HZ = 5000;
static const uint8_t  PWM_BITS    = 12;
static const float RAMP_PCT_PER_S = 400.0f;

struct Motor
{
    GigaPWM pwm;
    uint8_t dirPin;
    float   target;
    float   current;
};

static Motor driveMotor;
static Motor excMotor;

static void writeMotor(Motor& m)
{

    digitalWrite(m.dirPin, m.current >= 0.0f ? HIGH : LOW);
    m.pwm.setPercent(fabsf(m.current));
}

static void rampMotor(Motor& m, float dtSec)
{
    float maxStep = RAMP_PCT_PER_S * dtSec;
    float diff = m.target - m.current;

    if (diff > maxStep)       diff = maxStep;
    else if (diff < -maxStep) diff = -maxStep;

    m.current += diff;
    writeMotor(m);
}

static float sanitize(float speed)
{
    if (isnan(speed))
    {
        return 0.0f;
    }
    return constrain(speed, -100.0f, 100.0f);
}

bool motorInit()
{
    driveMotor.dirPin = DRIVE_DIR_PIN;
    excMotor.dirPin   = EXC_DIR_PIN;

    pinMode(DRIVE_DIR_PIN, OUTPUT);
    pinMode(EXC_DIR_PIN, OUTPUT);

    bool ok = true;
    ok &= driveMotor.pwm.begin(DRIVE_PWM_PIN, PWM_FREQ_HZ, PWM_BITS);
    ok &= excMotor.pwm.begin(EXC_PWM_PIN, PWM_FREQ_HZ, PWM_BITS);

    stopAllMotors();
    return ok;
}

void setDriveSpeed(float speed)      { driveMotor.target = sanitize(speed); }
void setExcavationSpeed(float speed) { excMotor.target   = sanitize(speed); }

void motorUpdate()
{
    static uint32_t lastUs = micros();
    uint32_t now = micros();
    float dt = (now - lastUs) * 1e-6f;
    lastUs = now;

    if (dt > 0.1f) dt = 0.1f;   

    rampMotor(driveMotor, dt);
    rampMotor(excMotor, dt);
}

void stopDrive()
{
    driveMotor.target = driveMotor.current = 0.0f;
    writeMotor(driveMotor);
}

void stopExcavation()
{
    excMotor.target = excMotor.current = 0.0f;
    writeMotor(excMotor);
}

void stopAllMotors()
{
    stopDrive();
    stopExcavation();
}

