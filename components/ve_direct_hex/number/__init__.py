import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_ENTITY_CATEGORY,
    CONF_ICON,
    CONF_ID,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_MODE,
    CONF_STEP,
    CONF_UNIT_OF_MEASUREMENT,
    ENTITY_CATEGORY_CONFIG,
    ICON_EMPTY,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_HOUR,
    UNIT_VOLT,
)

from .. import (
    CONF_VE_DIRECT_HEX_ID,
    VE_DIRECT_HEX_COMPONENT_SCHEMA,
    ve_direct_hex_ns,
)

CODEOWNERS = ["@acmetelka"]

DEPENDENCIES = ["ve_direct_hex"]

CONF_CHARGE_CURRENT_LIMIT = "charge_current_limit"
CONF_NETWORK_MODE = "network_mode"

NUMBERS = {
    # address, factor, length
    CONF_CHARGE_CURRENT_LIMIT: [0x2015, 10, 2],
    CONF_NETWORK_MODE : [0x200E, 1, 1],
}

VEDirectHexNumber = ve_direct_hex_ns.class_("VEDirectHexNumber", number.Number, cg.Component)

VEDIRECTHEXNUMBER_SCHEMA = number.NUMBER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(VEDirectHexNumber),
        cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
        cv.Optional(CONF_MIN_VALUE, default=0.0): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=100.0): cv.float_,
        cv.Optional(CONF_STEP, default=0.1): cv.float_,
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = VE_DIRECT_HEX_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CHARGE_CURRENT_LIMIT): VEDIRECTHEXNUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=100.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
                ): cv.string_strict,
                # cv.Optional("ved_format", default="un16"): cv.one_of("un16", lower=True),
            }
        ),
        cv.Optional(CONF_NETWORK_MODE): VEDIRECTHEXNUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_STEP, default=1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_EMPTY
                ): cv.string_strict,
                # cv.Optional("ved_format", default="un8"): cv.one_of("un8", lower=True),
            }
        ),
    }
)

async def to_code(config):
    hub = await cg.get_variable(config[CONF_VE_DIRECT_HEX_ID])
    for key, address in NUMBERS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await number.register_number(
                var,
                conf,
                min_value=conf[CONF_MIN_VALUE],
                max_value=conf[CONF_MAX_VALUE],
                step=conf[CONF_STEP],
            )
            cg.add(getattr(hub, f"set_{key}_number")(var))
            cg.add(var.set_parent(hub))  
            cg.add(var.set_holding_register(address[0]))  # Assign the holding register            
            cg.add(var.set_factor(address[1]))
            cg.add(var.set_length(address[2]))