#include "endpoints.h"
#include "pid_controller.h"
#include "temperature_sensor.h"
#include "pwm_controller.h"
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/server/Server.h>
#include <credentials/DeviceAttestationCredsProvider.h>
// #include <matter_config.h>
#include <platform/PlatformManager.h>

using namespace chip;
using namespace chip::app;
using namespace chip::Credentials;
using namespace chip::DeviceLayer;

static const char *TAG = "SmartFloorHeating";

// Глобальные переменные
static EndpointId thermostatEndpoint = 1;
static EndpointId tempSensorEndpoint = 2;
static float currentTemp = 0.0f;
static float targetTemp = 25.0f;
static bool heatingEnabled = false;
static PidController pidController(2.0f, 0.5f, 1.0f, 1000);
static TemperatureSensor tempSensor(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
static PwmController pwmController(LEDC_CHANNEL_0, LEDC_TIMER_0, 
                                 LEDC_LOW_SPEED_MODE, GPIO_NUM_5, 1000);

class MatterCallbacks : public AppDelegate {
public:
    void OnCommissioningSessionEstablishmentStarted() override {
        ESP_LOGI(TAG, "Commissioning started");
    }
    
    void OnCommissioningSessionStopped() override {
        ESP_LOGI(TAG, "Commissioning stopped");
    }
    
    void OnCommissioningSessionEstablishmentError(CHIP_ERROR err) override {
        ESP_LOGE(TAG, "Commissioning error: %" CHIP_ERROR_FORMAT, err.Format());
    }
    
    void OnCommissioningSessionEstablishmentFinished() override {
        ESP_LOGI(TAG, "Commissioning completed");
    }
};

void UpdateClusterState() {
    Protocols::InteractionModel::Status status;
    
    // Обновление температуры в термостате
    status = Clusters::Thermostat::Attributes::LocalTemperature::Set(
        thermostatEndpoint, (int16_t)(currentTemp * 100));
    
    // Обновление температуры в датчике
    status = Clusters::TemperatureMeasurement::Attributes::MeasuredValue::Set(
        tempSensorEndpoint, (int16_t)(currentTemp * 100));
}

void MatterPostAttributeChangeCallback(const app::ConcreteAttributePath &path,
                                    AttributeId attributeId, uint8_t type,
                                    uint16_t size, uint8_t *value) {
    if (path.mEndpointId == thermostatEndpoint) {
        if (attributeId == Clusters::Thermostat::Attributes::OccupiedHeatingSetpoint::Id) {
            int16_t temp = *reinterpret_cast<int16_t*>(value);
            targetTemp = temp / 100.0f;
            ESP_LOGI(TAG, "New target temp: %.1f°C", targetTemp);
        }
        else if (attributeId == Clusters::Thermostat::Attributes::SystemMode::Id) {
            uint8_t mode = *value;
            heatingEnabled = (mode == Clusters::Thermostat::SystemModeEnum::kHeat);
            ESP_LOGI(TAG, "Heating %s", heatingEnabled ? "enabled" : "disabled");
        }
    }
}

void InitMatter() {
    // Инициализация NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Инициализация Matter
    err = PlatformMgr().InitChipStack();
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Chip stack init failed: %" CHIP_ERROR_FORMAT, err.Format());
        return;
    }

    // Инициализация сервера
    static chip::CommonCaseDeviceServerInitParams initParams;
    initParams.InitializeStaticResourcesBeforeServerInit();
    Server::GetInstance().Init(initParams);

    // Создание endpoint'ов
    thermostatEndpoint = AddThermostatEndpoint();
    tempSensorEndpoint = AddTempSensorEndpoint();

    // Настройка callback'ов
    static MatterCallbacks callbacks;
    Server::GetInstance().GetCommissioningWindowManager().SetAppDelegate(&callbacks);
    SetDeviceAttestationCredentialsProvider(Examples::GetExampleDACProvider());
    
    // Установка callback'а для атрибутов
    Server::GetInstance().GetInteractionModelDelegate()->SetAttributeWriteCallback(
        MatterPostAttributeChangeCallback);
    
    // Запуск Matter
    err = PlatformMgr().StartEventLoopTask();
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to start event loop: %" CHIP_ERROR_FORMAT, err.Format());
        return;
    }

    ESP_LOGI(TAG, "Matter initialized");
}

void HeatingControlTask(void *pvParameters) {
    while (true) {
        // Чтение температуры
        currentTemp = tempSensor.ReadTemperature();
        ESP_LOGI(TAG, "Current temp: %.1f°C, Target: %.1f°C", 
                currentTemp, targetTemp);

        // Управление нагревом
        if (heatingEnabled) {
            float output = pidController.Compute(targetTemp, currentTemp);
            pwmController.SetDutyCycle(output);
            ESP_LOGI(TAG, "PID output: %.1f%%", output);
        } else {
            pwmController.SetDutyCycle(0);
        }

        // Обновление атрибутов Matter
        UpdateClusterState();

        vTaskDelay(pdMS_TO_TICKS(pidController.GetSampleTime()));
    }
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "Starting Smart Floor Heating system");

    // Инициализация периферии
    tempSensor.Init();
    pwmController.Init();

    // Инициализация Matter
    InitMatter();

    // Запуск задачи управления
    xTaskCreate(HeatingControlTask, "HeatingCtrl", 4096, NULL, 5, NULL);
}