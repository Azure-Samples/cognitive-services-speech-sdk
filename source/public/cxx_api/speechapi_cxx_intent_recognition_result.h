//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_intent_recognition_result.h: Public API declarations for IntentRecognitionResult C++ class
//

#pragma once
#include <string>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Intent {


class IntentRecognitionResult final : public RecognitionResult
{
public:

    IntentRecognitionResult(SPXRESULTHANDLE hresult) :
        RecognitionResult(hresult),
        IntentId(m_intentId)
    {
        PopulateIntentFields(hresult, &m_intentId);
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%x, handle=0x%x) -- resultid=%S; reason=0x%x; text=%S", __FUNCTION__, this, Handle, ResultId.c_str(), Reason, Text.c_str());
    }

    ~IntentRecognitionResult()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, Handle);
    }

    const std::wstring& IntentId;

private:
    DISABLE_DEFAULT_CTORS(IntentRecognitionResult);

    void PopulateIntentFields(SPXRESULTHANDLE hresult, std::wstring* pintentId)
    {
        SPX_INIT_HR(hr);
        
        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount+1];

        if (pintentId != nullptr)
        {
            SPX_THROW_ON_FAIL(hr = IntentResult_GetIntentId(hresult, sz, maxCharCount));
            *pintentId = sz;
        }
    }

    std::wstring m_intentId;
};


} } } // CARBON_NAMESPACE_ROOT :: Recognition :: Intent
