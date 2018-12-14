#  Copyright (c) Microsoft. All rights reserved.
#  See https://aka.ms/csspeech/license201809 for the full license information.
"""
Classes related to recognizing text from speech, and general classes used in the various recognizers.
"""


from . import speech_py_impl as impl
from .audio import AudioConfig

from .speech_py_impl import (
    OutputFormat,
    PropertyCollection,
    PropertyId,
    CancellationDetails,
    ResultReason,
    NoMatchDetails
)

from typing import Optional, Dict, Union, Callable
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


class RecognitionResult():
    """
    Detailed information about the result of a recognition operation.
    """

    def __init__(self, impl_result):
        """
        Constructor for internal use.
        """
        self._cancellation_details = impl_result.cancellation_details
        self._no_match_details = impl_result.no_match_details
        self._offset = impl_result.offset
        self._duration = impl_result.duration
        self._result_id = impl_result.result_id
        self._reason = impl_result.reason
        self._text = impl_result.text
        self._json = impl_result.json
        self._error_json = impl_result.error_json

    @property
    def cancellation_details(self) -> "CancellationDetails":
        """
        The reason why recognition was cancelled.

        Returns `None` if there was no cancellation.
        """
        return self._cancellation_details

    @property
    def no_match_details(self) -> "NoMatchDetails":
        """
        Detailed information for NoMatch recognition results.

        Returns `None` if there was a match found.
        """
        return self._no_match_details

    @property
    def offset(self) -> int:
        """
        Offset of the recognized speech in ticks. A single tick represents one hundred nanoseconds
        or one ten-millionth of a second.
        """
        return self._offset

    @property
    def duration(self) -> int:
        """
        Duration of recognized speech in ticks. A single tick represents one hundred
        nanoseconds or one ten-millionth of a second.
        """
        return self._duration

    @property
    def result_id(self) -> str:
        """
        Unique result id.
        """
        return self._result_id

    @property
    def reason(self) -> "ResultReason":
        """
        Recognition reason.
        """
        return self._reason

    @property
    def text(self) -> str:
        """
        Normalized text generated by a speech recognition engine from recognized input.
        """
        return self._text

    @property
    def json(self) -> str:
        """
        The bare JSON representation of the result from the Speech Service.
        """
        return self._json

    @property
    def error_json(self) -> str:
        """
        The bare JSON representation of the error from the Speech Service.
        """
        return self._error_json

    def __str__(self):
        return u'{}:(result_id={}, text={})'.format(type(self), self.result_id, self.text)


class SpeechRecognitionResult(RecognitionResult):
    """
    Base class for speech recognition results.
    """

    def __init__(self, impl_result):
        """
        Constructor for internal use.
        """
        super().__init__(impl_result)

class ResultFuture():
    """
    The result of an asynchronous operation.
    """
    def __init__(self, impl_future, wrapped_type):
        """
        private constructor
        """
        self._impl = impl_future
        self._wrapped_type = wrapped_type

    def get(self):
        """
        Waits until the result is available, and returns it.
        """
        return self._wrapped_type(self._impl.get())


class EventSignal():
    """
    Clients can connect to the event signal to receive events, or disconnect from
    the event signal to stop receiving events.
    """

    def __init__(self, impl_event_signal, event_args_type):
        """
        Constructor for internal use.
        """
        self._impl = impl_event_signal
        self._event_args_type = event_args_type

    def connect(self, callback: Callable):
        """
        Connects given callback function to the event signal, to be invoked when the
        event is signalled.
        """
        self._impl.connect(callback, self._event_args_type)

    def disconnect_all(self):
        """
        Disconnects all registered callbacks.
        """
        self._impl.disconnect_all()


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

    def recognize_once(self) -> "Union[SpeechRecognitionResult, TranslationRecognitionResult,IntentRecognitionResult]":
        """
        Performs recognition in a blocking (synchronous) mode.

        :return: The result value of the synchronous recognition.
        """
        raise NotImplementedError  # implemented in derived class

    def recognize_once_async(self) -> ResultFuture:
        """
        Performs recognition in a non-blocking (asynchronous) mode.

        :return: A future containing the result value of the asynchronous recognition.
        """
        raise NotImplementedError  # implemented in derived class

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
    def session_started(self) -> EventSignal:
        """
        Signal for events indicating the start of a recognition session (operation).

        Callbacks connected to this signal are called with a :class:`.SessionEventArgs` instance as
        the single argument.
        """
        return EventSignal(self._impl.session_started, SessionEventArgs)

    @property
    def session_stopped(self) -> EventSignal:
        """
        Signal for events indicating the end of a recognition session (operation).

        Callbacks connected to this signal are called with a :class:`.SessionEventArgs` instance as
        the single argument.
        """
        return EventSignal(self._impl.session_stopped, SessionEventArgs)

    @property
    def speech_start_detected(self) -> EventSignal:
        """
        Signal for events indicating the start of speech.

        Callbacks connected to this signal are called with a :class:`.RecognitionEventArgs`
        instance as the single argument.
        """
        return EventSignal(self._impl.speech_start_detected, RecognitionEventArgs)

    @property
    def speech_end_detected(self) -> EventSignal:
        """
        Signal for events indicating the end of speech.

        Callbacks connected to this signal are called with a :class:`.RecognitionEventArgs`
        instance as the single argument.
        """
        return EventSignal(self._impl.speech_end_detected, RecognitionEventArgs)

    @property
    def recognizing(self) -> EventSignal:
        """
        Signal for events containing intermediate recognition results.

        Callbacks connected to this signal are called with a :class:`.SpeechRecognitionEventArgs`,
        :class:`.TranslationRecognitionEventArgs` or :class:`.IntentRecognitionEventArgs` instance
        as the single argument, dependent on the type of recognizer.
        """
        raise NotImplementedError  # implemented in derived class

    @property
    def recognized(self) -> EventSignal:
        """
        Signal for events containing final recognition results (indicating a successful
        recognition attempt).

        Callbacks connected to this signal are called with a :class:`.SpeechRecognitionEventArgs`,
        :class:`.TranslationRecognitionEventArgs` or :class:`.IntentRecognitionEventArgs` instance
        as the single argument, dependent on the type of recognizer.
        """
        raise NotImplementedError  # implemented in derived class

    @property
    def canceled(self) -> EventSignal:
        """
        Signal for events containing canceled recognition results (indicating a recognition attempt
        that was canceled as a result or a direct cancellation request or, alternatively, a
        transport or protocol failure).

        Callbacks connected to this signal are called with a
        :class:`.SpeechRecognitionCanceledEventArgs`,
        :class:`.TranslationRecognitionCanceledEventArgs` or
        :class:`.IntentRecognitionCanceledEventArgs` instance as the single argument, dependent on
        the type of recognizer.
        """
        raise NotImplementedError  # implemented in derived class


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

    def recognize_once(self) -> SpeechRecognitionResult:
        """
        Performs recognition in a blocking (synchronous) mode.

        :return: The result value of the synchronous recognition.
        """
        return SpeechRecognitionResult(self._impl.recognize_once())

    def recognize_once_async(self) -> ResultFuture:
        """
        Performs recognition in a non-blocking (asynchronous) mode.

        :return: A future containing the result value of the asynchronous recognition.
        """
        return ResultFuture(self._impl.recognize_once_async(), SpeechRecognitionResult)

    @property
    def recognizing(self) -> EventSignal:
        """
        Signal for events containing intermediate recognition results.

        Callbacks connected to this signal are called with a :class:`.SpeechRecognitionEventArgs`,
        instance as the single argument.
        """
        return EventSignal(self._impl.recognizing, SpeechRecognitionEventArgs)

    @property
    def recognized(self) -> EventSignal:
        """
        Signal for events containing final recognition results (indicating a successful
        recognition attempt).

        Callbacks connected to this signal are called with a :class:`.SpeechRecognitionEventArgs`,
        instance as the single argument, dependent on the type of recognizer.
        """
        return EventSignal(self._impl.recognized, SpeechRecognitionEventArgs)

    @property
    def canceled(self) -> EventSignal:
        """
        Signal for events containing canceled recognition results (indicating a recognition attempt
        that was canceled as a result or a direct cancellation request or, alternatively, a
        transport or protocol failure).

        Callbacks connected to this signal are called with a :class:`.SpeechRecognitionCanceledEventArgs`,
        instance as the single argument.
        """
        return EventSignal(self._impl.canceled, SpeechRecognitionCanceledEventArgs)


class SessionEventArgs():
    """
    Base class for session event arguments.
    """

    def __init__(self, evt_args):
        """
        Constructor for internal use.
        """
        self._impl = None
        self._session_id = evt_args.session_id

    @property
    def session_id(self) -> str:
        """
        Session identifier (a GUID in string format).
        """
        return self._session_id

    def __str__(self):
        return u'{}:(session_id={})'.format(type(self), self.session_id)


class RecognitionEventArgs(SessionEventArgs):
    """
    Provides data for the RecognitionEvent.
    """

    def __init__(self, evt_args):
        """
        Constructor for internal use.
        """
        super().__init__(evt_args)
        self._offset = evt_args.offset

    @property
    def offset(self) -> int:
        """
        The offset of the recognition event in ticks. A single tick represents one hundred
        nanoseconds or one ten-millionth of a second.
        """
        return self._offset


class SpeechRecognitionEventArgs(RecognitionEventArgs):
    """
    Class for speech recognition event arguments.
    """

    def __init__(self, evt_args):
        """
        Constructor for internal use.
        """
        super().__init__(evt_args)
        self._result = SpeechRecognitionResult(evt_args.result)

    @property
    def result(self) -> "SpeechRecognitionResult":
        """
        Speech recognition event result.
        """
        return self._result

    def __str__(self):
        return u'{}:(session_id={}, result={})'.format(type(self), self.session_id, self.result)


class SpeechRecognitionCanceledEventArgs(SpeechRecognitionEventArgs):
    """
    Class for speech recognition canceled event arguments.
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


