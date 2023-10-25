from email.mime.text import MIMEText
from api_tests.core.smtp_utils.smtp_send_email import send_email


async def send_email_with_audio(sender_email,receiver_email,subject,headers,audio_language,content):
    message = MIMEText("email")
    message['From'] = sender_email
    message['To'] = receiver_email
    message['Subject'] = subject
    message['X-Language'] = audio_language
    for k, v in headers.items():
        message.add_header(k, v)

    message.set_payload(content)
    smtp_host, smtp_port, ack_response = await send_email(message)
    return f"Email sent to client on smtp host: {smtp_host}, port: {smtp_port} and ack: {ack_response}"
