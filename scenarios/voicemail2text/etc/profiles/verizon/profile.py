from email.utils import formatdate
import quopri
import typing
from Common.abstract_profile import AbstractProfile
import v2ticlib.string_utils as string_utils
import v2ticlib.constants.fields as Fields
import v2ticlib.constants.headers as Headers
import v2ticlib.constants.constants as Constants
import etc.profiles.verizon.constants.headers as Verizon_Headers
import etc.profiles.verizon.constants.fields as Verizon_Fields
import etc.profiles.verizon.constants.constants as Verizon_Constants
import ast

class VerizonProfile(AbstractProfile):

    def get_mandatory_headers(cls):
        return [Headers.REPLY_TO, Headers.FROM, Headers.TO,
                Headers.DATE, Headers.SUBJECT, Headers.MESSAGE_ID,
                Headers.DELIVERED_TO, Verizon_Headers.X_CNS_INTERFACE_VERSION,
                Verizon_Headers.X_CNS_VOICE_WRITER, Verizon_Headers.X_CNS_LANGUAGE]

    def get_request_context(cls, context:dict[str, any], headers:typing.Mapping[str, str]) -> dict[str, any]:

        x_cns_language = headers.get(Verizon_Headers.X_CNS_LANGUAGE)

        # If value does not match then map it to the default value as en-US
        context[Fields.LANGUAGE] = cls._map_language(x_cns_language, 'en-US')
        # If value does not match then map it to the default value as blank
        context[Verizon_Fields.X_CNS_LANGUAGE] = cls._map_language(x_cns_language, default='')
        return context

    def get_rfc2822_time(cls):
        return formatdate()

    def get_after_deposit_ack_context(cls, request: dict[str, any]) -> dict[str, any]:
        request[Fields.PASS_THROUGH_DATA][Verizon_Fields.X_NOTIFICATION_SND_TIME] = cls.get_rfc2822_time()
        return request

    def update_after_deposit_ack_hook(cls, request: dict[str, any], after_deposit_ack: dict[str, any]) -> dict[str, any]:
        return after_deposit_ack

    def get_response_context(cls, request: dict[str, any]) -> dict[str, any]:
        request[Fields.PASS_THROUGH_DATA][Verizon_Fields.X_ERROR_CODE] = cls._get_error_code(request)
        request[Fields.PASS_THROUGH_DATA][Verizon_Fields.X_RESPONSE_SND_TIME] = cls.get_rfc2822_time()
        return request

    def update_response_hook(cls, request: dict[str, any], response: dict[str, any]) -> dict[str, any]:
        if string_utils.is_blank(request[Fields.HEADERS][Verizon_Headers.X_CNS_VOICE_WRITER]):
            response[Fields.HEADERS][Verizon_Headers.X_TRANS_TEXT] = 'An error has occurred'
            response[Fields.HEADERS][Verizon_Headers.X_ERROR_CODE] = Verizon_Constants.ERR_HACK

        mail_from = response[Fields.MAIL_FROM]
        response[Fields.MAIL_FROM] = cls.format_mail_from(mail_from)
        return response

    def format_mail_from(cls, mail_from):
        eval_mail_from = ast.literal_eval(mail_from)
        if type(eval_mail_from) == list:
            if len(eval_mail_from) >= 1:
                return eval_mail_from[0]
        return mail_from

    def _get_error_code(cls, request):
        '''
        X-Error-Code shall contain an indication of whether the voice message was
        successfully converted into text, or the reasons for failure:
        • Err-Succ - if the voice message was successfully converted into text
        • Err-Trans - if the voice message was unconvertible due to the voice message being in an unsupported language or of a poor audio content
        • Err-Inaudible - if there was no meaningful audio to convert into text
        • Err-Hack - if the X-CNS-Voice-Writer header from the inbound message was missing or did not contain a value of  'true'
        • Err-NoSrv - if the Nuance platform was unable to process the voice messag
        '''
        if request[Fields.STATUS] == Constants.SUCCESS:
            recognition_result = request[Fields.RECOGNITION_RESULT]
            transcription_text = recognition_result[Fields.TEXT]
            conversion_status = recognition_result[Fields.CONVERSION_STATUS]
            error_code = Verizon_Constants.ERR_SUCC
            if(Verizon_Constants.ERROR_MESSAGE == transcription_text):
                error_code = Verizon_Constants.ERR_HACK
            elif (Constants.UNCONVERTIBLE == conversion_status):
                error_code = Verizon_Constants.ERR_TRANS
            elif (string_utils.is_blank(transcription_text)):
                error_code = Verizon_Constants.ERR_NOSRV
        else:
            error_code = Verizon_Constants.ERR_NOSRV

        return error_code


    def _map_language(cls, language:str, default:str = ''):
        if string_utils.is_blank(language):
            return default

        language_lower_case = language.lower()
        language_mapping = {
            "en-us": "en-US",
            "es-us": "es-US",
            "es-mx": "es-MX"
        }
        return language_mapping.get(language_lower_case, default)