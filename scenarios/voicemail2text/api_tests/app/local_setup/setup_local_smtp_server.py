import sys, os
import os
import subprocess
import threading
sys.path.append(os.path.join(os.path.abspath(os.curdir)))
from api_tests.core.file_utils import comment_file_content,uncomment_file_content
from api_tests.core.reporter import reporter

# Define a threading event to signal the HTTP server thread to stop
stop_server_flag = False
http_server_process = None
current_dir = os.path.dirname(os.path.abspath(__file__))
config_dir_path = os.path.join(current_dir, '..','..','..')
config_file_path = os.path.join(config_dir_path, 'etc' ,'config', 'config.properties')
project_base_dir = os.path.join(current_dir, '..','..','..')

def start_smtp_local_server():
    global http_server_process,config_file_path,project_base_dir

    #Uncomment SMTP profile section and comment HTTPS profile section in config.properties file
    comment_file_content(config_file_path,"folder=etc/profiles/sample_https")
    comment_file_content(config_file_path,"profile=etc.profiles.sample_https.profile.SampleHttpsProfile")

    uncomment_file_content(config_file_path,"folder=etc/profiles/sample_smtp")
    uncomment_file_content(config_file_path,"profile=etc.profiles.sample_smtp.profile.SampleSmtpProfile")

    os.chdir(os.path.abspath(project_base_dir))
    http_server_process = subprocess.Popen(["python", "./servers/smtp_server.py"])


def reset_smtp_server_thread():
    # Set the stop_event to signal the HTTP server thread to stop
    global stop_server_flag
    stop_server_flag = False

def stop_smtp_local_server():
    # Set the stop_event to signal the HTTP server thread to stop
    global stop_server_flag,config_file_path
    stop_server_flag = True

    #Comment SMTP profile section and uncomment HTTPS profile section in config.properties file
    uncomment_file_content(config_file_path,"folder=etc/profiles/sample_https")
    uncomment_file_content(config_file_path,"profile=etc.profiles.sample_https.profile.SampleHttpsProfile")

    comment_file_content(config_file_path,"folder=etc/profiles/sample_smtp")
    comment_file_content(config_file_path,"profile=etc.profiles.sample_smtp.profile.SampleSmtpProfile")


def start_smtp_server_local_using_thread():
    global stop_server_flag,http_server_process

    http_server_thread = threading.Thread(target=start_smtp_local_server)
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
