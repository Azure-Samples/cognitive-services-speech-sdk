import pytest
from api_tests.app.http_utils.http_listener_client import HTTPListenerClient
from api_tests.app.local_setup.setup_local_http_server import start_http_server_local_using_thread,stop_http_local_server,reset_http_server_thread
import time
import threading
from api_tests.core.reporter import reporter


@pytest.fixture(scope="module")
def http_listener_fixture(request):
    setup_local_server =  request.config.getoption("--setup_local_server")

    # Start local http server if setup_local_server is true
    if setup_local_server.lower() == "true":
        reporter.report(f"\noption --setup_local_server is {setup_local_server}, starting local http server")
        server_thread = threading.Thread(target=start_http_server_local_using_thread)
        print("============LOCAL HTTP SERVER STARTING=================")
        server_thread.start()
        time.sleep(5)


    # Create HTTP Listener client
    print("============HTTP TEST LISTENER STARTING=================")
    http_listener_client = HTTPListenerClient()
    # Start HTTP Listener server at the start
    http_listener_client.start()

    yield

    # Stop local http server if setup_local_server is true
    if setup_local_server.lower() == "true":
        print("============LOCAL HTTP SERVER STOPPING=================")
        stop_http_local_server()
        time.sleep(5)
        reset_http_server_thread()

    # Stop HTTP Listener server at the end
    print("============HTTP TEST LISTENER STOPPING=================")
    http_listener_client.stop()