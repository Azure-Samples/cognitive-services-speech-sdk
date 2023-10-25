import socket

def get_machine_ip():
    try:
        # Create a socket object
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Connect to a remote server (in this case, Google's public DNS server)
        sock.connect(("8.8.8.8", 80))

        # Get the local IP address of the machine
        machine_ip = sock.getsockname()[0]

        # Close the socket
        sock.close()

        return machine_ip
    except socket.error as e:
        print(f"Error: {e}")
        return None


def get_available_port():
    # Create a socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Bind the socket to any available port (port 0)
        sock.bind(('localhost', 0))

        # Get the port number assigned by the operating system
        _, port = sock.getsockname()

        return port
    finally:
        # Close the socket
        sock.close()