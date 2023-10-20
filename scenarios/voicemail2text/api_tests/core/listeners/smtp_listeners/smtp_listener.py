import asyncio
import os
import time
from api_tests.core.config_utils import get_config_value
from api_tests.core.smtp_utils.smtp_email_parser import EmailResponseParser


class SimpleSMTPRequestHandler():
    def __init__(self):
        self.scrid_dict = {}

    async def handle_DATA(self, server, session, envelope):
        content = envelope.content.decode()
        received_email = EmailResponseParser(envelope.mail_from, envelope.rcpt_tos, f"{os.linesep}{content}")
        received_email.get_parse_email_text()
        scrid = received_email.email_headers['scrid'].strip()
        scrid_dict_entry = {
            'email_from': envelope.mail_from,
            'email_to': envelope.rcpt_tos,
            'email_content': f"{os.linesep}{content}"
        }

        self.scrid_dict[scrid] = scrid_dict_entry
        return "250 OK"

    async def get_received_email_details(self,scrid):
        timeout = int(get_config_value('common', 'fetch_scrid_from_listener_timeout'))
        start_time = time.time()

        while True:
            if scrid in self.scrid_dict.keys():
                return self.scrid_dict[scrid]
            elif time.time() - start_time >= timeout:
                raise TimeoutError("Timeout reached. Email response not received.")

            await asyncio.sleep(1)
