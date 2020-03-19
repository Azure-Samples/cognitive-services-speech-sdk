# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.

import sys
import pytest

import azure.cognitiveservices.speech as msspeech

@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_speech_recognition_with_source_language(speech_input, default_speech_auth, speech_region):
    from azure.cognitiveservices.speech import PropertyId
     # use endpoint if it has been specified
    endpoint = default_speech_auth['endpoint'] or "wss://{}.stt.speech.microsoft.com/speech/" \
            "recognition/interactive/cognitiveservices/v1".format(speech_region)

    speechConfig = msspeech.SpeechConfig(subscription=default_speech_auth['subscription'],
            endpoint=endpoint)
    audioConfig = msspeech.audio.AudioConfig(filename=speech_input.path)
    # Creates a speech recognizer using a file as audio input and specifies source language
    speechLanguage = "de-DE"
    reco = msspeech.SpeechRecognizer(speechConfig, language=speechLanguage, audio_config=audioConfig)
    assert speechLanguage == reco.properties.get_property(PropertyId.SpeechServiceConnection_RecoLanguage)
    reco.recognize_once()    
    connectionUrl = reco.properties.get_property(PropertyId.SpeechServiceConnection_Url)
    assert -1 != connectionUrl.find("language={}".format(speechLanguage))


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_speech_recognition_with_source_language_config(speech_input, default_speech_auth, speech_region):
    from azure.cognitiveservices.speech import PropertyId
     # use endpoint if it has been specified
    endpoint = default_speech_auth['endpoint'] or "wss://{}.stt.speech.microsoft.com/speech/" \
            "recognition/interactive/cognitiveservices/v1".format(speech_region)

    speechConfig = msspeech.SpeechConfig(subscription=default_speech_auth['subscription'],
            endpoint=endpoint)
    audioConfig = msspeech.audio.AudioConfig(filename=speech_input.path)
    # Creates a speech recognizer using a file as audio input and specifies source language config
    speechLanguage = "de-DE"
    sourceLanguageConfig = msspeech.languageconfig.SourceLanguageConfig(speechLanguage)
    reco = msspeech.SpeechRecognizer(speechConfig, source_language_config=sourceLanguageConfig, audio_config=audioConfig)
    assert speechLanguage == reco.properties.get_property(PropertyId.SpeechServiceConnection_RecoLanguage)
    reco.recognize_once()
    connectionUrl = reco.properties.get_property(PropertyId.SpeechServiceConnection_Url)
    assert -1 != connectionUrl.find("language={}".format(speechLanguage))

    # Creates a speech recognizer using a file as audio input and specifies source language config with endpointId
    endpointId="myendpointId"
    sourceLanguageConfig = msspeech.languageconfig.SourceLanguageConfig(speechLanguage, endpointId)
    reco = msspeech.SpeechRecognizer(speechConfig, source_language_config=sourceLanguageConfig, audio_config=audioConfig)
    assert speechLanguage == reco.properties.get_property(PropertyId.SpeechServiceConnection_RecoLanguage)
    assert endpointId == reco.properties.get_property(PropertyId.SpeechServiceConnection_EndpointId)

@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_speech_recognition_with_auto_source_language_config(speech_input):
    from azure.cognitiveservices.speech import PropertyId     
    speechConfig = msspeech.SpeechConfig(subscription="subsription", endpoint="endpoint")
    audioConfig = msspeech.audio.AudioConfig(filename=speech_input.path)

    autoDetectSourceLanguageConfig = msspeech.languageconfig.AutoDetectSourceLanguageConfig(languages=["de-DE", "en-US"])
    reco = msspeech.SpeechRecognizer(speechConfig, auto_detect_source_language_config=autoDetectSourceLanguageConfig, audio_config=audioConfig)
    assert "de-DE,en-US" == reco.properties.get_property(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguages)
    assert "" == reco.properties.get_property_by_name("de-DESPEECH-ModelId")
    assert "" == reco.properties.get_property_by_name("en-USSPEECH-ModelId")
    
    sourceLanguageConfig1 = msspeech.languageconfig.SourceLanguageConfig("de-DE")    
    sourceLanguageConfig2 = msspeech.languageconfig.SourceLanguageConfig("zh-CN", "myendpointId")
    autoDetectSourceLanguageConfig = msspeech.languageconfig.AutoDetectSourceLanguageConfig(sourceLanguageConfigs=[sourceLanguageConfig1, sourceLanguageConfig2])
    reco = msspeech.SpeechRecognizer(speechConfig, auto_detect_source_language_config=autoDetectSourceLanguageConfig, audio_config=audioConfig)
    assert "de-DE,zh-CN" == reco.properties.get_property(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguages)
    assert "" == reco.properties.get_property_by_name("de-DESPEECH-ModelId")
    assert "myendpointId" == reco.properties.get_property_by_name("zh-CNSPEECH-ModelId")
    
def test_creation_source_language_config_bad_parameter(speech_input):
    errFound = None
    try:
        sourceLanguageConfig = msspeech.languageconfig.SourceLanguageConfig("")
    except ValueError as err:
        errFound = err
    assert None != errFound
    assert "language cannot be an empty string" == str(errFound)

    errFound = None
    try:
        sourceLanguageConfig = msspeech.languageconfig.SourceLanguageConfig("en-US", "")
    except ValueError as err:
        errFound = err
    assert None != errFound
    assert "endpointId cannot be an empty string" == str(errFound)

def test_creation_auto_detect_source_language_config_bad_parameter(speech_input):
    errFound = None
    try:
        autoDetectSourceLanguageConfig = msspeech.languageconfig.AutoDetectSourceLanguageConfig()
    except ValueError as err:
        errFound = err
    assert None != errFound
    assert "either languages or sourceLanguageConfigs has to be specified to create AutoDetectSourceLanguageConfig" == str(errFound)

    sourceLanguageConfig1 = msspeech.languageconfig.SourceLanguageConfig("de-DE")
    sourceLanguageConfig2 = msspeech.languageconfig.SourceLanguageConfig("fr-FR", "endpointId1")
    errFound = None
    try:
        autoDetectSourceLanguageConfig = msspeech.languageconfig.AutoDetectSourceLanguageConfig(languages=["en-US", "de-DE"], sourceLanguageConfigs=[sourceLanguageConfig1, sourceLanguageConfig2])
    except ValueError as err:
        errFound = err
    assert None != errFound
    assert "languages and sourceLanguageConfigs cannot be both specified to create AutoDetectSourceLanguageConfig" == str(errFound)

    errFound = None
    try:
        autoDetectSourceLanguageConfig = msspeech.languageconfig.AutoDetectSourceLanguageConfig(languages=[])
    except ValueError as err:
        errFound = err
    assert None != errFound
    assert "languages list cannot be empty" == str(errFound)

    errFound = None
    try:
        autoDetectSourceLanguageConfig = msspeech.languageconfig.AutoDetectSourceLanguageConfig(sourceLanguageConfigs=[])
    except ValueError as err:
        errFound = err
    assert None != errFound
    assert "source language config list cannot be empty" == str(errFound)