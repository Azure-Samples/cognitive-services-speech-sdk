import os, socket
import asyncio
from aiosmtpd.controller import Controller

class SimpleSMTPRequestHandler():

    async def handle_DATA(self, server, session, envelope):
        try:
            print("Received message:")
            print(f'From: {envelope.mail_from}')
            print(f'To: {envelope.rcpt_tos}')
            print("-----------------------")
            content = envelope.content.decode()
            print(f'Message data:{os.linesep}{content}')
        except Exception as ex:
            print(f'Exception occurred while handling response:{ex}')

        return "250 OK"

async def run_smtp_server():
    hostname = socket.gethostname()
    host = socket.gethostbyname(hostname)
    port = 9025

    handler = SimpleSMTPRequestHandler()
    controller = Controller(handler, hostname=host, port=port)
    controller.start()
    print(f"SMTP Server Listening On {host}:{port}")

    # Keep the server running
    while True:
        await asyncio.sleep(1)

if __name__ == "__main__":
    asyncio.run(run_smtp_server())