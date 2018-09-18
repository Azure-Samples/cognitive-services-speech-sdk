//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_intent_recognition_result.h: Public API declarations for IntentRecognitionResult C++ class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
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
    explicit IntentRecognitionResult(SPXRESULTHANDLE hresult) :
        RecognitionResult(hresult),
        IntentId(m_intentId)
    {
        PopulateIntentFields(hresult, &m_intentId);
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%x, handle=0x%x) -- resultid=%s; reason=0x%x; text=%s", __FUNCTION__, this, Handle, Utils::ToUTF8(ResultId).c_str(), Reason, Utils::ToUTF8(Text).c_str());
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~IntentRecognitionResult()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, Handle);
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
