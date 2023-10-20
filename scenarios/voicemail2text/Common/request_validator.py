import v2ticlib.request_utils as request_utils

class RequestValidator():
    def validate(self, request:dict):
        if request_utils.does_not_have_audio(request):
            raise ValueError("Missing audio")
