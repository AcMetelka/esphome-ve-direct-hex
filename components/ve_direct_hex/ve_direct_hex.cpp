
#include "ve_direct_hex.h"

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ve_direct_hex {

  // function header codes
  static const uint8_t VED_PING_COMMAND = 0x01;
  // static const uint8_t VEDirect_kRestartCommand = 0x06;
  static const uint8_t VED_SET_VALUE_COMMAND = 0x08;
  static const uint8_t VED_GET_VALUE_COMMAND = 0x07;
  // static const uint8_t VEDirect_kAsyncCommand = 0x0A;

  // read/write header codes
  static const uint8_t FUNCTION_READ_REGISTER = 0x02;
  static const uint8_t FUNCTION_WRITE_REGISTER = 0x02;

  VEDirectHexComponent::VEDirectHexComponent(uart::UARTComponent *uart, std::string uart_id)
      : uart::UARTDevice(uart), uart_(uart), uart_id_(uart_id) {}

  void VEDirectHexComponent::setup()
  {

    // Clear UART Input Buffer
    while (this->available())
    {
      this->read(); // Read and discard any bytes in the buffer
    }
    ESP_LOGD(TAG, "UART input buffer cleared.");
  }

  void VEDirectHexComponent::loop()
  {

    // Check if the interval has elapsed, if not, it is the mean time to check for incoming data
    // which are comming each second as TEXT data or at any time as :A.. async HEX data or,
    // as response to sent command
    if (millis() - ( this->last_update_ + this->initial_delay_ ) >= this->update_interval_ && !update_in_progress_)
    {
      this->last_update_ = millis() - this->initial_delay_;
      comm_stage_ = 1;
    }

    // stage 1 - request for all sensors data
    // stage 2 - receive response for sensor
    // stage 3 - number value changed, request for update

    // Check if any number has been updated
    if (!update_in_progress_)
    {
      for (size_t i = 0; i < this->numbers_.size(); i++)
      {
        auto *number = this->numbers_[i];
        // ESP_LOGI(TAG, "Querying update for number: %s", number->get_name().c_str());
        if (number->has_pending_update())
        {
          ESP_LOGV_UART("Instance[%d]: Querying update: '%s', value: %f", this->instance_id_, number->get_name().c_str(), number->get_new_value());
          current_number_index_ = i;
          comm_stage_ = 3;
          update_in_progress_ = true;
          break;
        }
      }
    }

    // Stage 1 - query for new sensor data
    if (comm_stage_ == 1 || comm_stage_ == 3)
    {

      ESP_LOGV_UART("Polling data started at: %d", (millis() - (this->last_update_ + this->initial_delay_)));

      // Clear UART Input Buffer
      while (this->available())
      {
        uint8_t discard_buffer[256];
        size_t bytes_to_read = this->available();
        if (bytes_to_read > sizeof(discard_buffer))
        {
          bytes_to_read = sizeof(discard_buffer);
        }
        int bytes_read = this->read_array(discard_buffer, bytes_to_read);
        ESP_LOGV_UART("Discarded %d bytes from UART buffer.", bytes_to_read);
        if (bytes_read <= 0)
        {
          break;
        }
      }

      if (comm_stage_ == 1)
      {
        // Query for actual sensor  // Handle the next sensor
        if (this->current_sensor_index_ < this->sensors.size())
        {
          auto *sensor = this->sensors[this->current_sensor_index_];
          // ESP_LOGD(TAG, "Querying value for sensor: %s", sensor->get_name().c_str());

          this->get_sensor(sensor->get_holding_register());
          rxBuffer_ = {};  // Initialize the response buffer
          comm_stage_ = 2; // Next phase is receive response
          ESP_LOGD_UART("Sensor '%s' query sent at: %d", sensor->get_name().c_str(), (millis() - this->last_update_));
          return;
        }
        else
        {
          ESP_LOGD_UART("No sensors available to query.");
        }
      }

      if (comm_stage_ == 3)
      {
        // Send command to update number
        auto *number = this->numbers_[this->current_number_index_];
        uint16_t formatted_value = static_cast<uint16_t>((number->get_new_value() * number->get_factor()));
        ESP_LOGD(TAG, "Sending value %d", formatted_value);
        this->set_number(number->get_holding_register(), formatted_value, number->get_length());
        rxBuffer_ = {};  // Initialize the response buffer
        comm_stage_ = 2; // Next phase is receive response
        ESP_LOGD(TAG, "Number '%s' update query sent at: %d", number->get_name().c_str(), (millis() - this->last_update_));
        return;
      }
    }

    // Stage 2 - receive response for queried sensor data or input number update
    if (comm_stage_ == 2)
    {
      ESP_LOGD_UART("Stage 2 - UART receive response for sensor.");

      if (this->available())
      {

        while (this->available())
        {
          uint8_t byte = this->read(); // Read a single byte

          uint8_t rxCount = this->ved_decode_frame(&rxBuffer_, byte);
          if (rxCount > 0)
          {
            ESP_LOGD_UART("Sensor response recvd at: %d", (millis() - this->last_update_));

            // ESP_LOGI(TAG, "Received response: %s", reinterpret_cast<char *>(rxBuffer_.data_));
            //  Log the hex-dump of the response

            if (this->ved_get_command(&rxBuffer_) == VED_GET_VALUE_COMMAND && !update_in_progress_)
            {
              // Decode value based on sensor address
              for (auto *sensor : this->sensors)
              {
                if (sensor->get_holding_register() == this->ved_get_id(&rxBuffer_))
                {
                  ESP_LOGD_UART("Found sensor with holding register: 0x%04X", sensor->get_holding_register());
                  // Perform operations with the sensor
                  int32_t sensor_val{0};
                  if (sensor->get_length() == 1)
                    sensor_val = this->ved_get_u8(&rxBuffer_);
                  if (sensor->get_length() == 2)
                    sensor_val = this->ved_get_u16(&rxBuffer_);
                  if (sensor->get_length() == 4)
                    sensor_val = this->ved_get_u32(&rxBuffer_);

                  if (sensor->get_length() == -2)
                    sensor_val = this->ved_get_s16(&rxBuffer_);

                  sensor->handle_value_(sensor_val);
                }
              }

              this->current_sensor_index_ = (this->current_sensor_index_ + 1) % this->sensors.size(); // Move to next sensor
              if (current_sensor_index_ == 0)
              {
                comm_stage_ = 0;
                ESP_LOGD_UART("Update sensors in time: %d", (millis() - (this->last_update_ + this->initial_delay_)));
              }
              else
                comm_stage_ = 1;

              // ESP_LOGD(TAG, "Received response: %s", reinterpret_cast<char *>(rxBuffer.data_));
              // ESP_LOGD(TAG, "Received response: %s", (char *)rxBuffer.data_);
              break;
            }

            if (this->ved_get_command(&rxBuffer_) == VED_SET_VALUE_COMMAND)
            {
              auto *number = this->numbers_[this->current_number_index_];
              if (number->get_holding_register() == this->ved_get_id(&rxBuffer_))
              {

                int32_t sensor_val{0};
                if (number->get_length() == 1)
                  sensor_val = this->ved_get_u8(&rxBuffer_);
                if (number->get_length() == 2)
                  sensor_val = this->ved_get_u16(&rxBuffer_);
                if (number->get_length() == 4)
                  sensor_val = this->ved_get_u32(&rxBuffer_);

                if ((static_cast<float>(sensor_val) / number->get_factor()) == number->get_new_value())
                {
                  ESP_LOGD(TAG, "Correct decoded value: %06d", sensor_val);
                  // number->set_state(static_cast<float>(sensor_val)/number->get_factor());
                  number->handle_value(static_cast<float>(sensor_val));
                  number->clear_pending_update();
                  update_in_progress_ = false;
                  comm_stage_ = 0;
                }

                // int32_t sensor_val{0};
                // sensor_val = this->ved_get_u8(&rxBuffer_);
                // ESP_LOGI(TAG, "Decoded value: %06d", sensor_val);
              }
            }
          }
        }
      } // if available
    }
  }

  uint32_t VEDirectHexComponent::set_number(uint16_t id, int32_t value, uint8_t length)
  {
    VedBuffer txBuffer;
    ved_set_command(&txBuffer, VED_SET_VALUE_COMMAND);
    ved_set_id(&txBuffer, id);
    if (length == 1)
      ved_set_u8(&txBuffer, value);
    if (length == 2)
      ved_set_u16(&txBuffer, value);

    ved_encode_frame(&txBuffer);

    if (txBuffer.size_ < sizeof(txBuffer.data_))
    {
      txBuffer.data_[txBuffer.size_] = '\0'; // Add null-terminator
    }
    ESP_LOGD_UART("Sending frame as raw string: %s,  %02d", reinterpret_cast<char *>(txBuffer.data_), txBuffer.size_);
    this->write_array(txBuffer.data_, txBuffer.size_);
    return txBuffer.size_;
  }

  uint32_t VEDirectHexComponent::get_sensor(uint16_t id)
  {
    VedBuffer txBuffer;
    ved_set_command(&txBuffer, VED_GET_VALUE_COMMAND);
    ved_set_id(&txBuffer, id);
    ved_encode_frame(&txBuffer);

    if (txBuffer.size_ < sizeof(txBuffer.data_))
    {
      txBuffer.data_[txBuffer.size_] = '\0'; // Add null-terminator
    }
    ESP_LOGD_UART("Sending frame as raw string: %s,  %02d", reinterpret_cast<char *>(txBuffer.data_), txBuffer.size_);

    this->write_array(txBuffer.data_, txBuffer.size_);
    return txBuffer.size_;
  }
  uint8_t VEDirectHexComponent::ved_get_command(VedBuffer *vedata) { return vedata->data_[0]; }
  uint16_t VEDirectHexComponent::ved_get_id(VedBuffer *vedata)
  {
    return (((uint16_t)vedata->data_[2]) << 8) + (uint16_t)vedata->data_[1];
  }
  void VEDirectHexComponent::ved_set_command(VedBuffer *vedata, uint8_t value)
  {
    vedata->data_[0] = value;
    vedata->size_ = 1;
  }
  void VEDirectHexComponent::ved_set_id(VedBuffer *vedata, uint16_t value)
  {
    vedata->data_[1] = value;
    vedata->data_[2] = (value >> 8);
    vedata->data_[3] = 0;
    vedata->size_ = 4;
  }

  uint8_t VEDirectHexComponent::bin_2_hex(uint8_t bin)
  {
    bin &= 0x0F;
    if (bin < 10)
      return bin + '0';
    return bin + 'A' - 10;
  }

  uint8_t VEDirectHexComponent::ved_encode_frame(VedBuffer *vedata)
  {
    uint8_t *input = vedata->data_;
    uint8_t buffer[VED_BUFFER_SIZE];
    uint8_t *output = buffer;
    uint8_t csum = 0x00;
    uint8_t src = *input++;
    *output++ = ':';
    *output++ = bin_2_hex(src);
    csum -= src;
    while (input < vedata->data_ + vedata->size_)
    {
      src = *input++;
      *output++ = bin_2_hex(src >> 4);
      *output++ = bin_2_hex(src);
      csum -= src;
    }
    csum += 0x55;
    *output++ = bin_2_hex(csum >> 4);
    *output++ = bin_2_hex(csum);
    *output++ = '\n';
    *output = '\0';
    vedata->size_ = output - buffer;
    memcpy(vedata->data_, buffer, vedata->size_);
    return vedata->size_;
  }

  uint8_t VEDirectHexComponent::hex_2_bin(uint8_t hex)
  {
    if (hex < '0')
      return 0;
    uint8_t val = hex - '0';
    if (val > 9)
    {
      val -= 7;
      if ((val > 15) || (val < 10))
        return 0;
    }
    return val;
  }

  uint8_t VEDirectHexComponent::ved_decode_frame(VedBuffer *vedata, char inByte)
  {
    if (inByte == ':')
    {
      vedata->size_ = 0; // reset data buffer
    }
    if (vedata->size_ < VED_BUFFER_SIZE - 2)
    {
      vedata->data_[vedata->size_++] = inByte;
      if (inByte == '\n' && vedata->data_[1] != 'A')
      { // supress :A async messages
        vedata->data_[vedata->size_] = '\0';
        uint8_t *input = vedata->data_;
        uint8_t *output = vedata->data_;
        uint8_t csum = 0x00;
        while ((*input != '\n') && (*input != '\0'))
        {
          uint8_t byte = hex_2_bin(*input++) << 4;
          byte += hex_2_bin(*input++);
          csum += byte;
          *output++ = byte;
        }
        if (csum == 0x55)
        {
          vedata->size_ = output - vedata->data_ - 1;
          return vedata->size_; // size not including terminating null
        }
      }
    }
    return 0;
  }
  int32_t VEDirectHexComponent::ved_get_u8(VedBuffer *vedata)
  {
    return (uint32_t)vedata->data_[4];
  }
  int32_t VEDirectHexComponent::ved_get_u16(VedBuffer *vedata)
  {
    return (((uint32_t)vedata->data_[5]) << 8) + (uint32_t)vedata->data_[4];
  }
  int32_t VEDirectHexComponent::ved_get_u32(VedBuffer *vedata)
  {
    return (((uint32_t)vedata->data_[7]) << 24) +
           (((uint32_t)vedata->data_[6]) << 16) +
           (((uint32_t)vedata->data_[5]) << 8) + (uint32_t)vedata->data_[4];
  }

  int32_t VEDirectHexComponent::ved_get_s16(VedBuffer *vedata)
  {
    return (int32_t)(int16_t)((((uint16_t)vedata->data_[5]) << 8) | (uint16_t)vedata->data_[4]);
  }

  void VEDirectHexComponent::ved_set_u8(VedBuffer *vedata, uint8_t value)
  {
    vedata->data_[4] = value;
    vedata->size_ = 5;
  }
  void VEDirectHexComponent::ved_set_u16(VedBuffer *vedata, uint16_t value)
  {
    vedata->data_[5] = (value >> 8);
    vedata->data_[4] = value;
    vedata->size_ = 6;
  }

  void VEDirectHexComponent::dump_config()
  {
    ESP_LOGCONFIG("ve_direct_hex", "VE.Direct HEX UART Component");
  }

  void VEDirectHexComponent::publish_state_(VEDirectHexSensor *sensor, float value)
  {
    if (sensor == nullptr)
      return;

    sensor->publish_state(value);
  }
  void VEDirectHexComponent::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state)
  {
    if (text_sensor == nullptr)
      return;

    text_sensor->publish_state(state);
  }

  const std::string VEDirectHexComponent::device_mode_str(int value)
  {
    switch (value)
    {
    case 0:
      return "OFF";
    case 2:
      return "Fault";
    case 3:
      return "Bulk";
    case 4:
      return "Absorption";
    case 5:
      return "Float";
    case 6:
      return "Stprage";
    case 7:
      return "Manual equalise";
    case 245:
      return "Wake-up";
    case 247:
      return "Auto equalise";
    case 250:
      return "Blocked";
    case 252:
      return "External control";
    case 255:
      return "Unavailable";
    default:
      return "Unknown";
    }
  }

  const std::string VEDirectHexComponent::network_mode_str(int value)
  {
    // Define a lookup table for the known modes
    static const std::map<int, std::string> mode_map = {
        {0, "Standalone"},
        {3, "Slave mode"},
        {5, "Remote"},
        {9, "BMS controlled"},
        {17, "Charge group master"},
        {33, "Charge instance master"},
        {65, "Standby"}};
    // Check if the value is in the map
    auto it = mode_map.find(value);
    if (it != mode_map.end())
    {
      return it->second;
    }
    return "Unknown";
  }

}  // namespace ve_direct_hex
}  // namespace esphome
