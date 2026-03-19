#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include "sensor/ve_direct_hex_sensor.h"
#include "number/ve_direct_hex_number.h"

namespace esphome {
namespace ve_direct_hex {

#define ESP_LOGI_UART(format, ...) do { \
    const char* tag = this->get_tag(); \
    ESP_LOGI(tag, format, ##__VA_ARGS__); \
} while (0)
#define ESP_LOGD_UART(format, ...) do { \
    const char* tag = this->get_tag(); \
    ESP_LOGD(tag, format, ##__VA_ARGS__); \
} while (0)
#define ESP_LOGV_UART(format, ...) do { \
    const char* tag = this->get_tag(); \
    ESP_LOGV(tag, format, ##__VA_ARGS__); \
} while (0)

static const char *const TAG = "ved_hex";

class VEDirectHexComponent : public uart::UARTDevice, public Component {
 public:

    explicit VEDirectHexComponent(uart::UARTComponent *uart, std::string uart_id);

    void setup() override;
    void loop() override;
    void dump_config() override;

    const char *get_tag() const { static std::string tag; tag = std::string(TAG) + "::" + uart_id_; return tag.c_str(); }

    void set_update_interval(uint32_t interval) { this->update_interval_ = interval; }
    void set_initial_delay(uint32_t initial_delay) { this->initial_delay_ = initial_delay; }

    // SmartSolar sensors
    void set_battery_voltage_sensor(VEDirectHexSensor *battery_voltage_sensor) { 
        battery_voltage_sensor_ = battery_voltage_sensor; 
        sensors.push_back(battery_voltage_sensor_);
    }
    void set_battery_current_sensor(VEDirectHexSensor *battery_current_sensor) { 
        battery_current_sensor_ = battery_current_sensor; 
        sensors.push_back(battery_current_sensor_);
    }
    void set_panel_voltage_sensor(VEDirectHexSensor *panel_voltage_sensor) {
        panel_voltage_sensor_ = panel_voltage_sensor;
        sensors.push_back(panel_voltage_sensor_);
    }
    void set_panel_current_sensor(VEDirectHexSensor *panel_current_sensor) {
        panel_current_sensor_ = panel_current_sensor;
        sensors.push_back(panel_current_sensor_);
    }
    void set_panel_power_sensor(VEDirectHexSensor *panel_power_sensor) {
        panel_power_sensor_ = panel_power_sensor;
        sensors.push_back(panel_power_sensor_);
    }
    void set_device_mode_sensor(VEDirectHexSensor *device_mode_sensor) {
        device_mode_sensor_ = device_mode_sensor;
        sensors.push_back(device_mode_sensor_);
    }
    void set_device_state_id_sensor(VEDirectHexSensor *device_state_id_sensor) {
        device_state_id_sensor_ = device_state_id_sensor;
        sensors.push_back(device_state_id_sensor_);
    }
    void set_device_state_text_sensor(text_sensor::TextSensor *device_state_sensor) {
        device_state_sensor_ = device_state_sensor;
        //sensors.push_back(device_state_sensor_);
    }
    void set_charge_current_limit_number(VEDirectHexNumber *charge_current_limit_number) {
        charge_current_limit_number_ = charge_current_limit_number;
        numbers_.push_back(charge_current_limit_number_);
    }
    void set_network_mode_number(VEDirectHexNumber *network_mode_number) {
        network_mode_number_ = network_mode_number;
        numbers_.push_back(network_mode_number_);
    }
    void set_network_mode_id_sensor(VEDirectHexSensor *network_mode_id_sensor) {
        network_mode_id_sensor_ = network_mode_id_sensor;
        sensors.push_back(network_mode_id_sensor_);
    }
    void set_network_mode_text_sensor(text_sensor::TextSensor *network_mode_sensor) {
        network_mode_sensor_ = network_mode_sensor;
        //sensors.push_back(device_state_sensor_);
    }

    // SmartShunt sensors    
    void set_main_battery_voltage_sensor(VEDirectHexSensor *main_battery_voltage_sensor) { 
        main_battery_voltage_sensor_ = main_battery_voltage_sensor; 
        sensors.push_back(main_battery_voltage_sensor_);
    }
    void set_aux_battery_voltage_sensor(VEDirectHexSensor *aux_battery_voltage_sensor) { 
        aux_battery_voltage_sensor_ = aux_battery_voltage_sensor; 
        sensors.push_back(aux_battery_voltage_sensor_);
    }
    void set_main_battery_current_sensor(VEDirectHexSensor *main_battery_current_sensor) { 
        main_battery_current_sensor_ = main_battery_current_sensor; 
        sensors.push_back(main_battery_current_sensor_);
    }
    void set_main_battery_power_sensor(VEDirectHexSensor *main_battery_power_sensor) { 
        main_battery_power_sensor_ = main_battery_power_sensor; 
        sensors.push_back(main_battery_power_sensor_);
    }
    void set_main_battery_cons_ah_sensor(VEDirectHexSensor *main_battery_cons_ah_sensor) { 
        main_battery_cons_ah_sensor_ = main_battery_cons_ah_sensor; 
        sensors.push_back(main_battery_cons_ah_sensor_);
    }
    void set_main_battery_soc_sensor(VEDirectHexSensor *main_battery_soc_sensor) { 
        main_battery_soc_sensor_ = main_battery_soc_sensor; 
        sensors.push_back(main_battery_soc_sensor_);
    }

    const std::string device_mode_str(int value);
    const std::string network_mode_str(int value);

    // These are public because of access from VEDirectHexSensor class
    VEDirectHexSensor *network_mode_id_sensor_;  

    text_sensor::TextSensor *device_state_sensor_;
    text_sensor::TextSensor *network_mode_sensor_;

    void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
    void publish_state_(VEDirectHexSensor *sensor, float value);

 protected:

    uart::UARTComponent *uart_;
    std::string uart_id_;

    uint32_t update_interval_;
    uint32_t initial_delay_;

    static const uint8_t VED_BUFFER_SIZE = 32;
    struct VedBuffer { uint8_t  size_{0}; uint8_t  data_[VED_BUFFER_SIZE]; };

    uint32_t last_update_{0};
    //uint32_t real_update_interval_{0};

    size_t current_sensor_index_{0};  // Index of the current sensor being processed - maybe to move to VEDirectHexSensor class
    size_t current_number_index_{0}; 
    uint8_t comm_stage_{0};
    bool update_in_progress_{false};
    VedBuffer rxBuffer_{};
    
    std::vector<VEDirectHexSensor *> sensors;  // Sensors container
    std::vector<VEDirectHexNumber *> numbers_; // Numbers container

    //  SmartSolar
    VEDirectHexSensor *battery_voltage_sensor_;  
    VEDirectHexSensor *battery_current_sensor_;  
    VEDirectHexSensor *panel_voltage_sensor_;  
    VEDirectHexSensor *panel_current_sensor_;  
    VEDirectHexSensor *panel_power_sensor_;  
    VEDirectHexSensor *device_mode_sensor_;  
    VEDirectHexSensor *device_state_id_sensor_;

    VEDirectHexNumber  *network_mode_number_;  
    VEDirectHexNumber  *charge_current_limit_number_;  

    // SmartShunt sensors       
    VEDirectHexSensor *main_battery_voltage_sensor_;  
    VEDirectHexSensor *aux_battery_voltage_sensor_;  
    VEDirectHexSensor *main_battery_current_sensor_;  
    VEDirectHexSensor *main_battery_power_sensor_;  
    VEDirectHexSensor *main_battery_cons_ah_sensor_;  
    VEDirectHexSensor *main_battery_soc_sensor_; 


    uint8_t bin_2_hex(uint8_t bin);
    uint8_t ved_encode_frame(VedBuffer *vedata);
    uint8_t hex_2_bin(uint8_t hex);
    uint8_t ved_decode_frame(VedBuffer *vedata, char inByte);

    uint8_t ved_get_command(VedBuffer *vedata);
    uint16_t ved_get_id(VedBuffer *vedata);
    void ved_set_command(VedBuffer *vedata, uint8_t value);
    void ved_set_id(VedBuffer *vedata, uint16_t value);

    int32_t ved_get_u8(VedBuffer *vedata);
    int32_t ved_get_u16(VedBuffer *vedata);
    int32_t ved_get_u32(VedBuffer *vedata);
    int32_t ved_get_s16(VedBuffer *vedata);
    void ved_set_u8(VedBuffer *vedata, uint8_t value);
    void ved_set_u16(VedBuffer *vedata, uint16_t value);
    
    uint32_t get_sensor(uint16_t id);
    uint32_t set_number(uint16_t id, int32_t value, uint8_t length);

};

}  // namespace ve_direct_hex
}  // namespace esphome
