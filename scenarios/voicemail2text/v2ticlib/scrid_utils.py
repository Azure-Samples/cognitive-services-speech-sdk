from threading import Lock
from datetime import datetime
import v2ticlib.host_utils as host_utils

lock = Lock()
counter = 0
hostname = host_utils.get_hostname().replace("-", "")

def get_counter():
    global counter
    with lock:
        counter += 1
        return counter

def generate_scrid():
    timestamp = datetime.now().strftime("%Y%m%d%H%M%S")
    return f'{timestamp}-{hostname}-{host_utils.get_pid()}-{get_counter()}'