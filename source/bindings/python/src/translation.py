# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.
"""
Classes related to translation of speech to other languages.
"""

from . import speech_py_impl as impl
from .speech import (SpeechConfig, Recognizer, AudioConfig, ResultFuture, RecognitionEventArgs,
        RecognitionResult, EventSignal, SessionEventArgs)

from typing import Optional, Iterable, Tuple, Dict
OptionalStr = Optional[str]


class SpeechTranslationConfig(SpeechConfig):
    """
    Class that defines configurations for translation with speech input.

    The configuration can be initialized in different ways:

    - from subscription: pass a subscription key and a region
    - from endpoint: pass a subscription key and an endpoint
    - from authorization token: pass an authorization token and a region

    :param subscription: The subscription key.
    :param region: The region name (see the `region page <https://aka.ms/csspeech/region>`_).
    :param endpoint: The service endpoint to connect to.
    :param auth_token: The authorization token.
    :param speech_recognition_language: The input language to the speech recognition. The language
        is specified in BCP-47 format.
    :param target_languages: The target languages for translation.
    :param voice_name: The voice to use for synthesized output.
    """

    def __init__(self, subscription: OptionalStr = None, region: OptionalStr = None,
                 endpoint: OptionalStr = None, auth_token: OptionalStr = None,
                 target_languages: Iterable[str] = None, voice_name: str = '',
                 speech_recognition_language: str = ''):
        self._impl = self._get_impl(impl.SpeechTranslationConfig, subscription, region, endpoint,
                auth_token, speech_recognition_language)

        if target_languages:
            for lang in target_languages:
                self.add_target_language(lang)

        self.voice_name = voice_name

    @property
    def voice_name(self) -> str:
        """
        The voice to use for synthesized output.
        """
        return impl._speech_py_impl.SpeechTranslationConfig_get_voice_name(self._impl)

    @voice_name.setter
    def voice_name(self, voice_name):
        return impl._speech_py_impl.SpeechTranslationConfig_set_voice_name(self._impl, voice_name)

    @property
    def target_languages(self) -> Tuple[str]:
        """
        The target languages for translation.
        """
        return impl._speech_py_impl.SpeechTranslationConfig_get_target_languages(self._impl)

    def add_target_language(self, language: str):
        """
        Add `language` to the list of target languages for translation.

        :param language: The language code to add.
        """
        impl._speech_py_impl.SpeechTranslationConfig_add_target_language(self._impl, language)


class TranslationRecognitionEventArgs(RecognitionEventArgs):
    """
    Defines payload that is sent with the event Recognizing or Recognized.
    """

    def __init__(self, evt_args):
        """
        Constructor for internal use.
        """
        super().__init__(evt_args)
        self._result = TranslationRecognitionResult(evt_args.result)

    @property
    def result(self) -> "TranslationRecognitionResult":
        """
        Contains the translation recognition result.
        """
        return self._result

    def __str__(self):
        return u'{}(session_id={}, result={})'.format(type(self).__name__, self.session_id, self.result)


class TranslationRecognitionCanceledEventArgs(TranslationRecognitionEventArgs):
    """
    Class for translation recognition canceled event arguments.
    """

    def __init__(self, evt_args):
        """
        Constructor for internal use.
        """
        super().__init__(evt_args)
        self._cancellation_details = evt_args.cancellation_details

    @property
    def cancellation_details(self) -> "CancellationDetails":
        """
        The reason why recognition was cancelled.

        Returns `None` if there was no cancellation.
        """
        return self._cancellation_details


class TranslationSynthesisEventArgs(SessionEventArgs):
    """
    Defines payload that is sent with the event
    :py:attr:`azure.cognitiveservices.speech.translation.TranslationRecognizer.synthesizing`.
    """

    def __init__(self, evt_args):
        """
        Constructor for internal use.
        """
        super().__init__(evt_args)
        self._result = TranslationSynthesisResult(evt_args.result)

    @property
    def result(self) -> "TranslationSynthesisResult":
        """
        Contains the translation synthesis result.
        """
        return self._result

    def __str__(self):
        return '{}(session_id={}, result={})'.format(
            type(self).__name__, self.session_id, self.result)


class TranslationRecognitionResult(RecognitionResult):
    """
    Defines the translation text result.
    """

    def __init__(self, impl_result):
        """
        Constructor for internal use.
        """
        super().__init__(impl_result)

        self._translations = impl_result.translations

    @property
    def translations(self) -> Dict[str, str]:
        """
        Presents the translation results. Each item in the map is a key value pair, where key is
        the language tag of the translated text, and value is the translation text in that
        language.
        """
        return self._translations

    def __str__(self):
        return u'{}(result_id={}, translations={}, reason={})'.format(
                type(self).__name__, self.result_id, dict(self.translations), self.reason)


class TranslationSynthesisResult():
    """
    Defines the translation synthesis result, i.e. the voice output of the translated text in the
    target language.
    """

    def __init__(self, impl_result):
        """
        Constructor for internal use.
        """
        self._reason = impl_result.reason
        self._audio = impl_result.audio

    @property
    def audio(self) -> bytes:
        """
        The voice output of the translated text in the target language.
        """
        return self._audio

    @property
    def reason(self) -> "ResultReason":
        """
        Recognition reason.
        """
        return self._reason

    def __str__(self):
        return '{}(audio=<{} bytes of audio>, reason={})'.format(
            type(self).__name__, len(self.audio), self.reason)


class TranslationRecognizer(Recognizer):
    """
    Performs translation on the speech input.

    :param translation_config: The config for the translation recognizer.
    :param audio_config: The config for the audio input.
    """

    def __init__(self, translation_config: SpeechTranslationConfig,
                 audio_config: Optional[AudioConfig] = None):
        if not isinstance(translation_config, SpeechTranslationConfig):
            raise ValueError('translation_config must be a SpeechTranslationConfig instance')

        self._impl = self._get_impl(impl.TranslationRecognizer, translation_config, audio_config)

    def recognize_once(self) -> TranslationRecognitionResult:
        """
        Performs recognition in a blocking (synchronous) mode. Returns after a single utterance is
        recognized. The end of a single utterance is determined by listening for silence at the end
        or until a maximum of 15 seconds of audio is processed. The task returns the recognition
        text as result. For long-running multi-utterance recognition, use
        :py:meth:`.start_continuous_recognition_async` instead.

        :return: The result value of the synchronous recognition.
        """
        return TranslationRecognitionResult(self._impl.recognize_once())

    def recognize_once_async(self) -> ResultFuture:
        """
        Performs recognition in a non-blocking (asynchronous) mode. This will recognize a single
        utterance. The end of a single utterance is determined by listening for silence at the end
        or until a maximum of 15 seconds of audio is processed. For long-running multi-utterance
        recognition, use :py:meth:`.start_continuous_recognition_async` instead.

        :return: A future containing the result value of the asynchronous recognition.
        """
        return ResultFuture(self._impl.recognize_once_async(), TranslationRecognitionResult)

    @property
    def synthesizing(self) -> EventSignal:
        """
        The event signals that a translation synthesis result is received.

        Callbacks connected to this signal are called with a
        :class:`.TranslationSynthesisEventArgs` instance as the single argument.
        """
        return EventSignal(self._impl.synthesizing, TranslationSynthesisEventArgs)

    @property
    def recognizing(self) -> EventSignal:
        """
        Signal for events containing intermediate recognition results.

        Callbacks connected to this signal are called with a
        :class:`.TranslationRecognitionEventArgs`, instance as the single argument.
        """
        return EventSignal(self._impl.recognizing, TranslationRecognitionEventArgs)

    @property
    def recognized(self) -> EventSignal:
        """
        Signal for events containing final recognition results (indicating a successful
        recognition attempt).

        Callbacks connected to this signal are called with a
        :class:`.TranslationRecognitionEventArgs`, instance as the single argument, dependent on
        the type of recognizer.
        """
        return EventSignal(self._impl.recognized, TranslationRecognitionEventArgs)

    @property
    def canceled(self) -> EventSignal:
        """
        Signal for events containing canceled recognition results (indicating a recognition attempt
        that was canceled as a result or a direct cancellation request or, alternatively, a
        transport or protocol failure).

        Callbacks connected to this signal are called with a
        :class:`.TranslationRecognitionCanceledEventArgs`, instance as the single argument.
        """
        return EventSignal(self._impl.canceled, TranslationRecognitionCanceledEventArgs)

