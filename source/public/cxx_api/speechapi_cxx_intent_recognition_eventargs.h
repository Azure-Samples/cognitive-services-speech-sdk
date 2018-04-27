//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_intent_recognition_eventargs.h: Public API declarations for IntentRecognitionEventArgs C++ class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_c_common.h>
#include <speechapi_cxx_recognition_eventargs.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Intent {


/// <summary>
/// Class for intent recognition event arguments.
/// </summary>
class IntentRecognitionEventArgs final : public RecognitionEventArgs
{
private:

    SPXEVENTHANDLE m_hevent;
    std::shared_ptr<IntentRecognitionResult> m_result;


public:

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="hevent">Event handle</param>
    explicit IntentRecognitionEventArgs(SPXEVENTHANDLE hevent) :
        RecognitionEventArgs(hevent),
        m_hevent(hevent),
        m_result(std::make_shared<IntentRecognitionResult>(IntentResultHandleFromEventHandle(hevent))),
        Result(*m_result.get())
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
    };

    /// <inheritdoc/>
    virtual ~IntentRecognitionEventArgs()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
        SPX_THROW_ON_FAIL(Recognizer_EventHandle_Close(m_hevent));
    };

    /// <summary>
    /// Intent recognition event result.
    /// </summary>
    const IntentRecognitionResult& Result;

private:

    DISABLE_DEFAULT_CTORS(IntentRecognitionEventArgs);

    SPXRESULTHANDLE IntentResultHandleFromEventHandle(SPXEVENTHANDLE hevent)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(Recognizer_RecognitionEvent_GetResult(hevent, &hresult));
        return hresult;
    }
};


} } } } // Microsoft::CognitiveServices::Speech::Intent
