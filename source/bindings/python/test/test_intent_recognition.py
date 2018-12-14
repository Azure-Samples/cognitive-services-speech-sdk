import pytest
import azure.cognitiveservices.speech as msspeech

from .conftest import IntentInput
from .utils import _check_intent_result


@pytest.mark.parametrize('intent_input,', ['lamp'], indirect=True)
def test_intent_recognition_simple(intent_input: IntentInput, luis_subscription: str,
                                   luis_region: str, language_understanding_app_id: str):
    audio_config = msspeech.AudioConfig(filename=intent_input.path)
    intent_config = msspeech.SpeechConfig(subscription=luis_subscription, region=luis_region)

    intent_recognizer = msspeech.IntentRecognizer(intent_config, audio_config)

    model = msspeech.LanguageUnderstandingModel(app_id=language_understanding_app_id)
    intent_recognizer.add_intent(model, "HomeAutomation.TurnOn")
    intent_recognizer.add_intent(model, "HomeAutomation.TurnOff")

    intent_recognizer.add_intent("This is a test.", "test")
    intent_recognizer.add_intent("Switch the to channel 34.", "34")
    intent_recognizer.add_intent("what's the weather like", "weather")

    result = intent_recognizer.recognize_once()

    _check_intent_result(result, intent_input, 0)


def test_language_understanding_model_constructor():
    with pytest.raises(ValueError) as excinfo:
        lm = msspeech.LanguageUnderstandingModel()
        assert "bad parameters: either an endpoint or an app id, with optional subscription key & region, must be given" == str(excinfo.value)

    with pytest.raises(ValueError) as excinfo:
        lm = msspeech.LanguageUnderstandingModel(endpoint='a', app_id='b')
        assert "bad parameters: either an endpoint or an app id, with optional subscription key & region, must be given" == str(excinfo.value)

    with pytest.raises(ValueError) as excinfo:
        lm = msspeech.LanguageUnderstandingModel(subscription='a')
        assert "all of subscription key, api id and region must be given to initialize from subscription" == str(excinfo.value)

    with pytest.raises(ValueError) as excinfo:
        lm = msspeech.LanguageUnderstandingModel(region='a')
        assert "all of subscription key, api id and region must be given to initialize from subscription" == str(excinfo.value)

    with pytest.raises(ValueError) as excinfo:
        lm = msspeech.LanguageUnderstandingModel(region='a', app_id='b')
        assert "all of subscription key, api id and region must be given to initialize from subscription" == str(excinfo.value)

    with pytest.raises(ValueError) as excinfo:
        lm = msspeech.LanguageUnderstandingModel(subscription='a', app_id='b')
        assert "all of subscription key, api id and region must be given to initialize from subscription" == str(excinfo.value)

    with pytest.raises(ValueError) as excinfo:
        lm = msspeech.LanguageUnderstandingModel('a', 'b')
        assert "all of subscription key, api id and region must be given to initialize from subscription" == str(excinfo.value)

    lm = msspeech.LanguageUnderstandingModel('a', 'b', 'c')
    assert lm
    lm = msspeech.LanguageUnderstandingModel(app_id='a')
    assert lm
    lm = msspeech.LanguageUnderstandingModel(endpoint='wss://example.com/')
    assert lm
    lm = msspeech.LanguageUnderstandingModel('a', 'b', 'c')
    assert lm


@pytest.mark.parametrize('intent_input,', ['lamp'], indirect=True)
def test_intent_recognition_constructor(intent_input: IntentInput):
    simple_intents = [('This is my first intent', 'Intent1'),
                      ('This is my second intent', 'Intent2'),
                      ('This is my third intent', 'Intent3')]

    model = msspeech.LanguageUnderstandingModel('a', 'b', 'c')
    model_intents = [(model, 'ModelIntent1'),
                     (model, 'ModelIntent2'),
                     (model, 'ModelIntent3')]

    audio_config = msspeech.AudioConfig(filename=intent_input.path)
    intent_config = msspeech.SpeechConfig(subscription='a', region='b')
    intent_recognizer = msspeech.IntentRecognizer(intent_config, audio_config)
    intent_recognizer.add_intents(simple_intents)

    audio_config = msspeech.AudioConfig(filename=intent_input.path)
    intent_config = msspeech.SpeechConfig(subscription='a', region='b')
    intent_recognizer = msspeech.IntentRecognizer(intent_config, audio_config)
    intent_recognizer.add_intents(model_intents)

    audio_config = msspeech.AudioConfig(filename=intent_input.path)
    intent_config = msspeech.SpeechConfig(subscription='a', region='b')
    intent_recognizer = msspeech.IntentRecognizer(intent_config, audio_config)
    intent_recognizer.add_intents(simple_intents + model_intents)


@pytest.mark.parametrize('speech_input,', ['silence'], indirect=True)
def test_intent_recognizer_default_constructor(speech_input):
    """test that the default argument for AudioConfig is correct"""
    speech_config = msspeech.SpeechConfig(subscription="subscription", region="some_region")
    audio_config = msspeech.AudioConfig(filename=speech_input.path)

    reco = msspeech.IntentRecognizer(speech_config, audio_config)
    assert isinstance(reco, msspeech.IntentRecognizer)

    assert "" == reco.authorization_token
    reco.authorization_token = "mytoken"
    assert "mytoken" == reco.authorization_token

    assert isinstance(reco.properties, msspeech.PropertyCollection)

