import http.server
import ssl
import threading
import os
from api_tests.core.listeners.http_listeners.request_handler import RequestHandler

class HTTPListenerServer:
    stop_server_flag = False

    def start_listener(self,host,port):
        # Create an HTTP server object with custom request handler
        http_server = http.server.HTTPServer((host, port), RequestHandler)
        current_dir = os.path.dirname(os.path.abspath(__file__))
        listener_dir = os.path.join(current_dir, '..')
        core_dir = os.path.join(listener_dir, '..')
        api_test_dir = os.path.join(core_dir, '..')
        # Enable TLS (HTTPS) support
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        context.load_cert_chain(f'{api_test_dir}/certs/certificate.pem', f'{api_test_dir}/certs/private_key.pem')
        http_server.socket = context.wrap_socket(http_server.socket)

        # Start the server
        server_thread = threading.Thread(target=http_server.serve_forever)
        server_thread.start()
        print(f"============Test HTTP Listener started on host {host} and port {port}============")
        # Wait for the user to stop the server
        try:
            while not HTTPListenerServer.stop_server_flag:
                pass
        except KeyboardInterrupt:
            pass

        # Stop the server gracefully
        http_server.shutdown()
        http_server.server_close()

    def stop_listener(self):
        # Set the stop_server_flag to True to stop the server
        HTTPListenerServer.stop_server_flag = True

    def reset_stop_server_flag(self):
        # Set the stop_server_flag to True to stop the server
        HTTPListenerServer.stop_server_flag = False
