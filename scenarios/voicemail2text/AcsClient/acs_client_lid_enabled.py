import azure.cognitiveservices.speech as speechsdk
from v2ticlib.logger_utils import log, debug, CONTEXT
import v2ticlib.constants.fields as Fields
from AcsClient.acs_client import AcsClient

class AcsClientLid(AcsClient):

    def get_lid_mode(self, request):
        return self.get_runtime_property(request, Fields.LID_MODE)

    def get_speech_config(self, request):
        speech_config = super().get_speech_config(request)
        lid_mode = self.get_lid_mode(request)
        speech_config.set_property(property_id=speechsdk.PropertyId.SpeechServiceConnection_LanguageIdMode, value=lid_mode)
        log(f"'lid_mode': {lid_mode}")
        return speech_config

    def do_do_transcribe(self, scrid, languages, audio_config, timeout, recognition_result, speech_config):
        recognition_result[Fields.DETECTED_LANGUAGES] = []
        self.recognize_multi_language(scrid, languages, audio_config, timeout, recognition_result, speech_config)
        recognition_result[Fields.DETECTED_LANGUAGES] = self.join_languages(recognition_result[Fields.DETECTED_LANGUAGES])

    def recognize_multi_language(self, scrid, languages, audio_config, timeout, recognition_result, speech_config):

        log(f'recognize_multi_language: {languages}')

        auto_detect_source_language_config = speechsdk.languageconfig.AutoDetectSourceLanguageConfig(languages=languages)

        speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config,
                                                       audio_config=audio_config,
                                                       auto_detect_source_language_config=auto_detect_source_language_config)

        def recognized_cb(evt: speechsdk.SpeechRecognitionEventArgs):
            CONTEXT.set(scrid)
            if evt.result.reason == speechsdk.ResultReason.RecognizedSpeech:
                debug(f'RECOGNIZED: {evt.result.properties}')
                if evt.result.properties.get(
                        speechsdk.PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult) is None:
                    log("Unable to detect any language")
                else:
                    detected_language = evt.result.properties[
                        speechsdk.PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult]

                    log(f'Detected language = {detected_language}')
                    detected_languages: list = recognition_result[Fields.DETECTED_LANGUAGES]
                    detected_languages.append(detected_language)

                    self.update_recognition_result(recognition_result, evt)

        self.do_recognize(scrid, speech_recognizer, recognized_cb, timeout)
