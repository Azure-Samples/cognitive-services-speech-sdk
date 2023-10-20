import typing
import v2ticlib.constants.fields as Fields
import v2ticlib.constants.headers as Headers
import v2ticlib.constants.constants as Constants
from Common.abstract_profile import AbstractProfile
import v2ticlib.request_utils as request_utils

class SampleSmtpProfile(AbstractProfile):

    def get_mandatory_headers(cls):
        return [Headers.X_REFERENCE, Headers.TO]

    def get_request_context(cls, context:dict[str, any], headers:typing.Mapping[str, str]) -> dict[str, any]:
        return context

    def update_response_hook(cls, request: dict[str, any], response: dict[str, any]) -> dict[str, any]:
        status = request[Fields.STATUS]
        if status == Constants.SUCCESS:
            recognition_result = request[Fields.RECOGNITION_RESULT]
            if recognition_result[Fields.CONVERSION_STATUS] == Constants.TRANSCRIBED and recognition_result[Fields.LID_ENABLED]:
                response[Fields.HEADERS][Headers.X_DETECTED_LANGUAGE_CONSTELLATION] = recognition_result[Fields.DETECTED_LANGUAGES]

        return response