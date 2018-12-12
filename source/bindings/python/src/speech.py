#  Copyright (c) Microsoft. All rights reserved.
#  See https://aka.ms/csspeech/license201809 for the full license information.

from . import speech_py_impl as impl
from .audio import AudioConfig

from .speech_py_impl import (
    OutputFormat,
    PropertyCollection,
    PropertyId,
    SpeechRecognitionResult,
    TranslationRecognitionResult,
    IntentRecognitionResult,
)

from typing import Optional, Dict, Union
OptionalStr = Optional[str]


class SpeechConfig():
    """
    Class that defines configurations for speech or intent recognition.

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
    """
    def __init__(self, subscription: OptionalStr = None, region: OptionalStr = None,
            endpoint: OptionalStr = None, auth_token: OptionalStr = None,
            speech_recognition_language: str = ''):

        self._impl = self._get_impl(impl.SpeechConfig, subscription, region, endpoint, auth_token,
                speech_recognition_language)
        self._impl.set_property("SPEECHSDK-SPEECH-CONFIG-SYSTEM-LANGUAGE", "Python")

    def get_property(self, property_id: PropertyId) -> str:
        """
        Get a property by id.

        :param property_id: The id of the property to be retrieved.
        :return: The value of the property.
        """
        if not isinstance(property_id, PropertyId):
            raise TypeError('wrong type, must be PropertyId')
        return impl._speech_py_impl.SpeechConfig_get_property(self._impl, property_id.value)

    def set_property(self, property_id: PropertyId, value: str):
        """
        Set a property by id.

        :param property_id: The id of the property to be set.
        :param value: The value to be set for the property.
        """
        if not isinstance(property_id, PropertyId):
            raise TypeError('wrong type, must be PropertyId')
        return impl._speech_py_impl.SpeechConfig_set_property(self._impl, property_id.value, value)

    def set_properties(self, properties: Dict[PropertyId, str]):
        """
        Set multiple properties by id.

        :param properties: A dict mapping property ids to the values to be set.
        """
        for property_id, value in properties.items():
            if not isinstance(property_id, PropertyId):
                raise TypeError('wrong type, must be PropertyId')

            impl._speech_py_impl.SpeechConfig_set_property(self._impl, property_id.value, value)

    @staticmethod
    def _get_impl(config_type, subscription, region, endpoint, auth_token,
            speech_recognition_language):
        _impl = None
        if region is not None and subscription is not None:
            _impl = config_type._from_subscription(subscription, region)
        elif region is not None and auth_token is not None:
            _impl = config_type._from_authorization_token(auth_token, region)
        elif endpoint is not None and subscription is not None:
            _impl = config_type._from_endpoint(endpoint, subscription)

        if _impl is not None:
            _impl.set_speech_recognition_language(speech_recognition_language)
            return _impl

        if region is not None and subscription is None and auth_token is None:
            raise ValueError('either endpoint or subscription key must be given along with a region')

        if endpoint is not None and region is not None:
            raise ValueError('cannot construct SpeechConfig with both region and endpoint information')

        if subscription is not None and endpoint is None and region is None:
            raise ValueError('either endpoint or region must be given along with a subscription keyregion')

        raise ValueError('cannot construct SpeechConfig with the given arguments')

    @property
    def output_format(self) -> OutputFormat:
        """
        The output format for results.
        """
        return OutputFormat(impl._speech_py_impl.SpeechConfig_get_output_format(self._impl))

    @output_format.setter
    def output_format(self, format: OutputFormat):
        if not isinstance(format, OutputFormat):
            raise TypeError('wrong type, must be OutputFormat')
        return impl._speech_py_impl.SpeechConfig_set_output_format(self._impl, format.value)

    @property
    def speech_recognition_language(self) -> str:
        """
        The name of spoken language to be recognized in BCP-47 format.
        """
        return self._impl.get_speech_recognition_language()

    @speech_recognition_language.setter
    def speech_recognition_language(self, language: str):
        self._impl.set_speech_recognition_language(language)

    @property
    def endpoint_id(self) -> str:
        """
        The endpoint id.
        """
        return self._impl.get_endpoint_id()

    @endpoint_id.setter
    def endpoint_id(self, endpoint_id: str):
        self._impl.set_endpoint_id(endpoint_id)

    @property
    def authorization_token(self) -> str:
        """
        The authorization token to connect to the service.
        """
        return self._impl.get_authorization_token()

    @authorization_token.setter
    def authorization_token(self, authorization_token: str):
        return self._impl.set_authorization_token(authorization_token)

    @property
    def subscription_key(self) -> str:
        """
        The subscription key that was used to create the Recognizer.
        """
        return self._impl.get_subscription_key()

    @property
    def region(self) -> str:
        """
        The region key that was used to create the Recognizer.
        """
        return self._impl.get_region()

    def set_proxy(self, hostname: str, port: str, username: str, password: str):
        """
        Set proxy information.

        :param hostname: The host name of the proxy server.
        :param port: The port number of the proxy server.
        :param username: The user name of the proxy server.
        :param password: The password of the proxy server.
        """
        self._impl.set_proxy(hostname, port, username, password)


class Recognizer():
    """
    Base class for different recognizers
    """

    @staticmethod
    def _get_impl(reco_type, speech_config, audio_config):
        if audio_config is not None:
            _impl = reco_type._from_config(speech_config._impl, audio_config._impl)
        else:
            _impl = reco_type._from_config(speech_config._impl, None)

        return _impl

    @property
    def properties(self) -> PropertyCollection:
        """
        A collection of properties and their values defined for this Recognizer.
        """
        return self._impl.properties

    @property
    def endpoint_id(self) -> str:
        """
        The endpoint ID of a customized speech model that is used for recognition.
        """
        return self._impl.get_endpoint_id()

    @property
    def authorization_token(self) -> str:
        """
        The authorization token that will be used for connecting the server.
        """
        return self._impl.get_authorization_token()

    @authorization_token.setter
    def authorization_token(self, authorization_token: str):
        self._impl.set_authorization_token(authorization_token)

    def recognize_once(self) -> Union[SpeechRecognitionResult, TranslationRecognitionResult,
            IntentRecognitionResult]:
        """
        Performs recognition in a blocking (synchronous) mode.

        :return: The result value of the synchronous recognition.
        """
        return self._impl.recognize_once()

    # TODO add annotation for return value
    def recognize_once_async(self):
        """
        Performs recognition in a non-blocking (asynchronous) mode.

        :return: A future containing the result value of the asynchronous recognition.
        """
        return self._impl.recognize_once_async()

    # TODO add annotation for return value
    def start_continuous_recognition_async(self):
        """
        Asynchronously initiates continuous recognition operation.

        :return: A future that is fulfilled once recognition has been initialized.
        """
        return self._impl.start_continuous_recognition_async()

    def stop_continuous_recognition_async(self):
        """
        Asynchronously terminates ongoing continuous recognition operation.

        :return: A future that is fulfilled once recognition has been stopped.
        """
        return self._impl.stop_continuous_recognition_async()

    def start_continuous_recognition(self):
        """
        Synchronously initiates continuous recognition operation.
        """
        return self._impl.start_continuous_recognition()

    def stop_continuous_recognition(self):
        """
        Synchronously terminates ongoing continuous recognition operation.
        """
        return self._impl.stop_continuous_recognition()

    @property
    def session_started(self):
        """
        Signal for events indicating the start of a recognition session (operation).

        Callbacks connected to this signal are called with a :class:`.SessionEventArgs` instance as the
        single argument.

        :type: SessionEventSignal
        """
        return self._impl.session_started

    @property
    def session_stopped(self):
        """
        Signal for events indicating the end of a recognition session (operation).

        Callbacks connected to this signal are called with a :class:`.SessionEventArgs` instance as the
        single argument.

        :type: SessionEventSignal
        """
        return self._impl.session_stopped

    @property
    def speech_start_detected(self):
        """
        Signal for events indicating the start of speech.

        Callbacks connected to this signal are called with a :class:`.RecognitionEventArgs` instance as the
        single argument.

        :type: RecognitionEventSignal
        """
        return self._impl.speech_start_detected

    @property
    def speech_end_detected(self):
        """
        Signal for events indicating the end of speech.

        Callbacks connected to this signal are called with a :class:`.RecognitionEventArgs` instance as the
        single argument.

        :type: RecognitionEventSignal
        """
        return self._impl.speech_end_detected

    @property
    def recognizing(self):
        """
        Signal for events containing intermediate recognition results.

        Callbacks connected to this signal are called with a :class:`.SpeechRecognitionEventArgs`,
        :class:`.TranslationRecognitionEventArgs` or :class:`.IntentRecognitionEventArgs` instance as the
        single argument, dependent on the type of recognizer.

        :type: SpeechRecognitionEventSignal or TranslationTextEventSignal or IntentEventSignal
        """
        return self._impl.recognizing

    @property
    def recognized(self):
        """
        Signal for events containing final recognition results (indicating a successful
        recognition attempt).

        Callbacks connected to this signal are called with a :class:`.SpeechRecognitionEventArgs`,
        :class:`.TranslationRecognitionEventArgs` or :class:`.IntentRecognitionEventArgs` instance as the single
        argument, dependent on the type of recognizer.

        :type: SpeechRecognitionEventSignal or TranslationTextEventSignal or IntentEventSignal
        """
        return self._impl.recognized

    @property
    def canceled(self):
        """
        Signal for events containing canceled recognition results (indicating a recognition attempt
        that was canceled as a result or a direct cancellation request or, alternatively, a
        transport or protocol failure).

        Callbacks connected to this signal are called with a :class:`.SpeechRecognitionCanceledEventArgs`,
        :class:`.TranslationRecognitionCanceledEventArgs` or :class:`.IntentRecognitionCanceledEventArgs` instance
        as the single argument, dependent on the type of recognizer.

        :type: SpeechRecognitionCanceledEventSignal or TranslationTextCanceledEventSignal or
            IntentCanceledEventSignal
        """
        return self._impl.canceled


class SpeechRecognizer(Recognizer):
    """
    A speech recognizer.

    :param speech_config: The config for the speech recognizer
    :param audio_config: The config for the audio input
    """
    def __init__(self, speech_config: SpeechConfig, audio_config: Optional[AudioConfig] = None):
        if not isinstance(speech_config, SpeechConfig):
            raise ValueError('speech_config must be a SpeechConfig instance')

        self._impl = self._get_impl(impl.SpeechRecognizer, speech_config, audio_config)

