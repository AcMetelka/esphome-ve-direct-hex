# ESPHome VE.Direct HEX Component (Victron)

Custom ESPHome component for communication with Victron devices (MPPT, SmartShunt, etc.) using the **VE.Direct HEX protocol over UART**.

👉 This component supports both:

* 📊 **Reading data**
* 🎛️ **Controlling Victron devices (WRITE commands)**

---

## 🚀 Features

* 📡 Communication via **VE.Direct HEX (UART TTL)**
* 🔋 Supports:

  * Victron SmartSolar MPPT
  * Victron SmartShunt
* 📊 Read values via:

  * `sensor`
  * `text_sensor`
* 🎛️ **Control Victron settings via `number` entities**

  * e.g. charge current limit, voltages, etc.
* ⚡ Fast and lightweight (runs directly on ESP32)
* 🔧 Native **Home Assistant integration via ESPHome**

---

## 📦 Installation

```yaml
external_components:
  - source: github://AcMetelka/ve_direct_hex_esphome
```

---

## 🔌 Hardware

* ESP32 (tested)
* Victron device with VE.Direct port
* VE.Direct → UART cable (or custom wiring)

---

## 🔧 VE.Direct Cable Wiring

### Typical VE.Direct → UART (TTL) connection:

![Image](https://communityarchive.victronenergy.com/storage/attachments/64361-1699055091736.png)

![Image](https://communityarchive.victronenergy.com/storage/attachments/50585-schematic-opto-2023-03-19.png)

![Image](https://europe1.discourse-cdn.com/victron/original/3X/b/7/b78c2cb70be133f87b0ce76d9750030573eeadf6.jpeg)

![Image](https://beta.ivc.no/wiki/images/thumb/f/fc/Victron_cable_unit_pinout.jpg/500px-Victron_cable_unit_pinout.jpg)

| VE.Direct wire  | Function          | ESP32      |
| --------------- | ----------------- | ---------- |
| **GND (black)** | Ground            | GND        |
| **TX (yellow)** | Data from Victron | RX (ESP32) |
| **RX (blue)**   | Data to Victron   | TX (ESP32) |

⚠️ Notes:

* Communication is **TTL (not RS232!)**
* Cross connection:

  * Victron TX → ESP RX
  * Victron RX → ESP TX
* Not all cheap cables expose RX (write won’t work then!)

---

## ⚙️ Configuration Example

### 📊 + 🎛️ Example: MPPT + SmartShunt (read + control)

```yaml
logger:
  baud_rate: 0   # important if sharing UART

uart:
  id: uart_bus
  rx_pin: GPIO16
  tx_pin: GPIO17
  baud_rate: 19200

external_components:
  - source: github://AcMetelka/ve_direct_hex_esphome

ve_direct_hex:
  id: victron

# -------- READ VALUES --------
sensor:
  - platform: ve_direct_hex
    name: "Battery Voltage"
    type: battery_voltage

  - platform: ve_direct_hex
    name: "PV Power"
    type: pv_power

text_sensor:
  - platform: ve_direct_hex
    name: "MPPT State"
    type: state

# -------- CONTROL (WRITE) --------
number:
  - platform: ve_direct_hex
    name: "Charge Current Limit"
    type: charge_current_limit
    min_value: 0
    max_value: 50
    step: 1
```

---

## 🎛️ Device Control (WRITE)

This component allows sending **VE.Direct HEX commands back to Victron**.

Control is exposed via ESPHome `number` entities:

```yaml
number:
  - platform: ve_direct_hex
    name: "Charge Current Limit"
    type: charge_current_limit
```

👉 Changing this value in Home Assistant will:

* send HEX command via UART
* update Victron device setting in real time

---

## 🧠 How It Works

* ESP32 listens on UART for **VE.Direct HEX frames**
* Frames are parsed in C++ component
* Values are mapped to ESPHome entities
* `number` entities send **write commands back to device**

---

## 🔥 Use Cases

- Limit charging current based on solar production
- Integrate Victron into Home Assistant automations
- Dynamic battery control (FVE)

---

## ⚠️ Important Notes

* ⚠️ **Write requires TX connection (ESP → Victron RX)**
* Some VE.Direct cables are **read-only**
* Always verify your cable supports **bidirectional communication**

---

## 🐛 Troubleshooting

### Cannot control device (read works)

* Check TX wire is connected
* Verify cable supports RX line
* Check UART `tx_pin` is configured

### No data received

* Check wiring (TX/RX swapped?)
* Verify baudrate = `19200`
* Disable logger UART:

  ```yaml
  logger:
    baud_rate: 0
  ```

---

## 🛣️ Roadmap

* [ ] More writable parameters
* [ ] Binary sensors (alarms)
* [ ] Auto device detection
* [ ] Multi-device support on one UART

---

## 📄 License

MIT License

---

## 🙌 Contribution

PRs welcome!

If you're using this for Victron control via ESPHome, share your setup 🚀
