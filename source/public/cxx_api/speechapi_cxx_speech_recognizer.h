//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_speech_recognizer.h: Public API declarations for SpeechRecognizer C++ class
//

#pragma once
#include <exception>
#include <future>
#include <memory>
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_c.h>
#include <speechapi_cxx_recognition_async_recognizer.h>
#include <speechapi_cxx_speech_recognition_eventargs.h>
#include <speechapi_cxx_speech_recognition_result.h>
#include <speechapi_cxx_properties.h>
#include <speechapi_cxx_speech_config.h>
#include <speechapi_cxx_audio_stream.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

class Session;

/// <summary>
/// Class for speech recognizers.
/// </summary>
class SpeechRecognizer final : public AsyncRecognizer<SpeechRecognitionResult, SpeechRecognitionEventArgs, SpeechRecognitionCanceledEventArgs>
{
public:

    using BaseType = AsyncRecognizer<SpeechRecognitionResult, SpeechRecognitionEventArgs, SpeechRecognitionCanceledEventArgs>;

    /// <summary>
    /// Create a speech recognizer from a speech config and audio config.
    /// </summary>
    /// <param name="speechconfig">Speech configuration.</param>
    /// <param name="audioInput">Audio configuration.</param>
    /// <returns>A smart pointer wrapped speech recognizer pointer.</returns>
    static std::shared_ptr<SpeechRecognizer> FromConfig(std::shared_ptr<SpeechConfig> speechconfig, std::shared_ptr<Audio::AudioConfig> audioInput = nullptr)
    {
        SPXRECOHANDLE hreco;
        SPX_THROW_ON_FAIL(::recognizer_create_speech_recognizer_from_config(
            &hreco,
            HandleOrInvalid<SPXAUDIOCONFIGHANDLE,SpeechConfig>(speechconfig),
            HandleOrInvalid<SPXAUDIOCONFIGHANDLE, Audio::AudioConfig>(audioInput)));
        return std::make_shared<SpeechRecognizer>(hreco);
    }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    /// <param name="hreco">Recognizer handle.</param>
    explicit SpeechRecognizer(SPXRECOHANDLE hreco) : BaseType(hreco), Properties(m_properties)
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~SpeechRecognizer()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        TermRecognizer();
    }

    /// <summary>
    /// Performs speech recognition in a non-blocking (asynchronous) mode.
    /// Note: RecognizeOnceAsync() returns when the first utterance has been recognized,
    /// so it is suitable only for single shot recognition like command or query.
    /// For long-running recognition, use StartContinuousRecognitionAsync() instead.
    /// </summary>
    /// <returns>Future containing result value (a shared pointer to IntentRecognitionResult)
    /// of the asynchronous speech recognition.
    /// </returns>
    std::future<std::shared_ptr<SpeechRecognitionResult>> RecognizeOnceAsync() override
    {
        return BaseType::RecognizeOnceAsyncInternal();
    }

    /// <summary>
    /// Asynchronously initiates continuous speech recognition operation.
    /// </summary>
    /// <returns>An empty future.</returns>
    std::future<void> StartContinuousRecognitionAsync() override
    {
        return BaseType::StartContinuousRecognitionAsyncInternal();
    }

    /// <summary>
    /// Asynchronously terminates ongoing continuous speech recognition operation.
    /// </summary>
    /// <returns>An empty future.</returns>
    std::future<void> StopContinuousRecognitionAsync() override
    {
        return BaseType::StopContinuousRecognitionAsyncInternal();
    }

    /// <summary>
    /// Asynchronously initiates keyword recognition operation.
    /// </summary>
    /// Note: Key word spotting functionality is only available on the Cognitive Services Device SDK.This functionality is currently not included in the SDK itself.
    /// <param name="model">Specifies the keyword model to be used.</param>
    /// <returns>An empty future.</returns>
    std::future<void> StartKeywordRecognitionAsync(std::shared_ptr<KeywordRecognitionModel> model) override
    {
        return BaseType::StartKeywordRecognitionAsyncInternal(model);
    }

    /// <summary>
    /// Asynchronously terminates keyword recognition operation.
    /// </summary>
    /// Note: Key word spotting functionality is only available on the Cognitive Services Device SDK.This functionality is currently not included in the SDK itself.
    /// <returns>An empty future.</returns>
    std::future<void> StopKeywordRecognitionAsync() override
    {
        return BaseType::StopKeywordRecognitionAsyncInternal();
    }

    /// <summary>
    /// A collection or properties and their values defined for this <see cref="SpeechRecognizer"/>.
    /// </summary>
    PropertyCollection& Properties;

    /// <summary>
    /// Gets the endpoint ID of a customized speech model that is used for speech recognition.
    /// </summary>
    /// <returns>the endpoint ID of a customized speech model that is used for speech recognition</returns>
    SPXSTRING GetEndpointId()
    {
        return Properties.GetProperty(PropertyId::SpeechServiceConnection_EndpointId, SPXSTRING());
    }

    /// <summary>
    /// Sets the authorization token that will be used for connecting the server.
    /// </summary>
    /// <param name="token">The authorization token.</param>
    void SetAuthorizationToken(const SPXSTRING& token)
    {
        Properties.SetProperty(PropertyId::SpeechServiceAuthorization_Token, token);
    }

    /// <summary>
    /// Gets the authorization token.
    /// </summary>
    /// <returns>Authorization token</returns>
    SPXSTRING GetAuthorizationToken()
    {
        return Properties.GetProperty(PropertyId::SpeechServiceAuthorization_Token, SPXSTRING());
    }

private:
    DISABLE_DEFAULT_CTORS(SpeechRecognizer);
    friend class Microsoft::CognitiveServices::Speech::Session;
};


} } } // Microsoft::CognitiveServices::Speech
