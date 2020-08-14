# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.

import pytest
import json

import azure.cognitiveservices.speech as msspeech

from .utils import (_check_callbacks, _check_sr_result, _wait_for_event)


def test_pronunciation_assessment_config():
    config = msspeech.PronunciationAssessmentConfig('reference', msspeech.PronunciationAssessmentGradingSystem.HundredMark)
    jo = json.loads(config.to_json())
    assert config.reference_text == 'reference'
    assert jo['referenceText'] == config.reference_text
    assert "HundredMark" == jo["gradingSystem"]
    assert "Phoneme" == jo["granularity"]
    assert "Comprehensive" == jo["dimension"]

    config = msspeech.PronunciationAssessmentConfig(reference_text='reference',
                                                    grading_system=msspeech.PronunciationAssessmentGradingSystem.HundredMark,
                                                    granularity=msspeech.PronunciationAssessmentGranularity.Word,
                                                    enable_miscue=True,
                                                    scenario_id="id")
    config.reference_text = "new reference"
    jo = json.loads(config.to_json())
    assert config.reference_text == 'new reference'
    assert jo['referenceText'] == config.reference_text
    assert "HundredMark" == jo["gradingSystem"]
    assert "Word" == jo["granularity"]
    assert "Comprehensive" == jo["dimension"]
    assert jo['enableMiscue']
    assert 'id' == jo['scenarioId']

    config2 = msspeech.PronunciationAssessmentConfig(json_string=config.to_json())
    assert config.to_json() == config2.to_json()

    err_found = None
    try:
        config3 = msspeech.PronunciationAssessmentConfig(json_string='invalid json')
    except RuntimeError as err:
        err_found = err
    assert err_found is not None


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_pronunciation_assessment_config_apply_to(from_file_speech_reco_with_callbacks, speech_input):
    reco, _ = from_file_speech_reco_with_callbacks()
    pron_config = msspeech.PronunciationAssessmentConfig(reference_text="")
    pron_config.apply_to(reco)
    assert pron_config.to_json() == reco.properties.get_property(msspeech.PropertyId.PronunciationAssessment_Params)


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_pronunciation_assessment(from_file_speech_reco_with_callbacks, speech_input):
    reco, callbacks = from_file_speech_reco_with_callbacks()

    pron_config = msspeech.PronunciationAssessmentConfig(reference_text="")
    pron_config.apply_to(reco)

    result = reco.recognize_once()
    _check_sr_result(result, speech_input, 0)
    _wait_for_event(callbacks, 'session_stopped')
    _check_callbacks(callbacks)

    pron_result = msspeech.PronunciationAssessmentResult(result)

    assert pron_result.accuracy_score > 0
    assert pron_result.pronunciation_score > 0
    assert pron_result.completeness_score > 0
    assert pron_result.fluency_score > 0

    assert pron_result.words is not None
    assert pron_result.words[0].accuracy_score > 0
    assert pron_result.words[0].phonemes is not None
    assert pron_result.words[0].phonemes[0].accuracy_score > 0
