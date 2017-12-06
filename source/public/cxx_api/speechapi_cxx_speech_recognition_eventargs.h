//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_speech_recognition_eventargs.h: Public API declarations for SpeechRecognitionEventArgs C++ class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_recognition_eventargs.h>


namespace CARBON_NAMESPACE_ROOT { 
namespace Recognition {
namespace Speech {


class SpeechRecognitionEventArgs final : public RecognitionEventArgs
{
public:

    SpeechRecognitionEventArgs(SPXEVENTHANDLE hevent) :
        RecognitionEventArgs(m_sessionId),
        Result(InitResult(hevent)),
        m_hevent(hevent)
    {
        // TODO: RobCh: Next: Init m_sessionId
    };

    virtual ~SpeechRecognitionEventArgs() {};

    const SpeechRecognitionResult& Result;

private:

    SpeechRecognitionEventArgs(const SpeechRecognitionEventArgs&) = delete;
    SpeechRecognitionEventArgs(const SpeechRecognitionEventArgs&&) = delete;

    SpeechRecognitionEventArgs& operator=(const SpeechRecognitionEventArgs&) = delete;

    SPXRESULTHANDLE ResultHandleFromEventHandle(SPXEVENTHANDLE hevent)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(Recognizer_RecognitionEvent_GetResult(hevent, &hresult));
        return hresult;
    }

    const SpeechRecognitionResult& InitResult(SPXEVENTHANDLE hevent)
    {
        m_result = std::make_shared<SpeechRecognitionResult>(ResultHandleFromEventHandle(hevent));
        return *m_result.get();
    }

    SPXEVENTHANDLE m_hevent;
    std::wstring m_sessionId;

    std::shared_ptr<SpeechRecognitionResult> m_result;
};


} } }; // CARBON_NAMESPACE_ROOT :: Recognition :: Speech
