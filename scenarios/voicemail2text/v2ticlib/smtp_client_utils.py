from email.mime.text import MIMEText
from aiosmtplib import SMTP
import ssl
from v2ticlib import config_utils
import v2ticlib.constants.protocols as Protocols
from v2ticlib.constants.fields import EMAIL
from v2ticlib.constants.headers import FROM, TO
from v2ticlib.logger_utils import log

def create_tls_context_no_verify():

     cert_file = config_utils.get_property(Protocols.SMTP, 'client_cert_file')

     tls_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
     tls_context.verify_mode = ssl.CERT_OPTIONAL
     tls_context.check_hostname = False
     tls_context.load_verify_locations(cafile=cert_file)
     tls_context.minimum_version = ssl.TLSVersion.TLSv1_2
     return tls_context

def create_tls_context_verify():
     tls_context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
     tls_context.minimum_version = ssl.TLSVersion.TLSv1_2
     return

def create_tls_context(start_tls:bool):
    tls_context = None
    if start_tls:
        tls_context = create_tls_context_verify()
    else:
        tls_context = create_tls_context_no_verify()
    return tls_context

async def send(smtp_sender_host, smtp_sender_port, mail_from, rcpt_to, headers, content, start_tls=False) -> str:
     message = MIMEText(EMAIL)
     message[FROM] = mail_from
     message[TO] = rcpt_to
     for k,v in headers.items():
          message.add_header(k, v)

     if content is not None:
          message.set_payload(content)

     tls_context = create_tls_context(start_tls)

     smtp_client = SMTP(hostname=smtp_sender_host, port=smtp_sender_port, tls_context=tls_context, start_tls=start_tls)
     async with smtp_client:
          response_tuple = await smtp_client.send_message(message)
          return_code = response_tuple[1]
          log(f'return code: {return_code}')
          return return_code
