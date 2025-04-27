#pragma once
#include <cmath>

class PidController {
public:
    PidController(float kp, float ki, float kd, uint32_t sampleTimeMs);
    
    void Reset();
    float Compute(float setpoint, float input);
    void SetTunings(float kp, float ki, float kd);
    uint32_t GetSampleTime() const { return sampleTimeMs; }

private:
    float kp;
    float ki;
    float kd;
    float integral;
    float prevError;
    uint32_t sampleTimeMs;
    uint32_t lastTime;
};