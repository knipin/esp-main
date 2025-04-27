#include "endpoints.h"
#include <app-common/zap-generated/ids/Clusters.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/DataModel.h>
#include <array>

EndpointId gThermostatEndpointId = 0;
EndpointId gTempSensorEndpointId = 0;

// Thermostat Endpoint
EndpointId AddThermostatEndpoint() {
    constexpr uint16_t clusterList[] = {
        Clusters::Thermostat::Id,
        Clusters::Identify::Id
    };

    constexpr chip::EndpointId endpoint = 1;
    constexpr EmberAfEndpointType endpointType = {
        .cluster = clusterList,
        .clusterCount = std::size(clusterList),
        .endpointSize = 0
    };

    // Регистрация endpoint
    emberAfEndpointEnableDisable(endpoint, true);

    // Установка атрибутов
    Clusters::Thermostat::Attributes::LocalTemperature::Set(endpoint, 2500);
    Clusters::Thermostat::Attributes::OccupiedHeatingSetpoint::Set(endpoint, 2500);
    Clusters::Thermostat::Attributes::SystemMode::Set(endpoint, Clusters::Thermostat::SystemModeEnum::kOff);

    return endpoint;
}

// Temperature Sensor Endpoint
EndpointId AddTempSensorEndpoint() {
    constexpr uint16_t clusterList[] = {
        Clusters::TemperatureMeasurement::Id,
        Clusters::Identify::Id
    };

    constexpr chip::EndpointId endpoint = 2;
    constexpr EmberAfEndpointType endpointType = {
        .cluster = clusterList,
        .clusterCount = std::size(clusterList),
        .endpointSize = 0
    };

    // Регистрация endpoint
    emberAfEndpointEnableDisable(endpoint, true);

    // Установка атрибутов
    Clusters::TemperatureMeasurement::Attributes::MeasuredValue::Set(endpoint, 2500);
    Clusters::TemperatureMeasurement::Attributes::MinMeasuredValue::Set(endpoint, -4000);
    Clusters::TemperatureMeasurement::Attributes::MaxMeasuredValue::Set(endpoint, 8000);

    return endpoint;
}