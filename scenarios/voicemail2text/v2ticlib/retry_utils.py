from tenacity import retry, stop_after_attempt, wait_chain, retry_if_exception_type, RetryCallState, wait_fixed
from v2ticlib import config_utils
from v2ticlib.logger_utils import log
from v2ticlib.converter_utils import str_to_class

def get_property(config_base, property, literal_eval=False):
    return config_utils.get_property(config_base, property, literal_eval=literal_eval)

def get_retry_max_attempts(config_base):
    return get_property(config_base, "retry_max_attempts", literal_eval=True)

def get_retry_wait_exp_backoff(config_base):
    return get_property(config_base, "retry_wait_exponential_backoff", literal_eval=True)

def get_retry_on_exception(config_base):
    exception_names = get_property(config_base, "retry_on_exception").strip().split(',')
    return tuple(str_to_class(name) for name in exception_names)

def is_first_attempt(retry_state: RetryCallState):
    return retry_state.attempt_number == 1

def before_callback_log(retry_state: RetryCallState):
    if is_first_attempt(retry_state):
        return
    #return one less than attempt number to account for first attempt
    log(f"Attempting retry {retry_state.attempt_number-1} ...")

def after_callback_log(retry_state: RetryCallState):
    #after callback will be triggered only in case of failed attempt
    if is_first_attempt(retry_state):
        return
    #return one less than attempt number to account for first attempt
    log(f"Retry failed on attempt {retry_state.attempt_number-1}")

def retry_with_custom_strategy(config_base):
    return retry(
        before=before_callback_log,
        after=after_callback_log,
        retry=retry_if_exception_type(get_retry_on_exception(config_base)),
        stop=stop_after_attempt(get_retry_max_attempts(config_base)+1),
        wait=retry_wait_config(config_base),
        reraise=True
    )

def retry_wait_config(config_base):
    max_attempts = get_retry_max_attempts(config_base)
    exponential_backoff = get_retry_wait_exp_backoff(config_base)
    
    if max_attempts < 1:
        return None

    if exponential_backoff is None or len(exponential_backoff) == 0:
        return None

    wait_fix_list = []
    backoff_list_max_index = len(exponential_backoff) - 1    
    for i in range(max_attempts):
        index = min(i, backoff_list_max_index)
        wait_fix_list.append(wait_fixed(exponential_backoff[index]))
    return wait_chain(*wait_fix_list)