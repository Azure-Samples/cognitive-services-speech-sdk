//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_speech_recognizer.h: Public API declarations for SpeechRecognizer C++ class
//

#pragma once
#include <exception>
#include <future>
#include <memory>
#include <string>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_recognition_async_recognizer.h>
#include <speechapi_cxx_speech_recognition_eventargs.h>
#include <speechapi_cxx_speech_recognition_result.h>
#include <speechapi_cxx_recognizer_parameter_collection.h>


namespace CARBON_NAMESPACE_ROOT {

class Session;


namespace Recognition {
namespace Speech {


class SpeechRecognizer final : virtual public AsyncRecognizer<SpeechRecognitionResult, SpeechRecognitionEventArgs>
{
public:

    using BaseType = AsyncRecognizer<SpeechRecognitionResult, SpeechRecognitionEventArgs>;

    SpeechRecognizer(SPXRECOHANDLE hreco) : BaseType(hreco), Parameters(hreco) { SPX_DBG_TRACE_FUNCTION(); }
    ~SpeechRecognizer() { SPX_DBG_TRACE_FUNCTION(); }

    std::future<std::shared_ptr<SpeechRecognitionResult>> RecognizeAsync() override { return BaseType::RecognizeAsyncInternal(); }
    std::future<void> StartContinuousRecognitionAsync() override { return BaseType::StartContinuousRecognitionAsyncInternal(); }
    std::future<void> StopContinuousRecognitionAsync() override { return BaseType::StopContinuousRecognitionAsyncInternal(); }

    std::future<void> StartKeywordRecognitionAsync(const wchar_t* keyword) override { return BaseType::StartKeywordRecognitionAsyncInternal(keyword); }
    std::future<void> StopKeywordRecognitionAsync() override { return BaseType::StopKeywordRecognitionAsyncInternal(); }

    RecognizerParameterValueCollection Parameters;

    void SetCustomSpeechModel(const wchar_t* modelId) { Parameters[RecognizerParameter::CustomSpeechModelId] = modelId; }


private:

    SpeechRecognizer() = delete;
    SpeechRecognizer(SpeechRecognizer&&) = delete;
    SpeechRecognizer(const SpeechRecognizer&) = delete;
    SpeechRecognizer& operator=(SpeechRecognizer&&) = delete;
    SpeechRecognizer& operator=(const SpeechRecognizer&) = delete;

    friend class CARBON_NAMESPACE_ROOT::Session;
};


} } } // CARBON_NAMESPACE_ROOT :: Recognition :: Speech
