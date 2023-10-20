import typing
from base64 import b64decode
from Common.request_validator import RequestValidator
from v2ticlib.template_utils import template_utils
from v2ticlib.constants.headers import CONTENT_ENCODING, CONTENT_TRANSFER_TYPE
from v2ticlib.constants.constants import BASE64
import v2ticlib.language_configuration_utils as language_configuration_utils
import v2ticlib.request_utils as request_utils

class RequestInjestor():
    def __init__(self):
        self._request_validator = RequestValidator()

    def injest(self, initial_request_content:dict, headers: typing.Mapping[str, str], body: bytes):
        request = template_utils.render_request(initial_request_content, headers)

        language_configuration_utils.resolve(request)
        self.resolve_audio(request, headers, body)
        self._request_validator.validate(request)
        return request

    def resolve_audio(self, request:dict, headers: typing.Mapping[str, str], body: bytes):
        audio = self.get_audio_bytes(headers, body)
        request_utils.set_audio(request, audio)

    def get_audio_bytes(self, headers: typing.Mapping[str, str], body: bytes):
        content_encoding = self.get_audio_encoding(headers)
        audio_bytes = body

        if content_encoding == BASE64:
            audio_bytes = self.get_base64_encoded_audio_bytes(body)

        return audio_bytes

    def get_audio_encoding(self, headers: typing.Mapping[str, str]):
        content_encoding = headers.get(CONTENT_ENCODING)
        if content_encoding is not None:
            return content_encoding

        content_encoding = headers.get(CONTENT_TRANSFER_TYPE)
        if content_encoding is not None:
            return content_encoding

        # assume base64 encoded
        return BASE64

    def get_base64_encoded_audio_bytes(self, body: bytes):
        return b64decode(body)