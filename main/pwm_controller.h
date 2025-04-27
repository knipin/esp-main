#pragma once
#include "driver/ledc.h"
#include "driver/gpio.h"

class PwmController {
public:
    PwmController(ledc_channel_t channel, ledc_timer_t timer, 
                 ledc_mode_t mode, gpio_num_t gpio, uint32_t freqHz);
    void Init();
    void SetDutyCycle(float percentage);

private:
    ledc_channel_t channel;
    ledc_timer_t timer;
    ledc_mode_t mode;
    gpio_num_t gpio;
    uint32_t freqHz;
};