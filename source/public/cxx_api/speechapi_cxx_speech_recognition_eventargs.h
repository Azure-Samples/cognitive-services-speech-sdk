//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_speech_recognition_eventargs.h: Public API declarations for SpeechRecognitionEventArgs C++ class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_c_common.h>
#include <speechapi_cxx_recognition_eventargs.h>
#include <speechapi_cxx_speech_recognition_result.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


/// <summary>
/// Class for speech recognition event arguments.
/// </summary>
class SpeechRecognitionEventArgs final : public RecognitionEventArgs
{
private:

    SPXEVENTHANDLE m_hevent;
    std::shared_ptr<SpeechRecognitionResult> m_result;

public:

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="hevent">Event handle</param>
    explicit SpeechRecognitionEventArgs(SPXEVENTHANDLE hevent) :
        RecognitionEventArgs(hevent),
        m_hevent(hevent),
        m_result(std::make_shared<SpeechRecognitionResult>(ResultHandleFromEventHandle(hevent))),
        Result(*m_result.get())
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
    };

    /// <inheritdoc/>
    virtual ~SpeechRecognitionEventArgs()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
        SPX_THROW_ON_FAIL(Recognizer_EventHandle_Close(m_hevent));
    };

    /// <summary>
    /// Speech recognition event result.
    /// </summary>
    const SpeechRecognitionResult& Result;

private:

    DISABLE_DEFAULT_CTORS(SpeechRecognitionEventArgs);
    
    SPXRESULTHANDLE ResultHandleFromEventHandle(SPXEVENTHANDLE hevent)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(Recognizer_RecognitionEvent_GetResult(hevent, &hresult));
        return hresult;
    }
};


} } } // Microsoft::CognitiveServices::Speech
