import threading
import allure
from api_tests.core.socket_utils import get_machine_ip, get_available_port
from api_tests.core.listeners.http_listeners.http_listeners_server import HTTPListenerServer
import time


class HTTPListenerClient:
    listener_host = get_machine_ip()
    listener_port = get_available_port()

    def __init__(self):
        self.http_listener_server = HTTPListenerServer()

    def start(self):
        with allure.step("Start HTTP Listener"):
            server_thread = threading.Thread(target=self.http_listener_server.start_listener,
                                             args=(HTTPListenerClient.listener_host, HTTPListenerClient.listener_port))
            server_thread.start()
            allure.attach(
                f"HTTP Listener is started on host {HTTPListenerClient.listener_host} and port {HTTPListenerClient.listener_port}",
                name="HTTP Listener Host & Port", attachment_type=allure.attachment_type.TEXT)

    def stop(self):
        with allure.step("Stop HTTP Listener"):
            self.http_listener_server.stop_listener()
            time.sleep(5)
            self.http_listener_server.reset_stop_server_flag()
