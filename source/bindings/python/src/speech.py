# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.
"""
Classes related to recognizing text from speech, and general classes used in the various recognizers.
"""


from . import speech_py_impl as impl
from .audio import AudioConfig

from .speech_py_impl import (
    CancellationDetails,
    NoMatchDetails,
    OutputFormat,
    ProfanityOption,
    PropertyCollection,
    PropertyId,
    ResultReason,
    ServicePropertyChannel,
)

from typing import Optional, Dict, Union, Callable
OptionalStr = Optional[str]


class SpeechConfig():
    """
    Class that defines configurations for speech or intent recognition.

    The configuration can be initialized in different ways:

    - from subscription: pass a subscription key and a region
    - from endpoint: pass an endpoint. Subscription key or authorization token are optional.
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

    def get_property(self, property_id: PropertyId) -> str:
        """
        Get a property by id.

        :param property_id: The id of the property to be retrieved.
        :return: The value of the property.
        """
        if not isinstance(property_id, PropertyId):
            raise TypeError('property_id value must be PropertyId instance')
        return impl._speech_py_impl.SpeechConfig_get_property(self._impl, property_id.value)

    def set_property(self, property_id: PropertyId, value: str):
        """
        Set a property by id.

        :param property_id: The id of the property to be set.
        :param value: The value to be set for the property.
        """
        if not isinstance(property_id, PropertyId):
            raise TypeError('property_id value must be PropertyId instance')
        return impl._speech_py_impl.SpeechConfig_set_property(self._impl, property_id.value, value)

    def set_properties(self, properties: Dict[PropertyId, str]):
        """
        Set multiple properties by id.

        :param properties: A dict mapping property ids to the values to be set.
        """
        for property_id, value in properties.items():
            if not isinstance(property_id, PropertyId):
                raise TypeError('property_id value must be PropertyId instance')

            impl._speech_py_impl.SpeechConfig_set_property(self._impl, property_id.value, value)

    @staticmethod
    def _get_impl(config_type, subscription, region, endpoint, auth_token,
            speech_recognition_language):
        if endpoint is not None:
            if region is not None:
                raise ValueError('cannot construct SpeechConfig with both region and endpoint information')
            if auth_token is not None:
                raise ValueError('cannot specify both auth_token and endpoint when constructing SpeechConfig. Set authorization token separately after creating SpeechConfig.')

        if region is not None and subscription is None and auth_token is None:
            raise ValueError('either subscription key or authorization token must be given along with a region')

        if subscription is not None and endpoint is None and region is None:
            raise ValueError('either endpoint or region must be given along with a subscription key')

        generic_error_message = 'cannot construct SpeechConfig with the given arguments'
        _impl = None
        if region is not None and subscription is not None:
            if endpoint is not None or auth_token is not None:
                raise ValueError(generic_error_message)
            _impl = config_type._from_subscription(subscription, region)
        elif region is not None and auth_token is not None:
            if endpoint is not None or subscription is not None:
                raise ValueError(generic_error_message)
            _impl = config_type._from_authorization_token(auth_token, region)
        elif endpoint is not None:
            if subscription is not None:
                _impl = config_type._from_endpoint(endpoint, subscription)
            else:
                _impl = config_type._from_endpoint(endpoint)

        if _impl is not None:
            _impl.set_speech_recognition_language(speech_recognition_language)
            _impl.set_property("SPEECHSDK-SPEECH-CONFIG-SYSTEM-LANGUAGE", "Python")
            return _impl

        raise ValueError(generic_error_message)

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
        The authorization token that will be used for connecting to the service.

        .. note::

          The caller needs to ensure that the authorization token is valid. Before the
          authorization token expires, the caller needs to refresh it by calling this setter with a
          new valid token. As configuration values are copied when creating a new recognizer, the
          new token value will not apply to recognizers that have already been created. For
          recognizers that have been created before, you need to set authorization token of the
          corresponding recognizer to refresh the token. Otherwise, the recognizers will encounter
          errors during recognition.
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

        .. note::

            Proxy functionality is not available on macOS. This function will have no effect on
            this platform.

        :param hostname: The host name of the proxy server. Do not add protocol information (http)
            to the hostname.
        :param port: The port number of the proxy server.
        :param username: The user name of the proxy server.
        :param password: The password of the proxy server.
        """
        self._impl.set_proxy(hostname, port, username, password)

    def set_service_property(self, name: str, value: str, channel: ServicePropertyChannel):
        """
        Sets a property value that will be passed to service using the specified channel.

        .. note::
          Added in version 1.5.0.

        :param name: The property name.
        :param value: The property value.
        :param channel: The channel used to pass the specified property to service.
        """
        if not isinstance(channel, ServicePropertyChannel):
            raise TypeError('wrong channel, must be ServicePropertyChannel')
        self._impl.set_service_property(name, value, channel.value)

    def set_profanity(self, profanity_option: ProfanityOption) -> str:
        """
        Set the profanity option.

        .. note::
          Added in version 1.5.0.

        :param profanity_option: The profanity level to set.
        """
        if not isinstance(profanity_option, ProfanityOption):
            raise TypeError('bad option, must be ProfanityOption')
        self._impl.set_profanity(profanity_option.value)

    def enable_audio_logging(self):
        """
        Enables audio logging in service.

        .. note::
          Added in version 1.5.0.

        """
        self._impl.enable_audio_logging()

    def request_word_level_timestamps(self):
        """
        Includes word level timestamps in response result.

        .. note::
          Added in version 1.5.0.

        """
        self._impl.request_word_level_timestamps()

    def enable_dictation(self):
        """
        Enables dictation. Only supported in speech continuous recognition.

        .. note::
          Added in version 1.5.0.

        """
        self._impl.enable_dictation()


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
        self._properties = {}
        for property_id in PropertyId:
            value = impl_result.properties.get_property(property_id)
            if value:
                self._properties[property_id] = value

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

    @property
    def properties(self) -> dict:
        """
        Other properties of the result.

        :returns: `dict` indexed with :py:class:`.PropertyId`, and `str` values.
        """
        return self._properties.copy()

    def __str__(self):
        return u'{}(result_id={}, text="{}", reason={})'.format(
                type(self).__name__, self.result_id, self.text, self.reason)


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


class KeywordRecognitionModel():
    """
    Represents a keyword recognition model.

    :param filename: file name for the keyword recognition model.

    """

    def __init__(self, filename: str = None):
        if filename is None:
            raise ValueError('filename needs to be provided')
        self._impl = impl.KeywordRecognitionModel._from_file(filename)

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
        The authorization token that will be used for connecting to the service.

        .. note::
          The caller needs to ensure that the authorization token is valid. Before the
          authorization token expires, the caller needs to refresh it by calling this setter with a
          new valid token. Otherwise, the recognizer will encounter errors during recognition.
        """
        return self._impl.get_authorization_token()

    @authorization_token.setter
    def authorization_token(self, authorization_token: str):
        self._impl.set_authorization_token(authorization_token)

    def recognize_once(self) -> "Union[SpeechRecognitionResult, TranslationRecognitionResult, " \
            "IntentRecognitionResult]":
        """
        Performs recognition in a blocking (synchronous) mode. Returns after a single utterance is
        recognized. The end of a single utterance is determined by listening for silence at the end
        or until a maximum of 15 seconds of audio is processed. The task returns the recognition
        text as result. For long-running multi-utterance recognition, use
        :py:meth:`.start_continuous_recognition_async` instead.

        :return: The result value of the synchronous recognition.
        """
        raise NotImplementedError  # implemented in derived class

    def recognize_once_async(self) -> ResultFuture:
        """
        Performs recognition in a non-blocking (asynchronous) mode. This will recognize a single
        utterance. The end of a single utterance is determined by listening for silence at the end
        or until a maximum of 15 seconds of audio is processed. For long-running multi-utterance
        recognition, use :py:meth:`.start_continuous_recognition_async` instead.

        :return: A future containing the result value of the asynchronous recognition.
        """
        raise NotImplementedError  # implemented in derived class

    def start_continuous_recognition_async(self):
        """
        Asynchronously initiates continuous recognition operation. User has to connect to
        EventSignal to receive recognition results. Call
        :py:meth:`.stop_continuous_recognition_async` to stop the recognition.

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
        Synchronously initiates continuous recognition operation. User has to connect to
        EventSignal to receive recognition results. Call
        :py:meth:`.stop_continuous_recognition_async` to stop the recognition.
        """
        return self._impl.start_continuous_recognition()

    def stop_continuous_recognition(self):
        """
        Synchronously terminates ongoing continuous recognition operation.
        """
        return self._impl.stop_continuous_recognition()

    def start_keyword_recognition_async(self, model: KeywordRecognitionModel):
        """
        Asynchronously initiates keyword recognition operation.

        :param model: the keyword recognition model that specifies the keyword to be recognized.

        :return: A future that is fulfilled once recognition has been initialized.
        """
        return self._impl.start_keyword_recognition_async(model._impl)

    def stop_keyword_recognition_async(self):
        """
        Asynchronously terminates ongoing keyword recognition operation.

        :return: A future that is fulfilled once recognition has been stopped.
        """
        return self._impl.stop_keyword_recognition_async()

    def start_keyword_recognition(self, model: KeywordRecognitionModel):
        """
        Synchronously initiates keyword recognition operation.

        :param model: the keyword recognition model that specifies the keyword to be recognized.
        """
        return self._impl.start_keyword_recognition(model._impl)

    def stop_keyword_recognition(self):
        """
        Synchronously terminates ongoing keyword recognition operation.
        """
        return self._impl.stop_keyword_recognition()

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
        Performs recognition in a blocking (synchronous) mode. Returns after a single utterance is
        recognized. The end of a single utterance is determined by listening for silence at the end
        or until a maximum of 15 seconds of audio is processed. The task returns the recognition
        text as result. For long-running multi-utterance recognition, use
        :py:meth:`.start_continuous_recognition_async` instead.

        :return: The result value of the synchronous recognition.
        """
        return SpeechRecognitionResult(self._impl.recognize_once())

    def recognize_once_async(self) -> ResultFuture:
        """
        Performs recognition in a non-blocking (asynchronous) mode. This will recognize a single
        utterance. The end of a single utterance is determined by listening for silence at the end
        or until a maximum of 15 seconds of audio is processed. For long-running multi-utterance
        recognition, use :py:meth:`.start_continuous_recognition_async` instead.

        :return: A future containing the result value of the asynchronous recognition.
        """
        return ResultFuture(self._impl.recognize_once_async(), SpeechRecognitionResult)

    @property
    def recognizing(self) -> EventSignal:
        """
        Signal for events containing intermediate recognition results.

        Callbacks connected to this signal are called with a :class:`.SpeechRecognitionEventArgs`
        instance as the single argument.
        """
        return EventSignal(self._impl.recognizing, SpeechRecognitionEventArgs)

    @property
    def recognized(self) -> EventSignal:
        """
        Signal for events containing final recognition results (indicating a successful
        recognition attempt).

        Callbacks connected to this signal are called with a :class:`.SpeechRecognitionEventArgs`
        instance as the single argument, dependent on the type of recognizer.
        """
        return EventSignal(self._impl.recognized, SpeechRecognitionEventArgs)

    @property
    def canceled(self) -> EventSignal:
        """
        Signal for events containing canceled recognition results (indicating a recognition attempt
        that was canceled as a result or a direct cancellation request or, alternatively, a
        transport or protocol failure).

        Callbacks connected to this signal are called with a
        :class:`.SpeechRecognitionCanceledEventArgs`, instance as the single argument.
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
        return u'{}(session_id={})'.format(type(self).__name__, self.session_id)


class ConnectionEventArgs(SessionEventArgs):
    """
    Provides data for the ConnectionEvent.

    .. note::
      Added in version 1.2.0
    """

    def __init__(self, evt_args):
        """
        Constructor for internal use.
        """
        super().__init__(evt_args)


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
        return u'{}(session_id={}, result={})'.format(type(self).__name__, self.session_id, self.result)


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


class Connection():
    """
    Proxy class for managing the connection to the speech service of the specified
    :class:`.Recognizer`.

    By default, a :class:`.Recognizer` autonomously manages connection to service when needed. The
    :class:`.Connection` class provides additional methods for users to explicitly open or close a
    connection and to subscribe to connection status changes. The use of :class:`.Connection` is
    optional, and mainly for scenarios where fine tuning of application behavior based on
    connection status is needed. Users can optionally call :meth:`.open()` to manually set up a
    connection in advance before starting recognition on the :class:`.Recognizer` associated with
    this :class:`.Connection`. After starting recognition, calling :meth:`.open()` or
    :meth:`close()` might fail, depending on the process state of the :class:`.Recognizer`. But
    this does not affect the state of the associated :class:`.Recognizer`. And if the
    :class:`.Recognizer` needs to connect or disconnect to service, it will setup or shutdown the
    connection independently. In this case the :class:`.Connection` will be notified by change of
    connection status via the :attr:`.connected`/:attr:`.disconnected` events.

    .. note::
      Added in version 1.2.0.
    """
    def __init__(self, impl_connection):
        """
        Constructor for internal use.
        """
        self._impl = impl_connection

    @classmethod
    def from_recognizer(cls, recognizer: Recognizer):
        """
        Gets the :class:`.Connection` instance from the specified recognizer.
        """
        return cls(impl.Connection.from_recognizer(recognizer._impl))

    def open(self, for_continuous_recognition: bool):
        """
        Starts to set up connection to the service. Users can optionally call :meth:`.open()` to
        manually set up a connection in advance before starting recognition on the
        :class:`.Recognizer` associated with this :class:`.Connection`. After starting recognition,
        calling :meth:`.open()`) might fail, depending on the process state of the
        :class:`.Recognizer`. But the failure does not affect the state of the associated
        :class:`.Recognizer`.

        :param forContinuousRecognition: indicates whether the connection is used for continuous
          recognition or single-shot recognition.

        .. note:: On return, the connection might not be ready yet. Please subscribe to the
          `connected` event to be notfied when the connection is established.
        """
        self._impl.open(for_continuous_recognition)

    def close(self):
        """
        Closes the connection the service. Users can optionally call :meth:`close()` to manually
        shutdown the connection of the associated :class:`.Recognizer`. The call might fail,
        depending on the process state of the :class:`.Recognizer`. But the failure does not affect
        the state of the associated :class:`.Recognizer`.
        """
        self._impl.close()

    @property
    def connected(self) -> EventSignal:
        """
        The Connected event to indicate that the recognizer is connected to service.

        """
        return EventSignal(self._impl.connected, ConnectionEventArgs)

    @property
    def disconnected(self) -> EventSignal:
        """
        The Disconnected event to indicate that the recognizer is disconnected from service.

        """
        return EventSignal(self._impl.disconnected, ConnectionEventArgs)


class PhraseListGrammar():
    """
    Class that allows runtime addition of phrase hints to aid in speech recognition.

    Phrases added to the recognizer are effective at the start of the next recognition, or the next
    time the speech recognizer must reconnect to the speech service.

    .. note::
        Added in version 1.5.0.

    """
    @classmethod
    def from_recognizer(cls, recognizer: Recognizer):
        """
        Gets the :class:`.PhraseListGrammar` instance from the specified recognizer.
        """
        return cls(impl.PhraseListGrammar.from_recognizer(recognizer._impl))

    def __init__(self, impl_phraseListGrammar):
        """
        Constructor for internal use.
        """
        self._impl = impl_phraseListGrammar

    def addPhrase(self, phrase: str):
        """
        Adds a single phrase to the current recognizer.
        """
        self._impl.add_phrase(phrase)

    def clear(self):
        """
        Clears all phrases from the current recognizer.
        """
        self._impl.clear()

