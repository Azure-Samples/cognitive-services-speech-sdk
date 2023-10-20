import configparser
import os

config = configparser.ConfigParser()
current_dir = os.path.dirname(os.path.abspath(__file__))
env_file_name = os.path.join(current_dir,'..','test_config.properties')

config.read(env_file_name)


def get_config_value(base, property):
    return config.get(base, property)
