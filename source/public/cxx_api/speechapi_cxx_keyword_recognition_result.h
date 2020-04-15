//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_keyword_recognition_result.h: Public API declarations for the KeywordRecognitionResult C++ class
//

#pragma once

#include <speechapi_cxx_common.h>
#include <speechapi_cxx_recognition_result.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Class that defines the results emitted by the <see cref="KeywordRecognizer" />.
/// </summary>
class KeywordRecognitionResult : public RecognitionResult
{
public:

    explicit KeywordRecognitionResult(SPXRESULTHANDLE hresult) :
        RecognitionResult(hresult)
    {
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%p, handle=0x%p) -- resultid=%s; reason=0x%x; text=%s", __FUNCTION__, (void*)this, (void*)Handle, Utils::ToUTF8(ResultId).c_str(), Reason, Utils::ToUTF8(Text).c_str());
    }

    virtual ~KeywordRecognitionResult() = default;

private:
    DISABLE_DEFAULT_CTORS(KeywordRecognitionResult);
};

} } } // Microsoft::CognitiveServices::Speech
