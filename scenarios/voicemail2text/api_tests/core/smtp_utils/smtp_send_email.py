import ssl
from aiosmtplib import SMTP
from api_tests.core.config_utils import get_config_value


def create_tls_context():
    tls_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    tls_context.verify_mode = ssl.CERT_OPTIONAL
    tls_context.check_hostname = False
    tls_context.minimum_version = ssl.TLSVersion.TLSv1_2
    return tls_context


async def send_email(message):
    smtp_server = get_config_value('smtptest', 'smtp_server_url')
    smtp_port = get_config_value('smtptest', 'smtp_server_port')
    tls_context = create_tls_context()
    smtp = SMTP(hostname=smtp_server, port=smtp_port, tls_context=tls_context, start_tls=False)
    await smtp.connect()
    response = await smtp.send_message(message)
    await smtp.quit()

    print(f"Acknowledgement after sending mail: {response}")

    return smtp.hostname,smtp.port,response
