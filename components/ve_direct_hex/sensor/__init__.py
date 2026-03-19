import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_BATTERY_VOLTAGE,
    CONF_UNIT_OF_MEASUREMENT,
    CONF_ACCURACY_DECIMALS,
    CONF_ICON,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLTAGE,
    ICON_CURRENT_AC,
    ICON_EMPTY,
    ICON_FLASH,
    ICON_GAUGE,
    ICON_PERCENT,
    ICON_POWER,
    ICON_TIMELAPSE,
    UNIT_EMPTY,
    UNIT_VOLT,
    UNIT_WATT,
    UNIT_AMPERE
)

from .. import (
    CONF_VE_DIRECT_HEX_ID,
    VE_DIRECT_HEX_COMPONENT_SCHEMA,
    ve_direct_hex_ns,
)

DEPENDENCIES = ["ve_direct_hex"]

CODEOWNERS = ["@acmetelka"]

# SmartSolar
CONF_BATTERY_CURRENT = "battery_current"
CONF_PANEL_VOLTAGE = "panel_voltage"
CONF_PANEL_CURRENT = "panel_current"
CONF_PANEL_POWER = "panel_power"
CONF_DEVICE_MODE = "device_mode"
CONF_DEVICE_STATE_ID = "device_state_id"
CONF_NETWORK_MODE_ID = "network_mode_id"

# SmartShunt 
CONF_MAIN_BATT_VOLTAGE = "main_battery_voltage"
CONF_AUX_BATT_VOLTAGE = "aux_battery_voltage"
CONF_MAIN_BATT_CURRENT = "main_battery_current"
CONF_MAIN_BATT_POWER = "main_battery_power"
CONF_MAIN_BATT_CONS_AH = "main_battery_cons_ah"
CONF_MAIN_BATT_SOC = "main_battery_soc"

SENSORS = {
    # address, factor, length
    CONF_BATTERY_VOLTAGE : [ 0xEDD5, 100, 2 ],
    CONF_BATTERY_CURRENT : [ 0xEDD7, 10, 2 ],
    CONF_PANEL_VOLTAGE : [ 0xEDBB, 100, 2 ],
    CONF_PANEL_CURRENT : [ 0xEDBD, 10, 2 ],
    CONF_PANEL_POWER : [ 0xEDBC, 100, 4 ],
    CONF_DEVICE_MODE : [ 0x0200, 1, 1 ],
    CONF_DEVICE_STATE_ID : [ 0x0201, 1, 1 ],
    CONF_NETWORK_MODE_ID : [ 0x200E, 1, 1 ],

    # SmartShunt registers
    CONF_MAIN_BATT_VOLTAGE : [ 0xED8D, 100, 2 ],
    CONF_AUX_BATT_VOLTAGE : [ 0xED7D, 100, 2 ],
    CONF_MAIN_BATT_CURRENT : [ 0xED8F, 10, -2 ],
    CONF_MAIN_BATT_POWER : [ 0xED8E, 1, -2 ],
    CONF_MAIN_BATT_CONS_AH : [ 0xEEFF, 10, 4 ],
    CONF_MAIN_BATT_SOC : [ 0x0FFF, 100, 2 ],

}

VEDirectHexSensor = ve_direct_hex_ns.class_('VEDirectHexSensor', sensor.Sensor, cg.Component)


VEDIRECTHEXSENSOR_SCHEMA = sensor.SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(VEDirectHexSensor),
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = VE_DIRECT_HEX_COMPONENT_SCHEMA.extend(
    {
        # SmartSolar sensors
        cv.Optional(CONF_BATTERY_VOLTAGE): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=2): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_BATTERY_CURRENT): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=1): cv.int_range(min=0, max=3),
            }
        ),
        cv.Optional(CONF_PANEL_VOLTAGE): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=2): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_PANEL_CURRENT): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=1): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_PANEL_POWER): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_WATT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=2): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_DEVICE_MODE): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_EMPTY): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=0): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_DEVICE_STATE_ID): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_EMPTY): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=0): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_NETWORK_MODE_ID): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_EMPTY): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=0): cv.int_range(min=0, max=6),
            }
        ),
        # SmartShunt sensors
        cv.Optional(CONF_MAIN_BATT_VOLTAGE): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=2): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_AUX_BATT_VOLTAGE): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=2): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_MAIN_BATT_CURRENT): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=1): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_MAIN_BATT_POWER): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=0): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_MAIN_BATT_CONS_AH): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=3): cv.int_range(min=0, max=6),
            }
        ),
        cv.Optional(CONF_MAIN_BATT_SOC): VEDIRECTHEXSENSOR_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_FLASH): cv.icon,
                cv.Optional(CONF_ACCURACY_DECIMALS, default=2): cv.int_range(min=0, max=6),
            }
        ),
    }
)

async def to_code(config):
    hub = await cg.get_variable(config[CONF_VE_DIRECT_HEX_ID])

    #print(f"Registering VEDirectHexSensor with ID: {config[CONF_VE_DIRECT_HEX_ID]}")

    for key, address in SENSORS.items():
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
            cg.add(sens.set_parent(hub))
            await sensor.register_sensor(sens, conf)
            cg.add(sens.set_holding_register(address[0]))  # Assign the holding register
            # Fetch and set the ved_format for this sensor
            #ved_format = conf.get("ved_format", "un16")  # Default to "un16" if not specified
            #cg.add(sens.set_ved_format(ved_format))
            cg.add(sens.set_factor(address[1]))
            cg.add(sens.set_length(address[2]))            