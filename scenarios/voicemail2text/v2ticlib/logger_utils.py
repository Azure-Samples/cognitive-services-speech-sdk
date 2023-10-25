from threading import current_thread
from datetime import datetime
from v2ticlib import config_utils
import contextvars

global CONTEXT
CONTEXT = contextvars.ContextVar("SCRID", default='Unknown_Scrid')

def log(*args, **kwargs):
    id = CONTEXT.get()
    thread_id = current_thread().getName()
    print(f'[{datetime.now()}][{thread_id}][{id}]',flush=True, *args, **kwargs)

def debug(*args, **kwargs):
    if is_logging_level_debug():
        log(*args, **kwargs)

def is_logging_level_debug():
    return config_utils.get_logging_level() == 'debug'
