from tenacity import retry, stop_after_attempt, wait_exponential, retry_if_exception_type, RetryCallState
from v2ticlib import config_utils
from v2ticlib.logger_utils import log
from v2ticlib.converter_utils import str_to_class
import v2ticlib.constants.constants as Constants

def get_property(config_base, property, literal_eval=False):
    return config_utils.get_property(config_base, property, literal_eval=literal_eval)

def get_retry_max_attempts(config_base):
    return get_property(config_base, "max_attempts", literal_eval=True)

def get_retry_wait_exp_multiplier(config_base):
    return get_property(config_base, "wait_exponential_multiplier", literal_eval=True)

def get_retry_wait_exp_min(config_base):
    return config_utils.get_timelength_property_secs(config_base, "wait_exponential_min")

def get_retry_wait_exp_max(config_base):
    return config_utils.get_timelength_property_secs(config_base, "wait_exponential_max")

def get_retry_on_exception(config_base):
    exception_names = get_property(config_base, "retry_on_exception").strip().split(',')
    return tuple(str_to_class(name) for name in exception_names)

def is_first_attempt(retry_state: RetryCallState):
    return retry_state.attempt_number == 1

def before_callback_log(retry_state: RetryCallState):
    if is_first_attempt(retry_state):
        return
    log(f"Attempting retry {retry_state.attempt_number} ...")

def after_callback_log(retry_state: RetryCallState):
    #after callback will be triggered only in case of failed attempt
    log(f"Retry failed on attempt {retry_state.attempt_number}")

def retry_with_custom_strategy(config_base):
    return retry(
        before=before_callback_log,
        after=after_callback_log,
        retry=retry_if_exception_type(get_retry_on_exception(config_base)),
        stop=stop_after_attempt(get_retry_max_attempts(config_base)),
        wait=wait_exponential(multiplier=get_retry_wait_exp_multiplier(config_base),
                              min=get_retry_wait_exp_min(config_base),
                              max=get_retry_wait_exp_max(config_base)),
        reraise=True
    )