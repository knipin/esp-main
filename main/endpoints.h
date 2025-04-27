#pragma once
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/util/attribute-storage.h>

using namespace chip;
using namespace chip::app;

// Объявление endpoint ID
extern EndpointId gThermostatEndpointId;
extern EndpointId gTempSensorEndpointId;

// Прототипы функций
EndpointId AddThermostatEndpoint();
EndpointId AddTempSensorEndpoint();