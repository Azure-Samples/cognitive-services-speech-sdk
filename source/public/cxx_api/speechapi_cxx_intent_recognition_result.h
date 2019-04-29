//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_intent_recognition_result.h: Public API declarations for IntentRecognitionResult C++ class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_cxx_recognition_result.h>
#include <speechapi_c.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Intent {

/// <summary>
/// Represents the result of an intent recognition.
/// </summary>
class IntentRecognitionResult final : public RecognitionResult
{
public:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    /// <param name="hresult">Result handle.</param>
    explicit IntentRecognitionResult(SPXRESULTHANDLE hresult) :
        RecognitionResult(hresult),
        IntentId(m_intentId)
    {
        PopulateIntentFields(hresult, &m_intentId);
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%p, handle=0x%p) -- resultid=%s; reason=0x%x; text=%s", __FUNCTION__, (void*)this, (void*)Handle, Utils::ToUTF8(ResultId).c_str(), Reason, Utils::ToUTF8(Text).c_str());
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~IntentRecognitionResult()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%p, handle=0x%p)", __FUNCTION__, (void*)this, (void*)Handle);
    }

    /// <summary>
    /// Unique intent id.
    /// </summary>
    const SPXSTRING& IntentId;

private:
    DISABLE_DEFAULT_CTORS(IntentRecognitionResult);

    void PopulateIntentFields(SPXRESULTHANDLE hresult, SPXSTRING* pintentId)
    {
        SPX_INIT_HR(hr);

        const size_t maxCharCount = 1024;
        char sz[maxCharCount+1];

        if (pintentId != nullptr && recognizer_result_handle_is_valid(hresult))
        {
            SPX_THROW_ON_FAIL(hr = intent_result_get_intent_id(hresult, sz, maxCharCount));
            *pintentId = Utils::ToSPXString(sz);
        }
    }

    SPXSTRING m_intentId;
};


} } } } // Microsoft::CognitiveServices::Speech::Recognition::Intent
