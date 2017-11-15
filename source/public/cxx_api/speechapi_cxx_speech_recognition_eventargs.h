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


class SpeeechRecognitionEventArgs final : public RecognitionEventArgs
{
public:

    SpeeechRecognitionEventArgs() :
        RecognitionEventArgs(m_sessionId),
        Result(m_result),
        m_result(SPXHANDLE_INVALID)
    {
    };

    virtual ~SpeeechRecognitionEventArgs() {};

    const SpeechRecognitionResult& Result;

private:

    SpeeechRecognitionEventArgs(const SpeeechRecognitionEventArgs&) = delete;
    SpeeechRecognitionEventArgs(const SpeeechRecognitionEventArgs&&) = delete;

    SpeeechRecognitionEventArgs& operator=(const SpeeechRecognitionEventArgs&) = delete;

    std::wstring m_sessionId;
    SpeechRecognitionResult m_result;
};


} } }; // CARBON_NAMESPACE_ROOT :: Recognition :: Speech
