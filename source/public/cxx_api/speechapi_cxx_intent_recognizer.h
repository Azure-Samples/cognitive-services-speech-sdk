//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_intent_recognizer.h: Public API declarations for IntentRecognizer C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_recognizer.h>
#include <speechapi_cxx_intent_recognition_result.h>
#include <speechapi_cxx_intent_recognition_eventargs.h>
#include <speechapi_cxx_intent_trigger.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Intent {

/// <summary>
/// In addition to performing speech-to-text recognition, the IntentRecognizer extracts structured information
/// about the intent of the speaker, which can be used to drive further actions using dedicated intent triggers
/// (see <see cref="IntentTrigger"/>).
/// </summary>
class IntentRecognizer : public AsyncRecognizer<IntentRecognitionResult, IntentRecognitionEventArgs>
{
public:

    using BaseType = AsyncRecognizer<IntentRecognitionResult, IntentRecognitionEventArgs>;

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit IntentRecognizer(SPXRECOHANDLE hreco) : BaseType(hreco), Parameters(hreco)
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    /// <summary>
    /// Deconstructor.
    /// </summary>
    ~IntentRecognizer()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        TermRecognizer();
    }

    /// <summary>
    /// Performs intent recognition in a non-blocking (asynchronous) mode.
    /// </summary>
    /// <returns>Future containing result value (a shared pointer to IntentRecognitionResult)
    /// of the asynchronous intent recognition.
    /// </returns>
    std::future<std::shared_ptr<IntentRecognitionResult>> RecognizeAsync() override
    {
        return BaseType::RecognizeAsyncInternal();
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
    /// A collection of parameter names and their values.
    /// </summary>
    RecognizerParameterValueCollection Parameters;

    /// <summary>
    /// Adds a phrase that should be recognized as intent with the specified id.
    /// </summary>
    /// <param name="intentId">Id of the intent.</param>
    /// <param name="phrase">The phrase corresponding to the intent.</param>
    void AddIntent(const std::wstring& intentId, const std::wstring& simplePhrase)
    {
        auto trigger = IntentTrigger::From(simplePhrase);
        return AddIntent(intentId, trigger);
    }

    /// <summary>
    /// Adds all intents from Language Understanding service for recognition that will be recognized as intent with the specified id.
    /// </summary>
    /// <param name="intentId">A string that represents the identifier for model to be recognized.</param>
    /// <param name="model">The language understanding model from Language Understanding service.</param>
    void AddIntent(const std::wstring& intentId, std::shared_ptr<LanguageUnderstandingModel> model)
    {
        auto trigger = IntentTrigger::From(model);
        AddIntent(intentId, trigger);
    }

    /// <summary>
    /// Adds an intent from Language Understanding service for recognition that will be recognized as intent with the specified id.
    /// </summary>
    /// <param name="intentId">A string that represents the identifier of the intent to be recognized.</param>
    /// <param name="model">The language understanding model from Language Understanding service.</param>
    /// <param name="intentName">The intent name defined in the language understanding model.</param>
    void AddIntent(const std::wstring& intentId, std::shared_ptr<LanguageUnderstandingModel> model, const std::wstring& intentName)
    {
        auto trigger = IntentTrigger::From(model, intentName);
        AddIntent(intentId, trigger);
    }

    /// <summary>
    /// Adds an IntentTrigger that should be recognized as intent with the specified id.
    /// </summary>
    /// <param name="intentId">Id of the intent.</param>
    /// <param name="trigger">The IntentTrigger corresponding to the intent.</param>
    void AddIntent(const std::wstring& intentId, std::shared_ptr<IntentTrigger> trigger)
    {
        SPX_THROW_ON_FAIL(IntentRecognizer_AddIntent(m_hreco, intentId.c_str(), (SPXTRIGGERHANDLE)(*trigger.get())));
    }



private:
    DISABLE_COPY_AND_MOVE(IntentRecognizer);
    //DISABLE_DEFAULT_CTORS(IntentRecognizer);

    friend class Microsoft::CognitiveServices::Speech::Session;
};


} } } } // Microsoft::CognitiveServices::Speech::Intent
