

#include "../ve_direct_hex.h"

#include "ve_direct_hex_number.h"
#include "esphome/core/log.h"


namespace esphome {
namespace ve_direct_hex {

//static const char *const TAG = "ve_direct_hex";

void VEDirectHexNumber::dump_config() { LOG_NUMBER("", "VEDirectHexNumber Number", this); }

void VEDirectHexNumber::control(float value) {
  ESP_LOGD(TAG, "Setting new value: %.2f", value);

  // Save the new value
  this->new_value_ = value;
  this->pending_update_ = true; // Mark that a new value was received for processing
}

void VEDirectHexNumber::handle_value(float raw_value) {
  float formatted_value = raw_value / this->get_factor(); 
  const std::string &number_name = this->get_name();

  if (number_name == "network_mode") {
        this->publish_state(formatted_value);
        this->parent_->publish_state_(this->parent_->network_mode_id_sensor_, static_cast<int>(formatted_value));
        const std::string &var = this->parent_->network_mode_str(static_cast<int>(formatted_value));
        this->parent_->publish_state_(this->parent_->network_mode_sensor_, var);
  } else if (number_name == "charge_current_limit") {
        this->publish_state(formatted_value);
  } else {
    //ESP_LOGW(TAG, "Unknown sensor name: %s. Using raw value.", number_name.c_str());
  }
}


}  // namespace ve_direct_hex
}  // namespace esphome
