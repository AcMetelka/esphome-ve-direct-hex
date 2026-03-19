#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace ve_direct_hex {

class VEDirectHexComponent;

  class VEDirectHexSensor : public sensor::Sensor, public Component {
  public:

    VEDirectHexSensor() = default;

    void set_parent(VEDirectHexComponent *parent) { this->parent_ = parent; };

    void set_holding_register(uint16_t holding_register) {
      this->holding_register_ = holding_register;
    }
    uint16_t get_holding_register() const {
      return this->holding_register_;
    }
    
    void handle_value_(int32_t raw_value);   

    void set_factor(float factor) { this->factor_ = factor; };
    void set_length(int8_t length) { this->length_ = length; }
    float get_factor() { return this->factor_; };
    int8_t get_length() { return this->length_; }

  protected:
  
    VEDirectHexComponent *parent_{nullptr};

    int16_t holding_register_;
    int8_t length_;      // Length of data in register in bytes
    float factor_{1.0f};  // Multiplying factor or the value

    //const std::string device_mode_text_(int value);
    
};

}  // namespace ve_direct_hex
}  // namespace esphome
