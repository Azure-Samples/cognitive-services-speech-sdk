#  Copyright (c) Microsoft. All rights reserved.
#  See https://aka.ms/csspeech/license201809 for the full license information.

from . import speech_py_impl as impl
from .speech import SpeechConfig, Recognizer, AudioConfig

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
        if subscription is None and region is None and app_id is None and endpoint is None:
            raise ValueError(
                'bad arguments: either an endpoint or an app id, with optional subscription key & region, must be given')

        if (sum(val is not None for val in (subscription, region)) == 1 or (
                app_id is None and subscription is not None and region is not None)):
            raise ValueError('all of subscription key, api id and region must be given to initialize from subscription')

        if app_id is not None and endpoint is not None:
            raise ValueError('bad arguments: either an endpoint or an app id, with optional subscription key & region, must be given')

        if app_id is not None:
            self._impl = impl.LanguageUnderstandingModel._from_app_id(app_id)
        elif app_id is not None and subscription is not None and region is not None:
            self._impl = impl.LanguageUnderstandingModel._from_subscription(subscription, app_id, region)
        elif endpoint is not None:
            self._impl = impl.LanguageUnderstandingModel._from_endpoint(endpoint)
        else:
            raise ValueError('cannot construct LanguageUnderstandingModel')


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
        :param intent_name: The name of the single intent to be included from the language understanding model.
        :param simple_phrase: The phrase corresponding to the intent.
        :param intent_id: A custom id string to be returned in the
            :class:`.IntentRecognitionResult`'s `intent_id` property.
        """
        if isinstance(args[0], LanguageUnderstandingModel):
            args = (args[0]._impl, ) + args[1:]

        self._impl.add_intent(*args)

