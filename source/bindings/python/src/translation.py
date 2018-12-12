#  Copyright (c) Microsoft. All rights reserved.
#  See https://aka.ms/csspeech/license201809 for the full license information.

from . import speech_py_impl as impl
from .speech import SpeechConfig, Recognizer, AudioConfig

from typing import Optional, Iterable, Tuple
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
        self._impl = self._get_impl(impl.SpeechTranslationConfig, subscription, region, endpoint, auth_token,
                speech_recognition_language)

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


class TranslationRecognizer(Recognizer):
    """
    Performs translation on the speech input.

    :param translation_config: The config for the translation recognizer.
    :param audio_config: The config for the audio input.
    """
    def __init__(self, translation_config: SpeechTranslationConfig, audio_config: Optional[AudioConfig] = None):
        if not isinstance(translation_config, SpeechTranslationConfig):
            raise ValueError('translation_config must be a SpeechTranslationConfig instance')

        self._impl = self._get_impl(impl.TranslationRecognizer, translation_config, audio_config)

    @property
    def synthesizing(self):
        """
        The event signals that a translation synthesis result is received.

        Callbacks connected to this signal are called with a :class:`.TranslationSynthesisEventArgs` instance as the
        single argument.

        :type: TranslationSynthesisEventSignal
        """
        return self._impl.synthesizing

