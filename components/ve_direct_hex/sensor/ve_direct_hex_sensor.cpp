

#include "../ve_direct_hex.h"
#include "ve_direct_hex_sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ve_direct_hex {

//static const char *const TAG = "ve_direct_hex";

void VEDirectHexSensor::handle_value_(int32_t raw_value) {

  float formatted_value;
  const std::string &sensor_name = this->get_name();

  if (sensor_name == "battery_voltage"
      ||sensor_name == "battery_current"
      ||sensor_name == "panel_voltage"
      ||sensor_name == "panel_current"
      ||sensor_name == "panel_power"
      ||sensor_name == "device_mode"
      ||sensor_name == "main_battery_voltage"
      ||sensor_name == "aux_battery_voltage"
      ||sensor_name == "main_battery_current"
      ||sensor_name == "main_battery_power"
      ||sensor_name == "main_battery_cons_ah"
      ||sensor_name == "main_battery_soc"
    )
  {
    formatted_value = raw_value / this->get_factor();
    this->publish_state(formatted_value); 
  } else if (sensor_name == "device_state_id")
  {
    formatted_value = raw_value / this->get_factor();
    this->publish_state(formatted_value); 
    const std::string &var = this->parent_->device_mode_str(static_cast<int>(formatted_value));
    this->parent_->publish_state_(this->parent_->device_state_sensor_, var);
  } else if (sensor_name == "network_mode_id")
  {
    formatted_value = raw_value / this->get_factor();
    this->publish_state(formatted_value); 
    const std::string &var = this->parent_->network_mode_str(static_cast<int>(formatted_value));
    this->parent_->publish_state_(this->parent_->network_mode_sensor_, var);
  } else {
    ;
    //ESP_LOGW(TAG, "Unknown sensor name: %s. Using raw value.", sensor_name.c_str());      
  }
}

}  // namespace ve_direct_hex
}  // namespace esphome
