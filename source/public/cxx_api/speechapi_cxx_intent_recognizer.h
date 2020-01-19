//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_intent_recognizer.h: Public API declarations for IntentRecognizer C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_c.h>
#include <speechapi_cxx_recognizer.h>
#include <speechapi_cxx_intent_recognition_result.h>
#include <speechapi_cxx_intent_recognition_eventargs.h>
#include <speechapi_cxx_intent_trigger.h>
#include <speechapi_cxx_properties.h>
#include <speechapi_cxx_speech_config.h>
#include <speechapi_cxx_audio_stream.h>
#include <spxdebug.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Intent {

/// <summary>
/// In addition to performing speech-to-text recognition, the IntentRecognizer extracts structured information
/// about the intent of the speaker, which can be used to drive further actions using dedicated intent triggers
/// (see <see cref="IntentTrigger"/>).
/// </summary>
class IntentRecognizer : public AsyncRecognizer<IntentRecognitionResult, IntentRecognitionEventArgs, IntentRecognitionCanceledEventArgs>
{
public:

    using BaseType = AsyncRecognizer<IntentRecognitionResult, IntentRecognitionEventArgs, IntentRecognitionCanceledEventArgs>;

     /// <summary>
     /// Creates an intent recognizer from a speech config and an audio config.
     /// Users should use this function to create a new instance of an intent recognizer.
     /// </summary>
    /// <param name="speechConfig">Speech configuration. </param>
    /// <param name="audioInput">Audio configuration. </param>
    /// <returns>Instance of intent recognizer.</returns>
    static std::shared_ptr<IntentRecognizer> FromConfig(std::shared_ptr<SpeechConfig> speechConfig, std::shared_ptr<Audio::AudioConfig> audioInput = nullptr)
    {
        SPXRECOHANDLE hreco;
        SPX_THROW_ON_FAIL(::recognizer_create_intent_recognizer_from_config(
            &hreco,
            HandleOrInvalid<SPXSPEECHCONFIGHANDLE, SpeechConfig>(speechConfig),
            HandleOrInvalid<SPXAUDIOCONFIGHANDLE, Audio::AudioConfig>(audioInput)));
        return std::make_shared<IntentRecognizer>(hreco);
    }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    /// <param name="hreco">Recognizer handle.</param>
    explicit IntentRecognizer(SPXRECOHANDLE hreco) : BaseType(hreco), Properties(m_properties)
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    /// <summary>
    /// destructor
    /// </summary>
    ~IntentRecognizer()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        TermRecognizer();
    }

    /// <summary>
    /// Starts intent recognition, and returns after a single utterance is recognized. The end of a
    /// single utterance is determined by listening for silence at the end or until a maximum of 15
    /// seconds of audio is processed.  The task returns the recognition text as result.
    /// Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    /// shot recognition like command or query.
    /// For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead..
    /// </summary>
    /// <returns>Future containing result value (a shared pointer to IntentRecognitionResult)
    /// of the asynchronous intent recognition.
    /// </returns>
    std::future<std::shared_ptr<IntentRecognitionResult>> RecognizeOnceAsync() override
    {
        return BaseType::RecognizeOnceAsyncInternal();
    }

    /// <summary>
    /// Asynchronously initiates continuous intent recognition operation.
    /// </summary>
    /// <returns>An empty future.</returns>
    std::future<void> StartContinuousRecognitionAsync() override
    {
        return BaseType::StartContinuousRecognitionAsyncInternal();
    }

    /// <summary>
    /// Asynchronously terminates ongoing continuous intent recognition operation.
    /// </summary>
    /// <returns>An empty future.</returns>
    std::future<void> StopContinuousRecognitionAsync() override
    {
        return BaseType::StopContinuousRecognitionAsyncInternal();
    }

    /// <summary>
    /// Asynchronously initiates keyword recognition operation.
    /// </summary>
    /// Note: Keyword spotting (KWS) functionality might work with any microphone type, official KWS support, however, is currently limited to the microphone arrays found in the Azure Kinect DK hardware or the Speech Devices SDK.
    /// <param name="model">Specifies the keyword model to be used.</param>
    /// <returns>An empty future.</returns>
    std::future<void> StartKeywordRecognitionAsync(std::shared_ptr<KeywordRecognitionModel> model) override
    {
        return BaseType::StartKeywordRecognitionAsyncInternal(model);
    }

    /// <summary>
    /// Asynchronously terminates keyword recognition operation.
    /// </summary>
    /// Note: Keyword spotting (KWS) functionality might work with any microphone type, official KWS support, however, is currently limited to the microphone arrays found in the Azure Kinect DK hardware or the Speech Devices SDK.
    /// <returns>An empty future.</returns>
    std::future<void> StopKeywordRecognitionAsync() override
    {
        return BaseType::StopKeywordRecognitionAsyncInternal();
    }

    /// <summary>
    /// A collection of properties and their values defined for this <see cref="IntentRecognizer"/>.
    /// </summary>
    PropertyCollection& Properties;

    /// <summary>
    /// Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
    /// </summary>
    /// <param name="simplePhrase">The phrase corresponding to the intent.</param>
    /// <remarks>Once recognized, the IntentRecognitionResult's IntentId property will match the simplePhrase specified here.</remarks>
    void AddIntent(const SPXSTRING& simplePhrase)
    {
        auto trigger = IntentTrigger::From(simplePhrase);
        return AddIntent(trigger, simplePhrase);
    }

    /// <summary>
    /// Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
    /// </summary>
    /// <param name="simplePhrase">The phrase corresponding to the intent.</param>
    /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
    /// <remarks>Once recognized, the result's intent id will match the id supplied here.</remarks>
    void AddIntent(const SPXSTRING& simplePhrase, const SPXSTRING& intentId)
    {
        auto trigger = IntentTrigger::From(simplePhrase);
        return AddIntent(trigger, intentId);
    }

    /// <summary>
    /// Adds a single intent by name from the specified Language Understanding Model.
    /// </summary>
    /// <param name="model">The language understanding model containing the intent.</param>
    /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
    /// <remarks>Once recognized, the IntentRecognitionResult's IntentId property will contain the intentName specified here.</remarks>
    void AddIntent(std::shared_ptr<LanguageUnderstandingModel> model, const SPXSTRING& intentName)
    {
        auto trigger = IntentTrigger::From(model, intentName);
        AddIntent(trigger, intentName);
    }

    /// <summary>
    /// Adds a single intent by name from the specified Language Understanding Model.
    /// </summary>
    /// <param name="model">The language understanding model containing the intent.</param>
    /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
    /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
    void AddIntent(std::shared_ptr<LanguageUnderstandingModel> model, const SPXSTRING& intentName, const SPXSTRING& intentId)
    {
        auto trigger = IntentTrigger::From(model, intentName);
        AddIntent(trigger, intentId);
    }

    /// <summary>
    /// Adds all intents from the specified Language Understanding Model.
    /// </summary>
    /// <param name="model">The language understanding model containing the intents.</param>
    /// <remarks>Once recognized, the IntentRecognitionResult's IntentId property will contain the name of the intent recognized.</remarks>
    void AddAllIntents(std::shared_ptr<LanguageUnderstandingModel> model)
    {
        auto trigger = IntentTrigger::From(model);
        AddIntent(trigger, SPXSTRING_EMPTY);
    }

    /// <summary>
    /// Adds all intents from the specified Language Understanding Model.
    /// </summary>
    /// <param name="model">The language understanding model containing the intents.</param>
    /// <param name="intentId">A custom string id to be returned in the IntentRecognitionResult's IntentId property.</param>
    void AddAllIntents(std::shared_ptr<LanguageUnderstandingModel> model, const SPXSTRING& intentId)
    {
        auto trigger = IntentTrigger::From(model);
        AddIntent(trigger, intentId);
    }

    /// <summary>
    /// Adds the IntentTrigger specified.
    /// </summary>
    /// <param name="trigger">The IntentTrigger corresponding to the intent.</param>
    /// <param name="intentId">A custom string id to be returned in the IntentRecognitionResult's IntentId property.</param>
    void AddIntent(std::shared_ptr<IntentTrigger> trigger, const SPXSTRING& intentId)
    {
        SPX_THROW_ON_FAIL(intent_recognizer_add_intent(m_hreco, Utils::ToUTF8(intentId).c_str(), (SPXTRIGGERHANDLE)(*trigger.get())));
    }

    /// <summary>
    /// Sets the authorization token that will be used for connecting to the service.
    /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
    /// expires, the caller needs to refresh it by calling this setter with a new valid token.
    /// Otherwise, the recognizer will encounter errors during recognition.
    /// </summary>
    /// <param name="token">A string that represents the authorization token.</param>
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

    DISABLE_COPY_AND_MOVE(IntentRecognizer);

    friend class Microsoft::CognitiveServices::Speech::Session;
};


} } } } // Microsoft::CognitiveServices::Speech::Intent
