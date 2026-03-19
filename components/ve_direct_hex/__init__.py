import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

AUTO_LOAD = ["number","sensor","text_sensor"]

MULTI_CONF = True

CODEOWNERS = ["@acmetelka"]

DEPENDENCIES = ["uart"]

CONF_VE_DIRECT_HEX_ID = "ve_direct_hex_id"
CONF_INITIAL_DELAY = "initial_delay"

ve_direct_hex_ns = cg.esphome_ns.namespace('ve_direct_hex')
VEDirectHexComponent = ve_direct_hex_ns.class_('VEDirectHexComponent', uart.UARTDevice, cg.Component)

VE_DIRECT_HEX_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_VE_DIRECT_HEX_ID): cv.use_id(VEDirectHexComponent),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(VEDirectHexComponent),
        cv.Required("uart_id"): cv.use_id(uart.UARTComponent),
        cv.Optional(CONF_UPDATE_INTERVAL, default='2000ms'): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_INITIAL_DELAY, default='0ms'): cv.positive_time_period_milliseconds,
    }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    uart_component = await cg.get_variable(config["uart_id"])
    uart_name = config["uart_id"].id  # Get the ID as a string
    var = cg.new_Pvariable(config[CONF_ID], uart_component, uart_name)
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    
    if CONF_UPDATE_INTERVAL in config:
        cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL].total_milliseconds))
    if CONF_INITIAL_DELAY in config:
        cg.add(var.set_initial_delay(config[CONF_INITIAL_DELAY].total_milliseconds))