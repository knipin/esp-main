#include "temperature_sensor.h"
#include "esp_log.h"

TemperatureSensor::TemperatureSensor(adc1_channel_t channel, adc_atten_t atten)
    : channel(channel), atten(atten) {}

void TemperatureSensor::Init() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, atten);
}

float TemperatureSensor::ReadTemperature() {
    int raw = adc1_get_raw(channel);
    float voltage = (raw * VREF) / 4095.0f;
    return (voltage - OFFSET) / SCALE;
}