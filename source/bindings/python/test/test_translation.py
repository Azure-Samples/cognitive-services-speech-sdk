import pytest
import azure.cognitiveservices.speech as msspeech

from .conftest import SpeechInput
from .utils import _check_translation_result

@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_translation_simple(subscription: str, speech_input: SpeechInput, endpoint: str,
        speech_region: str):
    translation_config = msspeech.translation.SpeechTranslationConfig(subscription=subscription,
            region=speech_region)
    translation_config.speech_recognition_language = speech_input.input_language
    for language in speech_input.translations:
        translation_config.add_target_language(language)

    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)

    recognizer = msspeech.translation.TranslationRecognizer(translation_config, audio_config)

    result = recognizer.recognize_once()
    _check_translation_result(result, speech_input, 0, speech_input.translations.keys())


def test_translation_config_constructor():
    translation_config = msspeech.translation.SpeechTranslationConfig('somesubscription', 'someregion',
            target_languages=['en', 'de', 'fr'], voice_name='de-DE-Hedda')

    assert set(translation_config.target_languages) == set(['en', 'de', 'fr'])
    assert translation_config.voice_name == 'de-DE-Hedda'

    translation_config = msspeech.translation.SpeechTranslationConfig('somesubscription', 'someregion',
            speech_recognition_language='nb-NO')

    assert set(translation_config.target_languages) == set()
    assert translation_config.voice_name == ''
    assert translation_config.speech_recognition_language == 'nb-NO'


@pytest.mark.skip('not implemented')
def test_translation_synthesis_result():
    # test the conversion to msspeech.TranslationSynthesisResult
    pass


def test_speech_translation_config():
    speech_config = msspeech.translation.SpeechTranslationConfig(subscription="subscription", region="some_region")

    speech_config.set_property(msspeech.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse, 'mytext')
    assert "mytext" == speech_config.get_property(msspeech.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse)

    speech_config.set_property(msspeech.PropertyId.SpeechServiceConnection_Endpoint, 'mytext')
    assert "mytext" == speech_config.get_property(msspeech.PropertyId.SpeechServiceConnection_Endpoint)

    with pytest.raises(TypeError) as excinfo:
        speech_config.set_property(1000, "bad_value")

        assert 'wrong type, must be PropertyId' == str(excinfo.value)

    with pytest.raises(TypeError) as excinfo:
        speech_config.get_property(1000)

        assert 'wrong type, must be PropertyId' == str(excinfo.value)

    speech_config.add_target_language('de-de')
    assert ('de-de', ) == speech_config.target_languages
    speech_config.add_target_language('en-us')
    assert set(['de-de', 'en-us']) == set(speech_config.target_languages)
    assert isinstance(speech_config.target_languages, tuple)

    speech_config.voice_name = "myvoice"
    assert "myvoice" == speech_config.voice_name
    speech_config.voice_name = "myothervoice"
    assert "myothervoice" == speech_config.voice_name

