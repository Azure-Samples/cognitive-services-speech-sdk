import sys, os
sys.path.append(os.path.join(os.path.abspath(os.curdir)))
import os
import pytest
import subprocess
import traceback
import threading
import time
from api_tests.core.reporter import reporter

# Define a threading event to signal the HTTP server thread to stop
stop_server_flag = False
http_server_process = None

def start_http_local_server():
    global http_server_process
    current_dir = os.path.dirname(os.path.abspath(__file__))
    project_base_dir = os.path.join(current_dir, '..','..','..')
    os.chdir(os.path.abspath(project_base_dir))
    http_server_process = subprocess.Popen(["python", "./servers/https_server.py"])


def stop_http_local_server():
    # Set the stop_event to signal the HTTP server thread to stop
    global stop_server_flag
    stop_server_flag = True

def reset_http_server_thread():
    # Set the stop_event to signal the HTTP server thread to stop
    global stop_server_flag
    stop_server_flag = False

def start_http_server_local_using_thread():
    global stop_server_flag,http_server_process

    http_server_thread = threading.Thread(target=start_http_local_server)
    http_server_thread.start()

    try:
        while not stop_server_flag:
            pass
    except KeyboardInterrupt:
            pass

    http_server_thread.join()
    if http_server_process:
        http_server_process.terminate()
        http_server_process.wait()
        http_server_process = None
