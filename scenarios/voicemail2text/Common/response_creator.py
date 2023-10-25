from v2ticlib.template_utils import template_utils
from v2ticlib.logger_utils import log
import v2ticlib.constants.fields as Fields
import v2ticlib.constants.constants as Constants
import v2ticlib.string_utils as string_utils
import v2ticlib.rtf_utils as rtf_utils
import v2ticlib.request_utils as request_utils

class ResponseCreator():

    def create(self, request):
        status = Constants.FAILED
        if request_utils.does_not_have_notes_from_request(request):
            self.update_recognition_result(request)
            status = Constants.SUCCESS
        request_utils.set_status(request, status)
        return template_utils.render_response(request)

    def get_conversion_status(self, recognition_result:dict, min_confidence_percentage:int):
        if string_utils.is_blank(recognition_result[Fields.TEXT]):
            log('blank recognition result text')
            return Constants.UNCONVERTIBLE

        confidence_percentage = recognition_result[Fields.GLOBAL_CONFIDENCE_SCORE]
        if confidence_percentage < min_confidence_percentage:
            log(f'global confidence score [{confidence_percentage}] is lower than min confidence percentage [{min_confidence_percentage}]')
            return Constants.UNCONVERTIBLE

        return Constants.TRANSCRIBED

    def update_recognition_result(self, request):
        recognition_result:dict = request_utils.get_recognition_result(request)

        truncated = Constants.FALSE
        if request_utils.is_audio_truncated(request):
            truncated = Constants.TRUE
        recognition_result[Fields.AUDIO_TRUNCATED] = truncated

        min_confidence_percentage = request_utils.get_min_confidence_percentage(request)
        recognition_result[Fields.CONVERSION_STATUS] = self.get_conversion_status(recognition_result, min_confidence_percentage)
        if recognition_result[Fields.CONVERSION_STATUS] == Constants.UNCONVERTIBLE:
            log(f'recognition was found as {Constants.UNCONVERTIBLE}')
            recognition_result[Fields.TEXT] = ''
            recognition_result[Fields.DISPLAY_TEXT] = ''
            recognition_result[Fields.ITN_TEXT] = ''
        else:
            if request_utils.is_truncate_lenghty_transcriptions_enabled(request):
                max_transcription_length = request_utils.get_max_transcription_length(request)
                self.truncate_text(recognition_result, max_transcription_length)

        deposit_time = request_utils.get_deposit_time(request)
        recognition_result[Fields.RTF] = rtf_utils.get_rtf(deposit_time, recognition_result[Fields.FINAL_AUDIO_LENGTH_SECS])
        log_transcriptions_enabled:bool = request_utils.is_log_transcriptions_enabled(request)
        self.log_recognition_result(recognition_result, log_transcriptions_enabled)
        request_utils.set_recognition_result(request, recognition_result)

    def truncate_text(self, recognition_result:dict, max_transcription_length:int):
        for field in Fields.TEXT, Fields.DISPLAY_TEXT, Fields.ITN_TEXT:
            self.do_truncate_text(recognition_result, field, max_transcription_length)

    def do_truncate_text(self, recognition_result:dict, field:str, max_transcription_length:int):
        transcription_text = recognition_result[field]
        if(len(transcription_text) > max_transcription_length):
            recognition_result[field] = transcription_text[:max_transcription_length]

    def log_recognition_result(self, recognition_result:dict, log_transcriptions_enabled=False):
        recognition_result_to_log = recognition_result
        if(log_transcriptions_enabled == False):
            recognition_result_to_log = {key:('**not logging private content**' if key in [Fields.TEXT, Fields.ITN_TEXT, Fields.DISPLAY_TEXT] else val)
                                         for (key, val) in recognition_result.copy().items()}
        log(f'Recognition Results: {recognition_result_to_log}')