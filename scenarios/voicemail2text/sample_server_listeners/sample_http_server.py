from http.server import BaseHTTPRequestHandler, HTTPServer
import os, socket
from http import HTTPStatus

class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(HTTPStatus.OK)
        self.end_headers()
        response_text = 'Hello, GET request received!'
        self.wfile.write(response_text.encode())

    def do_POST(self):
        self.print_response()

    def do_PUT(self):
        self.print_response()

    def print_response(self):
        try:
            if self.path == '/response':
                print("-----------------------")
                print("Received Response:")
                content_length = int(self.headers['Content-Length'])
                self.print_response_headers()
                post_data = self.rfile.read(content_length)
                charset = self.headers.get_content_charset(failobj="utf-8")
                response_str = post_data.decode(encoding=charset)
                print(f'Message Content:{os.linesep}{response_str}')
        except Exception as ex:
            print(f'Exception occurred while handling response:{str(ex)}')
        finally:
            self.send_response(HTTPStatus.OK)
            self.end_headers()

    def print_response_headers(self):
        for header in self.headers.items():
            header_name = header[0]
            header_value = header[1]
            print(f'{header_name}: {header_value}')

def run_http():
    hostname = socket.gethostname()
    host = socket.gethostbyname(hostname)
    port = 8443
    server_address = (host, port)
    httpd = HTTPServer(server_address, SimpleHTTPRequestHandler)
    print(f"HTTP Server listening for responses on http://{host}:{port}/response")
    httpd.serve_forever()

if __name__ == '__main__':
    run_http()
