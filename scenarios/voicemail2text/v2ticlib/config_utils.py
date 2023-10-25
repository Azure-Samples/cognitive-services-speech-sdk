import os
import configparser
from configparser import ExtendedInterpolation
import ast
from timelength import TimeLength
import v2ticlib.constants.constants as Constants

config = configparser.ConfigParser(interpolation=ExtendedInterpolation())
config.read("etc/config/config.properties")

config_base = Constants.V2TIC

def get_property(base:str, property:str, literal_eval=False):
    base = base.replace('.', '_')
    env_property = property.replace('.', '_')
    value = os.getenv('_'.join([base, env_property]))
    if value is None:
        value = config.get(base, property)

    if literal_eval == False:
        return value

    return ast.literal_eval(value)

def get_timelength_property(base:str, property:str) -> TimeLength:
    value = get_property(base, property)
    return TimeLength(value)

def get_timelength_property_secs(base:str, property:str) -> int:
    timelength: TimeLength = get_timelength_property(base, property)
    return timelength.to_seconds()

def get_timelength_property_msecs(base:str, property:str) -> int:
    timelength: TimeLength = get_timelength_property(base, property)
    return timelength.to_milliseconds()

def has_property(base:str, property:str):
    return config.has_option(base, property)

def get_logging_level():
    return get_property(config_base, 'logging_level')

def get_host():
    return get_property(config_base, 'host')

def get_port():
    return get_property(config_base, 'port', literal_eval=True)

def get_cert_file():
    return get_property(config_base, 'cert_file')

def get_key_file():
    return get_property(config_base, 'key_file')

def get_consume_request_timeout():
    return get_timelength_property_secs(config_base, 'consume_request_timeout')

def get_coroutine_execution_default_timeout():
    return get_timelength_property_secs(config_base, 'coroutine_execution_default_timeout')