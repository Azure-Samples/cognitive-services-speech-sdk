import ssl
import asyncio
from http import HTTPStatus
from aiohttp import ClientSession as ClientSession

request_url='https://127.0.0.1:9443/transcribe'
response_url='https://127.0.0.1:9443/response'

def create_headers():
    headers = {
        "X-Reference": "1234",
        "Connection": "close",
        "Content-Type": "audio/wav",
        "X-Return-URL": response_url,
        "Content-Encoding": "base64"
    }
    return headers

def read_audio_file(audio_file_path: str) -> bytes:
    with open(audio_file_path, "rb") as audio_file:
        audio_data = audio_file.read()
        return audio_data

async def create_tls_context():
    tls_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    tls_context.verify_mode = ssl.CERT_NONE
    tls_context.check_hostname = False
    tls_context.minimum_version = ssl.TLSVersion.TLSv1_2
    return tls_context

async def post(url, headers, data, verify_ssl=False) -> str:
    async with ClientSession() as session:
        async with session.post(url, data=data, headers=headers, verify_ssl=verify_ssl) as response:
            print(f'Got [{response.status}] from {url}')
            if response.status not in [HTTPStatus.OK, HTTPStatus.ACCEPTED]:
                raise Exception
            return response

async def send_aio_https_post_request():
    audio_data = read_audio_file("etc/audio/sample-audio-en-US-short.txt")
    headers = create_headers()
    try:
        res = await post(request_url, headers, audio_data, verify_ssl=False)
        print(f'Response Status [{res.status}]')
        print(f'Response Header received SCRID Successfully [{res.headers["Location"]}]')
    except Exception as e:
        print("Exception received while sending a aio https request"+str(e))

if __name__ == "__main__":
    asyncio.run(send_aio_https_post_request())
print("Done")