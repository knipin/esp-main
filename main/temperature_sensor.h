#pragma once
#include "driver/adc.h"

class TemperatureSensor {
public:
    TemperatureSensor(adc1_channel_t channel, adc_atten_t atten);
    void Init();
    float ReadTemperature();

private:
    adc1_channel_t channel;
    adc_atten_t atten;
    
    static constexpr float VREF = 3300.0f; // mV
    static constexpr float OFFSET = 500.0f; // mV
    static constexpr float SCALE = 22.5f; // mV/°C
};