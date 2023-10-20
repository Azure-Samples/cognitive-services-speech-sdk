import os
import asyncio
import ssl
from email.mime.text import MIMEText
from aiosmtplib import SMTP

def create_tls_context():
     tls_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
     tls_context.check_hostname = False
     tls_context.verify_mode = ssl.CERT_NONE
     tls_context.minimum_version = ssl.TLSVersion.TLSv1_2
     return tls_context

async def send_email_with_audio_attachment():
    # Email configuration
    sender_email = 'sender@example.com'
    receiver_email = 'receiver@example.com'
    subject = 'Test Email'

    smtp_server = '127.0.0.1'
    smtp_port = 9443

    #Start adding headers
    headers = {
        "Message-Id": "20230519004356-epsvaibhav-4130-91405",
        "X-Reference": "20230519004356-epsvaibhav-4130-91405",
        "Reply-To": "replyto@nuance.com",
        "callingNumberWithheld": "False",
        "X-Language": "en-US",
        "Subject": "Test email",
        "Delivered-To": "193967777",
        "From": "11111111",
        "respondWithAudio": "False",
        "Content-Transfer-Encoding": "base64",
        "To": ", ".join([receiver_email])
    }
    content = None
    # Attach audio file
    with open("etc/audio/sample-audio-en-US.txt", "rb") as f:
        content = f.read()

    tls_context = create_tls_context()
    # Send the email
    smtp = SMTP(hostname=smtp_server, port=smtp_port, tls_context=tls_context, start_tls=False)
    await smtp.connect()
    print('smtp email server connected successfully :'+ smtp.hostname)
    message = MIMEText("email")
    message['From'] = sender_email
    message['To'] = receiver_email
    message['Subject'] = subject
    for k,v in headers.items():
          message.add_header(k, v)

    message.set_payload(content)

    response_tuple = await smtp.send_message(message)
    print(f'return code: {response_tuple[1]}')
    await smtp.quit()

# Run the email sending coroutine
if os.name == 'nt':
    asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())

asyncio.run(send_email_with_audio_attachment())

print("Done")