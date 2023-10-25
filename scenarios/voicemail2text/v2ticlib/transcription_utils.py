import v2ticlib.constants.fields as Fields
import v2ticlib.config_utils as config_utils

def get_property(property, literal_eval=False):
    return config_utils.get_property(Fields.METADATA, property, literal_eval=literal_eval)

def is_truncate_lenghty_transcriptions_enabled() -> bool:
    return get_property('truncate_lenghty_transcriptions_enabled', literal_eval=True)

def get_default_max_transcription_length() -> int:
    return get_property('default_max_transcription_length', literal_eval=True)

def is_log_transcriptions_enabled() -> bool:
    return get_property('log_transcriptions_enabled', literal_eval=True)