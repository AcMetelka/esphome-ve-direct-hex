#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace ve_direct_hex {

class VEDirectHexComponent;

class VEDirectHexNumber : public number::Number, public Component {
 
 public:
  void set_parent(VEDirectHexComponent *parent) { parent_ = parent; }
  
  //void publish_state(float state) override;
      
  void handle_value(float raw_value);  

  void control(float value) override;

  float get_state() const { return this->state; }
  void set_state( float value) { /*this->state = value;*/ this->publish_state(value); }
  float get_new_value() const { return this->new_value_; }

  bool has_pending_update() const { return this->pending_update_; }
  void clear_pending_update() { this->pending_update_ = false; }
    
  void set_holding_register(uint16_t holding_register) {
  this->holding_register_ = holding_register;
  }
  uint16_t get_holding_register() const {  // Add this getter
      return this->holding_register_;
  }  

  void set_factor(float factor) { this->factor_ = factor; };
  void set_length(uint8_t length) { this->length_ = length; }
  float get_factor() { return this->factor_; };
  uint8_t get_length() { return this->length_; }

  void dump_config() override;
  
 private:

  VEDirectHexComponent *parent_{nullptr};
  float new_value_;
  bool pending_update_{false};
  
  int16_t holding_register_;
  uint8_t length_;      // Length of data in register in bytes
  float factor_{1.0f};  // Multiplying factor or the value
};

}  // namespace ve_direct_hex
}  // namespace esphome
