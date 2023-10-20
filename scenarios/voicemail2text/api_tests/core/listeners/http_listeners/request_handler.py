import http.server


class RequestHandler(http.server.BaseHTTPRequestHandler):
    scrid_dict = {}

    def do_POST(self):
        # Read the POST request body
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        scrid = self.headers['X-Reference']
        print(f"Response Body from IC: {post_data}")
        scrid_dict_entry = {
            'headers': dict(self.headers),
            'body': post_data
        }
        RequestHandler.scrid_dict[scrid] = scrid_dict_entry

        # Send a response
        self.send_response(200)
        self.send_header('Content-type', 'text/plain')
        self.end_headers()
        self.wfile.write(b'Success')

    def do_GET(self):
        # Check if the request path matches the GET endpoint to send the received response
        if self.path.startswith('/response/scrid/'):
            scrid = self.path.split('/')[-1]
            if scrid in RequestHandler.scrid_dict:
                # Retrieve the corresponding response from the scrid_dict
                response = RequestHandler.scrid_dict[scrid]
                print(f"Response Details saved: {RequestHandler.scrid_dict[scrid]}")
                print(f"Response body received: {response['body']}")
                # Send a success response
                self.send_response(200)
                for header, value in response['headers'].items():
                    self.send_header(header, value)
                self.end_headers()
                self.wfile.write(response['body'])

                # Remove entry from dict after GET request from test to reduce size
                del RequestHandler.scrid_dict[scrid]
            else:
                # No received response yet, send a 404 error response
                self.send_error(404, "No received response yet")
        else:
            # Endpoint not found, send a 404 error response
            self.send_error(404, "Endpoint not found")