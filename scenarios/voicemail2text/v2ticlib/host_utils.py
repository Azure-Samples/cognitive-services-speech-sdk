import socket
import os

hostname = socket.gethostname()
ip_address = socket.gethostbyname(hostname)
pid = os.getpid()

def get_hostname():
    return hostname

def get_ip_address():
    return ip_address

def get_pid():
    return pid
