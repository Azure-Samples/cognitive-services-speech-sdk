import typing
from Common.abstract_profile import AbstractProfile
import v2ticlib.constants.headers as Headers

class SampleHttpsProfile(AbstractProfile):

    def get_mandatory_headers(cls):
        return [Headers.X_REFERENCE, Headers.X_RETURN_URL]

    def get_request_context(cls, context:dict[str, any], headers: typing.Mapping[str, str]) -> dict[str, any]:
        return context
