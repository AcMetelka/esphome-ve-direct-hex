import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ICON, CONF_NAME, CONF_ID, ICON_EMPTY

from . import (
    CONF_VE_DIRECT_HEX_ID,
    VE_DIRECT_HEX_COMPONENT_SCHEMA,
    ve_direct_hex_ns,
)

DEPENDENCIES = ["ve_direct_hex"]

CODEOWNERS = ["@acmetelka"]

CONF_DEVICE_STATE = "device_state"
CONF_NETWORK_MODE = "network_mode"

TEXT_SENSORS = [
    CONF_DEVICE_STATE,
    CONF_NETWORK_MODE,
]

CONFIG_SCHEMA = text_sensor.TEXT_SENSOR_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
    cv.Optional(CONF_ICON, default='mdi:information-outline'): cv.icon,
}).extend(cv.COMPONENT_SCHEMA)


CONFIG_SCHEMA = VE_DIRECT_HEX_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_DEVICE_STATE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
            }
        ),
        cv.Optional(CONF_NETWORK_MODE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VE_DIRECT_HEX_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
