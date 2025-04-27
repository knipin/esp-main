#include "pid_controller.h"
#include "esp_timer.h"

PidController::PidController(float kp, float ki, float kd, uint32_t sampleTimeMs)
    : kp(kp), ki(ki), kd(kd), sampleTimeMs(sampleTimeMs) {
    Reset();
}

void PidController::Reset() {
    integral = 0;
    prevError = 0;
    lastTime = esp_timer_get_time() / 1000;
}

float PidController::Compute(float setpoint, float input) {
    uint32_t now = esp_timer_get_time() / 1000;
    uint32_t dt = now - lastTime;
    
    if (dt < sampleTimeMs) {
        return prevError;
    }
    
    float error = setpoint - input;
    integral += error * dt;
    float derivative = (error - prevError) / dt;
    
    float output = kp * error + ki * integral + kd * derivative;
    
    // Anti-windup
    integral = fmaxf(-100.0f, fminf(integral, 100.0f));
    
    prevError = error;
    lastTime = now;
    
    return fmaxf(0.0f, fminf(output, 100.0f));
}

void PidController::SetTunings(float kp, float ki, float kd) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
}