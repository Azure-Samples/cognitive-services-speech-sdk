import pytest

import azure.cognitiveservices.speech as msspeech

from .utils import (_check_sr_result)
from .utils import (_check_result_common)


@pytest.mark.parametrize('speech_input,', ['beach'], indirect=True)
def test_single_phrase(from_file_speech_reco_with_callbacks, speech_input):
    reco, _ = from_file_speech_reco_with_callbacks()

    # setup PhraseListGrammar
    pg = msspeech.PhraseListGrammar.from_recognizer(reco)

    pg.addPhrase("Wreck a nice beach")

    result = reco.recognize_once()

    # Check results
    _check_sr_result(result, speech_input, 0)


@pytest.mark.parametrize('speech_input,', ['beach_nohelp'], indirect=True)
def test_add_clear(from_file_speech_reco_with_callbacks, speech_input):
    reco, _ = from_file_speech_reco_with_callbacks()

    # setup PhraseListGrammar
    pg = msspeech.PhraseListGrammar.from_recognizer(reco)

    pg.addPhrase("Wreck a nice beach")
    pg.clear()

    result = reco.recognize_once()

    # Check results
    _check_sr_result(result, speech_input, 0)


@pytest.mark.parametrize('intent_input,', ['beach'], indirect=True)
def test_intent_recognition_phraselist(from_file_intent_reco_with_callbacks, intent_input):
    reco, _ = from_file_intent_reco_with_callbacks()

    # setup PhraseListGrammar
    pg = msspeech.PhraseListGrammar.from_recognizer(reco)

    pg.addPhrase("Wreck a nice beach")

    result = reco.recognize_once()

    _check_result_common(result, intent_input, 0)


@pytest.mark.parametrize('speech_input,', ['beach_nohelp_truman'], indirect=True)
def test_speech_recognition_phraselist_clear(from_file_speech_reco_with_callbacks, speech_input):
    reco, _ = from_file_speech_reco_with_callbacks()

    # setup PhraseListGrammar
    pg = msspeech.PhraseListGrammar.from_recognizer(reco)

    pg.addPhrase("Wreck a nice beach")
    pg.clear()

    result = reco.recognize_once()

    _check_result_common(result, speech_input, 0)

