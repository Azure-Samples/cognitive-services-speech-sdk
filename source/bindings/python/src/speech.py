# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.
"""
Classes related to recognizing text from speech, synthesizing speech from text, and general classes used in the various recognizers.
"""


from . import speech_py_impl as impl
from . import audio
from . import languageconfig

from .speech_py_impl import (
    CancellationDetails,
    NoMatchDetails,
    OutputFormat,
    ProfanityOption,
    PropertyCollection,
    PropertyId,
    ResultReason,
    ServicePropertyChannel,
    StreamStatus,
    SpeechSynthesisCancellationDetails,
    SpeechSynthesisOutputFormat,
    AudioStreamContainerFormat,
)

from typing import Optional, Dict, Union, Callable
OptionalStr = Optional[str]


class SpeechConfig():
    """
    Class that defines configurations for speech / intent recognition and speech synthesis.

    The configuration can be initialized in different ways:

    - from subscription: pass a subscription key and a region
    - from endpoint: pass an endpoint. Subscription key or authorization token are optional.
    - from host: pass a host address. Subscription key or authorization token are optional.
    - from authorization token: pass an authorization token and a region

    :param subscription: The subscription key.
    :param region: The region name (see the `region page <https://aka.ms/csspeech/region>`_).
    :param endpoint: The service endpoint to connect to.
    :param host: The service host to connect to. Standard resource path will be assumed. Format
        is "protocol://host:port" where ":port" is optional.
    :param auth_token: The authorization token.
    :param speech_recognition_language: The input language to the speech recognition. The language
        is specified in BCP-47 format.
    """
    def __init__(self, subscription: OptionalStr = None, region: OptionalStr = None,
            endpoint: OptionalStr = None, host: OptionalStr = None, auth_token: OptionalStr = None,
            speech_recognition_language: OptionalStr = None):

        self._impl = self._get_impl(impl.SpeechConfig, subscription, region, endpoint, host, auth_token,
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

    def get_property_by_name(self, property_name: str) -> str:
        """
        Get a property by name.

        :param property_name: The name of the property to be retrieved.
        :return: The value of the property.
        """
        if not isinstance(property_name, str):
            raise TypeError('property_name value must be str instance')
        return impl._speech_py_impl.SpeechConfig_get_property(self._impl, property_name)

    def set_property(self, property_id: PropertyId, value: str):
        """
        Set a property by id.

        :param property_id: The id of the property to be set.
        :param value: The value to be set for the property.
        """
        if not isinstance(property_id, PropertyId):
            raise TypeError('property_id value must be PropertyId instance')
        return impl._speech_py_impl.SpeechConfig_set_property(self._impl, property_id.value, value)

    def set_property_by_name(self, property_name: str, value: str):
        """
        Set a property by name.

        :param property_name: The name of the property to be set.
        :param value: The value to be set for the property.
        """
        if not isinstance(property_name, str):
            raise TypeError('property_name value must be str instance')
        return impl._speech_py_impl.SpeechConfig_set_property(self._impl, property_name, value)

    def set_properties(self, properties: Dict[PropertyId, str]):
        """
        Set multiple properties by id.

        :param properties: A dict mapping property ids to the values to be set.
        """
        for property_id, value in properties.items():
            if not isinstance(property_id, PropertyId):
                raise TypeError('property_id value must be PropertyId instance')

            impl._speech_py_impl.SpeechConfig_set_property(self._impl, property_id.value, value)

    def set_properties_by_name(self, properties: Dict[str, str]):
        """
        Set multiple properties by name.

        :param properties: A dict mapping property ids to the values to be set.
        """
        for property_name, value in properties.items():
            if not isinstance(property_name, str):
                raise TypeError('property_name value must be str instance')

            impl._speech_py_impl.SpeechConfig_set_property(self._impl, property_name, value)

    @staticmethod
    def _get_impl(config_type, subscription, region, endpoint, host, auth_token,
            speech_recognition_language):
        if endpoint is not None or host is not None:
            if region is not None:
                raise ValueError('cannot construct SpeechConfig with both region and endpoint or host information')
            if auth_token is not None:
                raise ValueError('cannot specify both auth_token and endpoint or host when constructing SpeechConfig. Set authorization token separately after creating SpeechConfig.')

        if region is not None and subscription is None and auth_token is None:
            raise ValueError('either subscription key or authorization token must be given along with a region')

        if subscription is not None and endpoint is None and host is None and region is None:
            raise ValueError('either endpoint, host, or region must be given along with a subscription key')

        generic_error_message = 'cannot construct SpeechConfig with the given arguments'
        _impl = None
        if region is not None and subscription is not None:
            if endpoint is not None or host is not None or auth_token is not None:
                raise ValueError(generic_error_message)
            _impl = config_type._from_subscription(subscription, region)
        elif region is not None and auth_token is not None:
            if endpoint is not None or host is not None or subscription is not None:
                raise ValueError(generic_error_message)
            _impl = config_type._from_authorization_token(auth_token, region)
        elif endpoint is not None:
            if subscription is not None:
                _impl = config_type._from_endpoint(endpoint, subscription)
            else:
                _impl = config_type._from_endpoint(endpoint)
        elif host is not None:
            if subscription is not None:
                _impl = config_type._from_host(host, subscription)
            else:
                _impl = config_type._from_host(host)

        if _impl is not None:
            _impl.set_property("SPEECHSDK-SPEECH-CONFIG-SYSTEM-LANGUAGE", "Python")
            if speech_recognition_language is not None:
                _impl.set_speech_recognition_language(speech_recognition_language)
            return _impl

        raise ValueError(generic_error_message)

    @property
    def output_format(self) -> OutputFormat:
        """
        The output format (simple or detailed) of the speech recognition result.
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

    @property
    def speech_synthesis_language(self) -> str:
        """
        Get speech synthesis language.
        """
        return self._impl.get_speech_synthesis_language()

    @speech_synthesis_language.setter
    def speech_synthesis_language(self, language: str):
        """
        Set speech synthesis language.

        :param language: The language for speech synthesis (e.g. en-US).
        """
        self._impl.set_speech_synthesis_language(language)

    @property
    def speech_synthesis_voice_name(self) -> str:
        """
        Get speech synthesis voice name.
        """
        return self._impl.get_speech_synthesis_voice_name()

    @speech_synthesis_voice_name.setter
    def speech_synthesis_voice_name(self, voice: str):
        """
        Set speech synthesis voice name.

        :param voice: The name of voice for speech synthesis.
        """
        self._impl.set_speech_synthesis_voice_name(voice)

    @property
    def speech_synthesis_output_format_string(self) -> str:
        """
        Get speech synthesis output audio format string.
        """
        return self._impl.get_speech_synthesis_output_format()

    def set_speech_synthesis_output_format(self, format_id: SpeechSynthesisOutputFormat):
        """
        Set speech synthesis output audio format.

        :param format_id: The audio format id, e.g. Riff16Khz16BitMonoPcm.
        """
        if not isinstance(format_id, SpeechSynthesisOutputFormat):
            raise TypeError('wrong type, must be SpeechSynthesisOutputFormat')
        self._impl.set_speech_synthesis_output_format(format_id.value)

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

class AutoDetectSourceLanguageResult():
    """
    Represents auto detection source language result.
    
    The result can be initialized from a speech recognition result.
    
    :param speechRecognitionResult: The speech recognition result
    """
    def __init__(self, speechRecognitionResult: SpeechRecognitionResult):        
        self._language = speechRecognitionResult.properties.get(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult)
    
    @property
    def language(self) -> str:
        """
        The language value
        If this is None, it means the system fails to detect the source language automatically
        """
        return self._language

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
    If you need to specify source language information, please only specify one of these three parameters, language, source_language_config or auto_detect_source_language_config.

    :param speech_config: The config for the speech recognizer 
    :param audio_config: The config for the audio input
    :param language: The source language
    :param source_language_config: The source language config
    :param auto_detect_source_language_config: The auto detection source language config
    """
    def __init__(self, speech_config: SpeechConfig, audio_config: Optional[audio.AudioConfig] = None, language: OptionalStr = None, 
            source_language_config: Optional[languageconfig.SourceLanguageConfig] = None, auto_detect_source_language_config: Optional[languageconfig.AutoDetectSourceLanguageConfig] = None):
        if not isinstance(speech_config, SpeechConfig):
            raise ValueError('speech_config must be a SpeechConfig instance')
        languageConfigNum = 0
        if language is not None: 
            if language == "":
                raise ValueError('language cannot be an empty string')
            languageConfigNum = languageConfigNum + 1
        if source_language_config is not None:
            languageConfigNum = languageConfigNum + 1
        if auto_detect_source_language_config is not None:
            languageConfigNum = languageConfigNum + 1
        if languageConfigNum > 1:
            raise ValueError('cannot construct SpeechRecognizer with more than one language configurations, please only specify one of these three parameters: language, source_language_config or auto_detect_source_language_config')

        
        self._impl = self._get_impl(impl.SpeechRecognizer, speech_config, audio_config, language, source_language_config, auto_detect_source_language_config)

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

    @staticmethod
    def _get_impl(reco_type, speech_config, audio_config, language, source_language_config, auto_detect_source_language_config):
        if language is None and source_language_config is None and auto_detect_source_language_config is None:
            if audio_config is None:
                return reco_type._from_config(speech_config._impl)
            return reco_type._from_config(speech_config._impl,  audio_config._impl)
        if language is not None:
            if audio_config is None:
                return reco_type._from_config(speech_config._impl, language)
            return reco_type._from_config(speech_config._impl, language, audio_config._impl)
        
        if source_language_config is not None:            
            if audio_config is None:
                return reco_type._from_config(speech_config._impl, source_language_config._impl)
            return reco_type._from_config(speech_config._impl, source_language_config._impl, audio_config._impl)
        
        # auto_detect_source_language_config must not be None if we arrive this code
        if audio_config is None:
            return reco_type._from_config(speech_config._impl, auto_detect_source_language_config._impl)
        return reco_type._from_config(speech_config._impl, auto_detect_source_language_config._impl, audio_config._impl)

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
    optional. It is intended for scenarios where fine tuning of application behavior based on connection
    status is needed. Users can optionally call :meth:`.open()` to manually initiate a service connection
    before starting recognition on the :class:`.Recognizer` associated with this :class:`.Connection`.
    After starting a recognition, calling :meth:`.open()` or :meth:`close()` might fail. This will not impact
    the Recognizer or the ongoing recognition. Connection might drop for various reasons, the Recognizer will
    always try to reinstitute the connection as required to guarantee ongoing operations. In all these cases
    :obj:`.connected`/:obj:`.disconnected` events will indicate the change of the connection status.

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

    def set_message_property(self, path: str, property_name: str, property_value: str):
        """
        Appends a parameter in a message to service.
        """
        return self._impl.set_message_property(path, property_name, property_value)

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


class SpeechSynthesisResult():
    """
    Result of a speech synthesis operation.
    """

    def __init__(self, impl_result):
        """
        Constructor for internal use.
        """
        self._impl = impl_result
        self._cancellation_details = impl_result.cancellation_details
        self._result_id = impl_result.result_id
        self._reason = impl_result.reason
        self._audio_data = impl_result.audio_data

    @property
    def cancellation_details(self) -> "SpeechSynthesisCancellationDetails":
        """
        The reason why speech synthesis was cancelled.

        Returns `None` if there was no cancellation.
        """
        return self._cancellation_details

    @property
    def result_id(self) -> str:
        """
        Synthesis result unique ID.
        """
        return self._result_id

    @property
    def reason(self) -> "ResultReason":
        """
        Synthesis reason.
        """
        return self._reason

    @property
    def audio_data(self) -> bytes:
        """
        The output audio data from the TTS.
        """
        return self._audio_data

    def __str__(self):
        return u'{}(result_id={}, reason={}, audio_length={})'.format(
            type(self).__name__, self._result_id, self._reason, len(self._audio_data))


class SpeechSynthesisEventArgs:
    """
    Class for speech synthesis event arguments.
    """

    def __init__(self, evt_args):
        """
        Constructor for internal use.
        """
        self._result = SpeechSynthesisResult(evt_args.result)

    @property
    def result(self) -> "SpeechSynthesisResult":
        """
        Speech synthesis event result.
        """
        return self._result

    def __str__(self):
        return u'{}(result=[{}])'.format(type(self).__name__, self._result.__str__())


class SpeechSynthesisWordBoundaryEventArgs:
    """
    Class for speech synthesis word boundary event arguments.
    """

    def __init__(self, evt_args):
        """
        Constructor for internal use.
        """
        self._audio_offset = evt_args.audio_offset
        self._text_offset = evt_args.text_offset
        self._word_length = evt_args.word_length

    @property
    def audio_offset(self) -> int:
        """
        Word boundary audio offset in ticks. A single tick represents one hundred
        nanoseconds or one ten-millionth of a second.
        """
        return self._audio_offset

    @property
    def text_offset(self) -> int:
        """
        Word boundary text offset in characters.
        """
        return self._text_offset

    @property
    def word_length(self) -> int:
        """
        Word boundary word length in characters.
        """
        return self._word_length

    def __str__(self):
        return u'{}(audio_offset={}, text_offset={}, word_length={})'.format(
            type(self).__name__, self._audio_offset, self._text_offset, self._word_length)


class AudioDataStream():
    """
    Represents audio data stream used for operating audio data as a stream.

    Generates an audio data stream from a speech synthesis result (type SpeechSynthesisResult).

    :param result: The speech synthesis result.
    """

    def __init__(self, result: SpeechSynthesisResult):
        if result is None:
            raise ValueError('result must be provided')

        if not isinstance(result, SpeechSynthesisResult):
            raise ValueError('result must be a SpeechSynthesisResult, is "{}"'.format(result))

        self._impl = impl.AudioDataStream._from_result(result._impl)

    @property
    def status(self) -> "StreamStatus":
        """
        Current status of the audio data stream.
        """
        return self._impl.status

    def can_read_data(self, requested_bytes: int, pos: Optional[int] = None) -> bool:
        """
        Check whether the stream has enough data to be read,
        starting from the specified position (if specified).

        :param requested_bytes: The requested data size in bytes.
        :param pos: The position to start with.
            Will start from current position if this param is not given.
        :return: A bool indicating the result
        """
        if pos is None:
            return self._impl.can_read_data(requested_bytes)
        elif isinstance(pos, int):
            current_pos = self._impl.get_position()
            self._impl.set_position(pos)
            can = self._impl.can_read_data(requested_bytes)
            self._impl.set_position(current_pos)
            return can
        else:
            raise ValueError('pos must be an int, is "{}"'.format(pos))

    def read_data(self, audio_buffer: bytes, pos: Optional[int] = None) -> int:
        """
        Reads the audio data from the audio data stream,
        starting from the specified position (if specified).
        The maximal number of bytes to be read is determined by the size of audio_buffer.
        If there is no data immediately available, read_data() blocks until
        the next data becomes available.

        :param audio_buffer: The buffer to receive the audio data.
        :param pos: The position to start with.
            Will start from current position if this param is not given.
        :return: The number of bytes filled, or 0 in case the stream hits its end and
            there is no more data available.
        """
        if audio_buffer is None:
            raise ValueError('audio_buffer must be provided')

        if not isinstance(audio_buffer, bytes):
            raise ValueError('audio_buffer must be a bytes, is "{}"'.format(audio_buffer))

        if pos is None:
            return self._impl.read_data(audio_buffer)
        elif isinstance(pos, int):
            self._impl.set_position(pos)
            return self._impl.read_data(audio_buffer)
        else:
            raise ValueError('pos must be an int, is "{}"'.format(pos))

    def save_to_wav_file(self, file_name: str):
        """
        Save the audio data to a file, synchronously.

        :param file_name: Name of the file to be saved to
        """
        if not file_name:
            raise ValueError('file_name must be provided')

        if not isinstance(file_name, str):
            raise ValueError('file_name must be a str, is "{}"'.format(file_name))

        self._impl.save_to_wav_file(file_name)

    def save_to_wav_file_async(self, file_name: str):
        """
        Save the audio data to a file, asynchronously.

        :param file_name: Name of the file to be saved to
        :return: An asynchronous operation representing the saving.
        """
        if not file_name:
            raise ValueError('file_name must be provided')

        if not isinstance(file_name, str):
            raise ValueError('file_name must be a str, is "{}"'.format(file_name))

        return self._impl.save_to_wav_file_async(file_name)

    @property
    def position(self) -> int:
        """
        Current position of the audio data stream.
        """
        return self._impl.get_position()

    @position.setter
    def position(self, pos: int):
        if pos is None:
            raise ValueError('pos must be provided')

        if not isinstance(pos, int):
            raise ValueError('pos must be an int, is "{}"'.format(pos))

        self._impl.set_position(pos)


class SpeechSynthesizer:
    """
    A speech synthesizer.

    :param speech_config: The config for the speech synthesizer
    :param audio_config: The config for the audio output.
        This parameter is optional.
        If it is not provided, the default speaker device will be used for audio output.
        If it is None, the output audio will be dropped.
        None can be used for scenarios like performance test.
    :param auto_detect_source_language_config: The auto detection source language config
    """
    def __init__(self, speech_config: SpeechConfig,
                audio_config: Optional[audio.AudioOutputConfig] = 'None',
                auto_detect_source_language_config: Optional[languageconfig.AutoDetectSourceLanguageConfig] = None):
        if not isinstance(speech_config, SpeechConfig):
            raise ValueError('speech_config must be a SpeechConfig instance')

        self._impl = self._get_impl(impl.SpeechSynthesizer, speech_config, audio_config, auto_detect_source_language_config)

    def speak_text(self, text: str) -> SpeechSynthesisResult:
        """
        Performs synthesis on plain text in a blocking (synchronous) mode.

        :return: A SpeechSynthesisResult.
        """
        return SpeechSynthesisResult(self._impl.speak_text(text))

    def speak_ssml(self, ssml: str) -> SpeechSynthesisResult:
        """
        Performs synthesis on ssml in a blocking (synchronous) mode.

        :return: A SpeechSynthesisResult.
        """
        return SpeechSynthesisResult(self._impl.speak_ssml(ssml))

    def speak_text_async(self, text: str) -> ResultFuture:
        """
        Performs synthesis on plain text in a non-blocking (asynchronous) mode.

        :return: A future with SpeechSynthesisResult.
        """
        return ResultFuture(self._impl.speak_text_async(text), SpeechSynthesisResult)

    def speak_ssml_async(self, ssml: str) -> ResultFuture:
        """
        Performs synthesis on ssml in a non-blocking (asynchronous) mode.

        :return: A future with SpeechSynthesisResult.
        """
        return ResultFuture(self._impl.speak_ssml_async(ssml), SpeechSynthesisResult)

    def start_speaking_text(self, text: str) -> SpeechSynthesisResult:
        """
        Starts synthesis on plain text in a blocking (synchronous) mode.

        :return: A SpeechSynthesisResult.
        """
        return SpeechSynthesisResult(self._impl.start_speaking_text(text))

    def start_speaking_ssml(self, ssml: str) -> SpeechSynthesisResult:
        """
        Starts synthesis on ssml in a blocking (synchronous) mode.

        :return: A SpeechSynthesisResult.
        """
        return SpeechSynthesisResult(self._impl.start_speaking_ssml(ssml))

    def start_speaking_text_async(self, text: str) -> ResultFuture:
        """
        Starts synthesis on plain text in a non-blocking (asynchronous) mode.

        :return: A future with SpeechSynthesisResult.
        """
        return ResultFuture(self._impl.start_speaking_text_async(text), SpeechSynthesisResult)

    def start_speaking_ssml_async(self, ssml: str) -> ResultFuture:
        """
        Starts synthesis on ssml in a non-blocking (asynchronous) mode.

        :return: A future with SpeechSynthesisResult.
        """
        return ResultFuture(self._impl.start_speaking_ssml_async(ssml), SpeechSynthesisResult)

    @staticmethod
    def _get_impl(synth_type, speech_config, audio_config, auto_detect_source_language_config):
        if auto_detect_source_language_config is not None:
            if isinstance(audio_config, str) and audio_config == 'None':
                _impl = synth_type._from_config(speech_config._impl, auto_detect_source_language_config._impl)
            else:
                _impl = synth_type._from_config(speech_config._impl,
                                                auto_detect_source_language_config._impl,
                                                None if audio_config is None else audio_config._impl)
        elif isinstance(audio_config, str) and audio_config == 'None':
            _impl = synth_type._from_config(speech_config._impl)
        else:
            _impl = synth_type._from_config(speech_config._impl, None if audio_config is None else audio_config._impl)

        return _impl

    @property
    def properties(self) -> PropertyCollection:
        """
        A collection of properties and their values defined for this SpeechSynthesizer.
        """
        return self._impl.properties

    @property
    def authorization_token(self) -> str:
        """
        The authorization token that will be used for connecting to the service.

        .. note::
          The caller needs to ensure that the authorization token is valid. Before the
          authorization token expires, the caller needs to refresh it by calling this setter with a
          new valid token. Otherwise, the synthesizer will encounter errors while speech synthesis.
        """
        return self._impl.get_authorization_token()

    @authorization_token.setter
    def authorization_token(self, authorization_token: str):
        self._impl.set_authorization_token(authorization_token)

    @property
    def synthesis_started(self) -> EventSignal:
        """
        Signal for events indicating synthesis has started.

        Callbacks connected to this signal are called with a :class:`.SpeechSynthesisEventArgs`
        instance as the single argument.
        """
        return EventSignal(self._impl.synthesis_started, SpeechSynthesisEventArgs)

    @property
    def synthesizing(self) -> EventSignal:
        """
        Signal for events indicating synthesis is ongoing.

        Callbacks connected to this signal are called with a :class:`.SpeechSynthesisEventArgs`
        instance as the single argument.
        """
        return EventSignal(self._impl.synthesizing, SpeechSynthesisEventArgs)

    @property
    def synthesis_completed(self) -> EventSignal:
        """
        Signal for events indicating synthesis has completed.

        Callbacks connected to this signal are called with a :class:`.SpeechSynthesisEventArgs`
        instance as the single argument.
        """
        return EventSignal(self._impl.synthesis_completed, SpeechSynthesisEventArgs)

    @property
    def synthesis_canceled(self) -> EventSignal:
        """
        Signal for events indicating synthesis has been canceled.

        Callbacks connected to this signal are called with a :class:`.SpeechSynthesisEventArgs`
        instance as the single argument.
        """
        return EventSignal(self._impl.synthesis_canceled, SpeechSynthesisEventArgs)

    @property
    def synthesis_word_boundary(self) -> EventSignal:
        """
        Signal for events indicating a word boundary.

        Callbacks connected to this signal are called with a :class:`.SpeechSynthesisWordBoundaryEventArgs`
        instance as the single argument.
        """
        return EventSignal(self._impl.word_boundary, SpeechSynthesisWordBoundaryEventArgs)
