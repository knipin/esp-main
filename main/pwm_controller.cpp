#include "pwm_controller.h"

PwmController::PwmController(ledc_channel_t channel, ledc_timer_t timer,
                           ledc_mode_t mode, gpio_num_t gpio, uint32_t freqHz)
    : channel(channel), timer(timer), mode(mode), gpio(gpio), freqHz(freqHz) {}

void PwmController::Init() {
    // Настройка таймера
    ledc_timer_config_t timerConf = {
        .speed_mode = mode,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = timer,
        .freq_hz = freqHz,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timerConf);
    
    // Настройка канала
    ledc_channel_config_t chConf = {
        .gpio_num = gpio,
        .speed_mode = mode,
        .channel = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = timer,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&chConf);
}

void PwmController::SetDutyCycle(float percentage) {
    uint32_t duty = (percentage / 100.0f) * 8191; // 13 бит (0-8191)
    ledc_set_duty(mode, channel, duty);
    ledc_update_duty(mode, channel);
}