//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_intent_recognizer.h: Public API declarations for IntentRecognizer C++ class
//

#pragma once
#include <speechapi_c.h>
#include <speechapi_cxx_recognizer.h>
#include <speechapi_cxx_intent_recognition_result.h>
#include <speechapi_cxx_intent_recognition_eventargs.h>
#include <speechapi_cxx_intent_trigger.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Intent {


class IntentRecognizer : virtual public AsyncRecognizer<IntentRecognitionResult, IntentRecognitionEventArgs>
{
public:

    using BaseType = AsyncRecognizer<IntentRecognitionResult, IntentRecognitionEventArgs>;

    IntentRecognizer(SPXRECOHANDLE hreco = SPXHANDLE_INVALID) : BaseType(hreco) { SPX_DBG_TRACE_FUNCTION(); }
    ~IntentRecognizer() { SPX_DBG_TRACE_FUNCTION(); }

    std::future<std::shared_ptr<IntentRecognitionResult>> RecognizeAsync() override { return BaseType::RecognizeAsyncInternal(); }
    std::future<void> StartContinuousRecognitionAsync() override { return BaseType::StartContinuousRecognitionAsyncInternal(); }
    std::future<void> StopContinuousRecognitionAsync() override { return BaseType::StopContinuousRecognitionAsyncInternal(); }

    std::future<void> StartKeywordRecognitionAsync(const wchar_t* keyword) override { return BaseType::StartKeywordRecognitionAsyncInternal(keyword); }
    std::future<void> StopKeywordRecognitionAsync() override { return BaseType::StopKeywordRecognitionAsyncInternal(); }

    void AddIntent(const wchar_t* intentId, const wchar_t* simplePhrase)
    {
        auto trigger = IntentTrigger::From(simplePhrase);
        return AddIntent(intentId, trigger);
    }

    void AddIntent(const wchar_t* intentId, std::shared_ptr<IntentTrigger> trigger)
    {
        SPX_THROW_ON_FAIL(IntentRecognizer_AddIntent(m_hreco, intentId, (SPXTRIGGERHANDLE)(*trigger.get())));
    }


private:

    IntentRecognizer(IntentRecognizer&&) = delete;
    IntentRecognizer(const IntentRecognizer&) = delete;
    IntentRecognizer& operator=(IntentRecognizer&&) = delete;
    IntentRecognizer& operator=(const IntentRecognizer&) = delete;

    friend class CARBON_NAMESPACE_ROOT::Session;
};


} } } // CARBON_NAMESPACE_ROOT :: Recognition :: Intent
