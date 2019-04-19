# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.

import pytest
import azure.cognitiveservices.speech as msspeech

from .conftest import SpeechInput
from .utils import _check_translation_result

@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_translation_simple(speech_input: SpeechInput, from_file_translation_reco_with_callbacks):
    recognizer, callbacks = from_file_translation_reco_with_callbacks()

    result = recognizer.recognize_once()
    _check_translation_result(result, speech_input, 0, speech_input.translations.keys())

    # test string representation, ordering of dict entries is nondeterministic
    # TranslationRecognitionResult:(result_id=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx, translations={'de':
    # 'Wie ist das Wetter?', 'fr': 'Quel temps fait-il?'}, reason=ResultReason.TranslatedSpeech)
    from itertools import permutations
    desired_result_strs = []
    translations = list(speech_input.translations.items())
    for perm in permutations(translations):
        permuted_dict_str = '{' + ', '.join("'{}': '{}'".format(k, v) for k, v in perm) + '}'
        desired_result_strs.append(
                'TranslationRecognitionResult(result_id={}, translations={}, '
                'reason=ResultReason.TranslatedSpeech)'.format(
                    result.result_id, permuted_dict_str))

    assert str(result) in desired_result_strs


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


def test_speech_translation_config():
    speech_config = msspeech.translation.SpeechTranslationConfig(subscription="subscription", region="some_region")

    speech_config.set_property(msspeech.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse, 'mytext')
    assert "mytext" == speech_config.get_property(msspeech.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse)

    speech_config.set_property(msspeech.PropertyId.SpeechServiceConnection_Endpoint, 'mytext')
    assert "mytext" == speech_config.get_property(msspeech.PropertyId.SpeechServiceConnection_Endpoint)

    with pytest.raises(TypeError, match='property_id value must be PropertyId instance'):
        speech_config.set_property(1000, "bad_value")

    with pytest.raises(TypeError, match='property_id value must be PropertyId instance'):
        speech_config.get_property(1000)

    speech_config.add_target_language('de-de')
    assert ('de-de', ) == speech_config.target_languages
    speech_config.add_target_language('en-us')
    assert set(['de-de', 'en-us']) == set(speech_config.target_languages)
    assert isinstance(speech_config.target_languages, tuple)

    speech_config.voice_name = "myvoice"
    assert "myvoice" == speech_config.voice_name
    speech_config.voice_name = "myothervoice"
    assert "myothervoice" == speech_config.voice_name

@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_set_service_property(subscription, speech_input, speech_region):
    config = msspeech.translation.SpeechTranslationConfig(subscription=subscription, region=speech_region)
    config.speech_recognition_language = 'en-us'
    config.set_service_property(name='to', value='de', channel=msspeech.ServicePropertyChannel.UriQueryParameter)
    audio_input = msspeech.AudioConfig(filename=speech_input.path)
    translation_recognizer = msspeech.translation.TranslationRecognizer(config, audio_input)
    result = translation_recognizer.recognize_once()
    _check_translation_result(result, speech_input, 0, target_languages=['de'])