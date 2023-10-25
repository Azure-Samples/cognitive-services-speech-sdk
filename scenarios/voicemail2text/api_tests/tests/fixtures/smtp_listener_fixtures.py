from aiosmtpd.controller import Controller
from api_tests.core.listeners.smtp_listeners.smtp_listener import SimpleSMTPRequestHandler
from api_tests.core.socket_utils import get_machine_ip
from api_tests.core.config_utils import get_config_value
from api_tests.app.local_setup.setup_local_smtp_server import start_smtp_server_local_using_thread,stop_smtp_local_server,reset_smtp_server_thread
import threading
import pytest
import time
from api_tests.core.reporter import reporter

@pytest.fixture(scope="module", autouse=True)
def smtp_listener_fixture(request):
    setup_local_server =  request.config.getoption("--setup_local_server")

    # Start local smtp server if setup_local_server is true
    if setup_local_server.lower() == "true":
        reporter.report(f"\noption --setup_local_server is {setup_local_server}, starting local smtp server")
        server_thread = threading.Thread(target=start_smtp_server_local_using_thread)
        print("============LOCAL SMTP SERVER STARTING=================")
        server_thread.start()
        time.sleep(5)

    # Create SMTP Listener client
    handler = SimpleSMTPRequestHandler()
    #host = get_machine_ip()
    host = get_config_value('smtptest', 'smtp_listener_host_local')
    port = get_config_value('smtptest', 'smtp_listener_port')
    controller = Controller(handler, hostname=host, port=port)
    print(f"============SMTP TEST LISTENER STARTING on host {host} and port {port}=================")
    controller.start()
    yield handler

    # Stop SMTP server at the end
    if setup_local_server.lower() == "true":
        print("============LOCAL SMTP SERVER STOPPING=================")
        stop_smtp_local_server()
        time.sleep(5)
        reset_smtp_server_thread()

    # Stop SMTP Listener server at the end
    print("============SMTP TEST LISTENER STOPPING=================")
    controller.stop()


def get_smtp_default_headers():
    headers = {
        "Message-Id": "Nuance V2T Conversion",
        "X-Reference": "20230519004356-epsvaibhav-4130-91405",
        "Reply-To": "replyto@nuance.com",
        "callingNumberWithheld": "False",
        "Subject": "Test email",
        "Delivered-To": "193967777",
        "From": "11111111",
        "respondWithAudio": "False",
        "Content-Transfer-Encoding": "base64"
    }

    return headers
