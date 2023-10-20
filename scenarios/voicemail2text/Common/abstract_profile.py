import v2ticlib.constants.fields as Fields
import v2ticlib.string_utils as string_utils
from abc import ABC, abstractmethod
import typing

class AbstractProfile(ABC):
    def __init__(cls):
        cls._mandatory_headers = cls.get_mandatory_headers()

    def get_mandatory_headers(cls):
        return []

    def create_request_context(cls, initial_request_content:dict, headers: typing.Mapping[str, str]) -> dict[str, any]:
        cls._validate_mandatory_headers(headers)
        context = {
            Fields.SCRID: initial_request_content[Fields.SCRID],
            Fields.DEPOSIT_TIME: initial_request_content[Fields.DEPOSIT_TIME],
            Fields.HEADERS: headers
        }
        return cls.get_request_context(context, headers)

    def _validate_mandatory_headers(cls, headers: typing.Mapping[str, str]):
        missing_headers = []
        for header in cls._mandatory_headers:
            value = headers.get(header)
            if type(value) == list:
                if value is None or len(value) == 0:
                    missing_headers.append(header)
            elif string_utils.is_blank(value):
                    missing_headers.append(header)

        if len(missing_headers) > 0:
            raise Exception(f'The following mandatory headers are missing {missing_headers}')

    @abstractmethod
    def get_request_context(cls, context:dict[str, any], request_headers:typing.Mapping[str, str]) -> dict[str, any]:
        pass

    def get_response_context(cls, request: dict[str, any]) -> dict[str, any]:
        return request

    def update_response_hook(cls, request: dict[str, any], response: dict[str, any]) -> dict[str, any]:
        return response

    def get_after_deposit_ack_context(cls, request: dict[str, any]) -> dict[str, any]:
        return request

    def update_after_deposit_ack_hook(cls, request: dict[str, any], after_deposit_ack: dict[str, any]) -> dict[str, any]:
        return after_deposit_ack

    def get_smtp_return_code_context(cls, request: dict[str, any]) -> dict[str, any]:
        return request