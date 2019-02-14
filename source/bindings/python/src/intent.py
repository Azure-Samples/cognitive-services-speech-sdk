# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.
"""
Classes related to intent recognition from speech.
"""

from . import speech_py_impl as impl
from .speech import (SpeechConfig, Recognizer, AudioConfig, ResultFuture, EventSignal,
                     RecognitionEventArgs, RecognitionResult)

from typing import Optional, Union, Tuple, Iterable
OptionalStr = Optional[str]


class LanguageUnderstandingModel():
    """
    Represents language understanding model used for intent recognition.

    The model can be initialized in different ways:

    - from subscription: pass a subscription key, an app_id, and a region
    - from endpoint: pass an endpoint
    - from app id: pass an app_id

    :param subscription: The subscription key.
    :param region: The region name (see the `region page <https://aka.ms/csspeech/region>`_).
    :param app_id: The app id to use for the model.
    :param auth_token: The authorization token.
    """

    def __init__(self, subscription: OptionalStr = None, region: OptionalStr = None,
                 app_id: OptionalStr = None, endpoint: OptionalStr = None):
        bad_params_error_message = "bad arguments: either pass just an endpoint id, or pass an app " \
                "id (with optional subscription and region)"
        if subscription is None and region is None and app_id is None and endpoint is None:
            raise ValueError(bad_params_error_message)

        if (sum(val is not None for val in (subscription, region)) == 1 or (
                app_id is None and subscription is not None and region is not None)):
            raise ValueError("all of subscription key, api id and region must be given to "
                    "initialize from subscription")

        if app_id is not None and endpoint is not None:
            raise ValueError(bad_params_error_message)

        if app_id is not None:
            self._impl = impl.LanguageUnderstandingModel._from_app_id(app_id)
        elif app_id is not None and subscription is not None and region is not None:
            self._impl = impl.LanguageUnderstandingModel._from_subscription(subscription, app_id, region)
        elif endpoint is not None:
            self._impl = impl.LanguageUnderstandingModel._from_endpoint(endpoint)
        else:
            raise ValueError('cannot construct LanguageUnderstandingModel')


class IntentRecognitionResult(RecognitionResult):
    """
    Represents the result of an intent recognition.
    """

    def __init__(self, impl_result):
        """
        Constructor for internal use.
        """
        super().__init__(impl_result)
        self._intent_id = impl_result.intent_id
        self._intent_json = impl_result.intent_json

    @property
    def intent_id(self) -> str:
        """
        Unique intent id.
        """
        return self._intent_id

    @property
    def intent_json(self) -> str:
        """
        The bare JSON representation of the result from the Language Understanding service.
        """
        return self._intent_json

    def __str__(self):
        return u'{}(result_id={}, text="{}", intent_id={}, reason={})'.format(
                type(self).__name__, self.result_id, self.text, self.intent_id, self.reason)


class IntentRecognitionEventArgs(RecognitionEventArgs):
    """
    Class for intent recognition event arguments.
    """

    def __init__(self, evt_args):
        """
        Constructor for internal use.
        """
        super().__init__(evt_args)
        self._result = IntentRecognitionResult(evt_args.result)

    @property
    def result(self) -> IntentRecognitionResult:
        """
        Intent recognition event result.
        """
        return self._result

    def __str__(self):
        return u'{}(session_id={}, result={})'.format(type(self).__name__, self.session_id, self.result)


class IntentRecognitionCanceledEventArgs(IntentRecognitionEventArgs):
    """
    Class for intent recognition canceled event arguments.
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


class IntentRecognizer(Recognizer):
    """
    In addition to performing speech-to-text recognition, the IntentRecognizer extracts structured
    information about the intent of the speaker.

    :param speech_config: The config for the speech recognizer.
    :param audio_config: The config for the audio input.
    :param intents: Intents from an iterable over pairs of (model, intent_id) or (simple_phrase,
        intent_id) to be recognized.
    """

    IntentsIter = Iterable[Tuple[Union[str, LanguageUnderstandingModel], str]]

    def __init__(self, speech_config: SpeechConfig, audio_config: Optional[AudioConfig] = None,
                 intents: Optional[IntentsIter] = None):

        if not isinstance(speech_config, SpeechConfig):
            raise ValueError('speech_config must be a SpeechConfig instance')

        self._impl = self._get_impl(impl.IntentRecognizer, speech_config, audio_config)

        if intents:
            self.add_intents(intents)

    def add_intents(self, intents_iter: IntentsIter):
        """
        Add intents from an iterable over pairs of (model, intent_id) or (simple_phrase,
        intent_id).

        :param intents: Intents from an iterable over pairs of (model, intent_id) or (simple_phrase,
            intent_id) to be recognized.
        """
        for key, value in intents_iter:
            if isinstance(key, LanguageUnderstandingModel):
                self.add_intent(key._impl, value)
            else:
                self.add_intent(key, value)

    def add_all_intents(self, model: LanguageUnderstandingModel):
        """
        Adds all intents from the specified Language Understanding Model.
        """
        self._impl.add_all_intents(model._impl)

    def add_intent(self, *args):
        """
        Add an intent to the recognizer. There are different ways to do this:

        - `add_intent(simple_phrase)`: Adds a simple phrase that may be spoken by the user,
          indicating a specific user intent.
        - `add_intent(simple_phrase, intent_id)`: Adds a simple phrase that may be spoken by the
          user, indicating a specific user intent. Once recognized, the result's intent id will
          match the id supplied here.
        - `add_intent(model, intent_name)`: Adds a single intent by name from the specified
          :class:`.LanguageUnderstandingModel`.
        - `add_intent(model, intent_name, intent_id)`: Adds a single intent by name from the
          specified :class:`.LanguageUnderstandingModel`.

        :param model: The language understanding model containing the intent.
        :param intent_name: The name of the single intent to be included from the language
            understanding model.
        :param simple_phrase: The phrase corresponding to the intent.
        :param intent_id: A custom id string to be returned in the
          :class:`.IntentRecognitionResult`'s `intent_id` property.
        """
        if isinstance(args[0], LanguageUnderstandingModel):
            args = (args[0]._impl, ) + args[1:]

        self._impl.add_intent(*args)

    def recognize_once(self) -> IntentRecognitionResult:
        """
        Performs recognition in a blocking (synchronous) mode. Returns after a single utterance is
        recognized. The end of a single utterance is determined by listening for silence at the end
        or until a maximum of 15 seconds of audio is processed. The task returns the recognition
        text as result. For long-running multi-utterance recognition, use
        :py:meth:`.start_continuous_recognition_async` instead.

        :return: The result value of the synchronous recognition.
        """
        return IntentRecognitionResult(self._impl.recognize_once())

    def recognize_once_async(self) -> ResultFuture:
        """
        Performs recognition in a non-blocking (asynchronous) mode. This will recognize a single
        utterance. The end of a single utterance is determined by listening for silence at the end
        or until a maximum of 15 seconds of audio is processed. For long-running multi-utterance
        recognition, use :py:meth:`.start_continuous_recognition_async` instead.

        :return: A future containing the result value of the asynchronous recognition.
        """
        return ResultFuture(self._impl.recognize_once_async(), IntentRecognitionResult)

    @property
    def recognizing(self) -> EventSignal:
        """
        Signal for events containing intermediate recognition results.

        Callbacks connected to this signal are called with a :class:`.IntentRecognitionEventArgs`,
        instance as the single argument.
        """
        return EventSignal(self._impl.recognizing, IntentRecognitionEventArgs)

    @property
    def recognized(self) -> EventSignal:
        """
        Signal for events containing final recognition results (indicating a successful
        recognition attempt).

        Callbacks connected to this signal are called with a :class:`.IntentRecognitionEventArgs`,
        instance as the single argument, dependent on the type of recognizer.
        """
        return EventSignal(self._impl.recognized, IntentRecognitionEventArgs)

    @property
    def canceled(self) -> EventSignal:
        """
        Signal for events containing canceled recognition results (indicating a recognition attempt
        that was canceled as a result or a direct cancellation request or, alternatively, a
        transport or protocol failure).

        Callbacks connected to this signal are called with a
        :class:`.IntentRecognitionCanceledEventArgs`, instance as the single argument.
        """
        return EventSignal(self._impl.canceled, IntentRecognitionCanceledEventArgs)

