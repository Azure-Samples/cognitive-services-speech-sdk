import pytest

import azure.cognitiveservices.speech as msspeech

from .utils import (_TestCallback, _connect_all_callbacks, _setup_callbacks, _check_callbacks,
                    _check_sr_result)


speech_config_types = (msspeech.SpeechConfig, msspeech.translation.SpeechTranslationConfig)
recognizer_types = (msspeech.SpeechRecognizer, msspeech.translation.TranslationRecognizer,
                    msspeech.intent.IntentRecognizer)


@pytest.mark.parametrize('speech_input,', ['weather', 'lamp'], indirect=True)
def test_recognize_once(subscription, speech_input, endpoint, speech_region):
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)
    speech_config = msspeech.SpeechConfig(subscription=subscription, region=speech_region)

    reco = msspeech.SpeechRecognizer(speech_config, audio_config)
    callbacks = _setup_callbacks()
    _connect_all_callbacks(reco, callbacks)

    result = reco.recognize_once()
    _check_sr_result(result, speech_input, 0)
    _check_callbacks(callbacks)


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_recognize_async(subscription, speech_input, endpoint, speech_region):
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)
    speech_config = msspeech.SpeechConfig(subscription=subscription, region=speech_region)

    reco = msspeech.SpeechRecognizer(speech_config, audio_config)
    callbacks = _setup_callbacks()
    _connect_all_callbacks(reco, callbacks)

    future = reco.recognize_once_async()
    result = future.get()

    _check_sr_result(result, speech_input, 0)
    _check_callbacks(callbacks)

    # verify that the second call to get from future fails orderly
    with pytest.raises(RuntimeError) as excinfo:
        future.get()

        assert "no state" == excinfo.value


@pytest.mark.xfail(reason='flaky')
@pytest.mark.parametrize('speech_input,', ['batman'], indirect=True)
def test_recognize_once_multiple(subscription, speech_input, endpoint, speech_region):
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)
    speech_config = msspeech.SpeechConfig(subscription=subscription, region=speech_region)

    reco = msspeech.SpeechRecognizer(speech_config, audio_config)

    for utterance_index in range(2):
        result = reco.recognize_once()
        _check_sr_result(result, speech_input, utterance_index)


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_multiple_callbacks(subscription, speech_input, endpoint, speech_region):
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)

    speech_config = msspeech.SpeechConfig(subscription=subscription, region=speech_region)
    reco = msspeech.SpeechRecognizer(speech_config, audio_config)

    callbacks = _setup_callbacks()
    _connect_all_callbacks(reco, callbacks)
    # connect a second callback to two signals
    other_session_started_cb = _TestCallback('In OTHER session_started callback')
    other_recognized_cb = _TestCallback('In OTHER recognized callback')
    reco.session_started.connect(other_session_started_cb)
    reco.recognized.connect(other_recognized_cb)

    result = reco.recognize_once()
    _check_sr_result(result, speech_input, 0)

    _check_callbacks(callbacks)
    assert other_recognized_cb.num_calls == 1


def test_config_construction_kwargs():
    speech_config = msspeech.SpeechConfig(subscription="somesubscription", region="someregion")
    assert speech_config


@pytest.mark.parametrize("config_type", speech_config_types)
def test_speech_config_default_constructor(config_type):
    # from_subscription
    speech_config = config_type(subscription="somesubscription", region="someregion")
    assert speech_config
    assert "somesubscription" == speech_config.subscription_key
    assert "someregion" == speech_config.region

    # from_subscription, positional args
    speech_config = config_type("somesubscription", "someregion")
    assert speech_config
    assert "somesubscription" == speech_config.subscription_key
    assert "someregion" == speech_config.region

    # from_authorization_token
    speech_config = config_type(auth_token="sometoken", region="someregion")
    assert speech_config
    assert "" == speech_config.subscription_key
    assert "sometoken" == speech_config.authorization_token
    assert "someregion" == speech_config.region

    # from_endpoint
    speech_config = config_type(endpoint="someendpoint", subscription="somesubscription")
    assert speech_config
    assert "somesubscription" == speech_config.subscription_key
    assert "" == speech_config.region
    assert "someendpoint" == speech_config.get_property(msspeech.PropertyId.SpeechServiceConnection_Endpoint)

    # from_subscription, with language
    speech_config = config_type(subscription="somesubscription", region="someregion",
                                speech_recognition_language='zh-CN')
    assert speech_config
    assert "somesubscription" == speech_config.subscription_key
    assert "someregion" == speech_config.region
    assert "zh-CN" == speech_config.speech_recognition_language

    with pytest.raises(ValueError) as excinfo:
        speech_config = config_type(subscription="somesubscription")

        assert '"region" needs to be provided' in str(excinfo.value)

    with pytest.raises(ValueError) as excinfo:
        speech_config = config_type(endpoint="someendpoint", region="someregion")

        assert 'cannot construct SpeechConfig with both region and endpoint information' in str(excinfo.value)

    with pytest.raises(ValueError) as excinfo:
        speech_config = config_type()

        assert 'insufficient information' in str(excinfo.value)


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_canceled_result(speech_input):
    invalid_cfg = msspeech.SpeechConfig(endpoint="invalid", subscription="invalid")
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)

    reco = msspeech.SpeechRecognizer(invalid_cfg, audio_config)

    result = reco.recognize_once_async().get()

    assert msspeech.ResultReason.Canceled == result.reason

    cancellation_details = result.cancellation_details
    assert msspeech.CancellationReason.Error == cancellation_details.reason
    assert 'Runtime error: Failed to create transport request.' == cancellation_details.error_details


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_bad_language_config(subscription, speech_input, speech_region):
    bad_lang_cfg = msspeech.SpeechConfig(subscription=subscription, region=speech_region)
    bad_lang_cfg.speech_recognition_language = "bad language"
    assert "bad language" == bad_lang_cfg.speech_recognition_language
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)

    reco = msspeech.SpeechRecognizer(bad_lang_cfg, audio_config)

    result = reco.recognize_once_async().get()

    assert msspeech.ResultReason.Canceled == result.reason

    cancellation_details = result.cancellation_details
    assert msspeech.CancellationReason.Error == cancellation_details.reason
    assert 'WebSocket Upgrade failed with HTTP status code: 505' == cancellation_details.error_details


@pytest.mark.parametrize('speech_input,', ['silence'], indirect=True)
def test_no_match_result(subscription, speech_input, speech_region):
    speech_config = msspeech.SpeechConfig(subscription=subscription, region=speech_region)
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)

    reco = msspeech.SpeechRecognizer(speech_config, audio_config)

    result = reco.recognize_once_async().get()

    assert msspeech.ResultReason.NoMatch == result.reason
    assert result.cancellation_details is None
    details = result.no_match_details
    assert msspeech.NoMatchReason.InitialSilenceTimeout == details.reason


def test_speech_config_properties(subscription, speech_region):
    speech_config = msspeech.SpeechConfig(subscription=subscription, region=speech_region)

    assert subscription == speech_config.subscription_key
    assert "" == speech_config.authorization_token
    assert "" == speech_config.endpoint_id
    assert speech_region == speech_config.region
    assert "" == speech_config.speech_recognition_language

    assert isinstance(speech_config.output_format, msspeech.OutputFormat)
    assert speech_config.output_format == msspeech.OutputFormat.Simple


@pytest.mark.parametrize('speech_input,', ['silence'], indirect=True)
def test_speech_recognizer_default_constructor(speech_input):
    """test that the default argument for AudioConfig is correct"""
    speech_config = msspeech.SpeechConfig(subscription="subscription", region="some_region")
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)

    reco = msspeech.SpeechRecognizer(speech_config, audio_config)
    assert isinstance(reco, msspeech.SpeechRecognizer)


@pytest.mark.parametrize('speech_input,', ['silence'], indirect=True)
def test_translation_recognizer_default_constructor(speech_input):
    """test that the default argument for AudioConfig is correct"""
    speech_config = msspeech.translation.SpeechTranslationConfig(subscription="subscription", region="some_region")
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)

    speech_config.speech_recognition_language = "en-US"
    speech_config.add_target_language('de')

    reco = msspeech.translation.TranslationRecognizer(speech_config, audio_config)
    assert isinstance(reco, msspeech.translation.TranslationRecognizer)

    assert "" == reco.authorization_token
    reco.authorization_token = "mytoken"
    assert "mytoken" == reco.authorization_token

    assert isinstance(reco.properties, msspeech.PropertyCollection)


@pytest.mark.parametrize('speech_input,', ['silence'], indirect=True)
def test_speech_recognizer_properties(speech_input):
    speech_config = msspeech.SpeechConfig(endpoint="myendpoint", subscription="mysubscription")
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)

    assert "mysubscription" == speech_config.subscription_key
    assert "myendpoint" == speech_config.get_property(msspeech.PropertyId.SpeechServiceConnection_Endpoint)

    reco = msspeech.SpeechRecognizer(speech_config, audio_config)

    assert "myendpoint" == reco.properties.get_property(msspeech.PropertyId.SpeechServiceConnection_Endpoint)

    assert "" == reco.authorization_token
    reco.authorization_token = "mytoken"
    assert "mytoken" == reco.authorization_token

    assert reco.endpoint_id == ""


def test_speech_config_properties_setters():
    speech_config = msspeech.SpeechConfig(subscription="some_key", region="some_region")

    speech_config.speech_recognition_language = "de_de"
    assert "de_de" == speech_config.speech_recognition_language

    speech_config.endpoint_id = "x"
    assert "x" == speech_config.endpoint_id

    speech_config.authorization_token = "x"
    assert "x" == speech_config.authorization_token

    with pytest.raises(AttributeError) as excinfo:
        speech_config.region = "newregion"

        assert 'can\'t set attribute ' in str(excinfo.value)


def test_speech_config_properties_output_format_setters():
    speech_config = msspeech.SpeechConfig(subscription="some_key", region="some_region")

    speech_config.output_format = msspeech.OutputFormat.Simple
    assert msspeech.OutputFormat.Simple == speech_config.output_format

    speech_config.output_format = msspeech.OutputFormat.Detailed
    assert msspeech.OutputFormat.Detailed == speech_config.output_format

    with pytest.raises(TypeError) as excinfo:
        speech_config.output_format = 0

        assert 'wrong type, must be OutputFormat' == str(excinfo.value)


def test_speech_config_property_ids():
    speech_config = msspeech.SpeechConfig(subscription="some_key", region="some_region")

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


def test_speech_config_set_properties():
    speech_config = msspeech.SpeechConfig(subscription="some_key", region="some_region")

    properties = {
            msspeech.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse: 'true',
            msspeech.PropertyId.SpeechServiceConnection_Endpoint: 'myendpoint'}

    speech_config.set_properties(properties)

    assert "true" == speech_config.get_property(msspeech.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse)
    assert "myendpoint" == speech_config.get_property(msspeech.PropertyId.SpeechServiceConnection_Endpoint)

    with pytest.raises(TypeError) as excinfo:
        speech_config.set_properties({1000: "bad_value"})

        assert 'wrong type, must be PropertyId' == str(excinfo.value)


@pytest.mark.skip(reason="keyword recognition is not implemented")
def test_keyword_recognition_model_constructor():
    model = msspeech.KeywordRecognitionModel(__file__)
    assert model

    with pytest.raises(ValueError) as excinfo:
        model = msspeech.KeywordRecognitionModel()

        assert "filename needs to be provided" == str(excinfo.value)


@pytest.mark.parametrize('speech_input,', ['silence'], indirect=True)
@pytest.mark.parametrize("recognizer_type", recognizer_types)
def test_speech_recognizer_constructor(speech_input, recognizer_type):
    if recognizer_type is msspeech.translation.TranslationRecognizer:
        speech_config = msspeech.translation.SpeechTranslationConfig(subscription='somesubscription', region='someregion')
        speech_config.speech_recognition_language = 'en-US'
        speech_config.add_target_language('de')
    else:
        speech_config = msspeech.SpeechConfig('somesubscription', 'someregion')

    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)

    with pytest.raises(TypeError) as excinfo:
        reco = recognizer_type()

    with pytest.raises(ValueError) as excinfo:
        reco = recognizer_type(None)

        assert "must be a {} instance".format(type(recognizer_type)) in str(excinfo.value)

    reco = recognizer_type(speech_config, audio_config)
    assert "somesubscription" == reco.properties.get_property(msspeech.PropertyId.SpeechServiceConnection_Key)
    assert "someregion" == reco.properties.get_property(msspeech.PropertyId.SpeechServiceConnection_Region)


@pytest.mark.skipif(pytest.config.option.no_use_default_microphone,
                    reason="requires default microphone")
@pytest.mark.parametrize("recognizer_type", recognizer_types)
def test_speech_recognizer_constructor_default_microphone(recognizer_type):
    if recognizer_type is msspeech.translation.TranslationRecognizer:
        speech_config = msspeech.translation.SpeechTranslationConfig(subscription='somesubscription', region='someregion')
        speech_config.speech_recognition_language = 'en-US'
        speech_config.add_target_language('de')
    else:
        speech_config = msspeech.SpeechConfig('somesubscription', 'someregion')

    reco = recognizer_type(speech_config)
    assert "somesubscription" == reco.properties.get_property(msspeech.PropertyId.SpeechServiceConnection_Key)
    assert "someregion" == reco.properties.get_property(msspeech.PropertyId.SpeechServiceConnection_Region)


@pytest.mark.parametrize("config_type", speech_config_types)
def test_speech_config_set_proxy(config_type):
    config = config_type("subscription", "region")
    config.set_proxy("hostname", 8888, "username", "very_secret123")

    assert "hostname" == config.get_property(msspeech.PropertyId.SpeechServiceConnection_ProxyHostName)
    assert "8888" == config.get_property(msspeech.PropertyId.SpeechServiceConnection_ProxyPort)
    assert "username" == config.get_property(msspeech.PropertyId.SpeechServiceConnection_ProxyUserName)
    assert "very_secret123" == config.get_property(msspeech.PropertyId.SpeechServiceConnection_ProxyPassword)

