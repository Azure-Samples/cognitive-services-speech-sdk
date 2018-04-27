//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognition_eventargs.h: Public API declarations for RecognitionEventArgs C++ base class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Provides data for the RecognitionEvent.
/// </summary>
class RecognitionEventArgs : public SessionEventArgs
{
public:

    /// <summary>
    /// Constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit RecognitionEventArgs(SPXEVENTHANDLE hevent) :
        SessionEventArgs(hevent),
        Offset(m_offset),
        m_offset(GetOffset(hevent))
    {
        if (hevent == nullptr)
            hevent = SPXHANDLE_INVALID;
    };

    /// <inheritdoc/>
    virtual ~RecognitionEventArgs() {}

    /// <summary>
    /// The offset of recognition event
    /// </summary>
    const uint64_t& Offset;

protected:

    /// <summary>
    /// Extract offset from given event handle <paramref name="hevent"/>
    /// </summary>
    static uint64_t GetOffset(SPXEVENTHANDLE hevent)
    {
        uint64_t offset = 0;
        SPX_THROW_ON_FAIL(Recognizer_RecognitionEvent_GetOffset(hevent, &offset));
        return offset;
    }

    // Todo: make it private after passing SWIG
private:

    DISABLE_COPY_AND_MOVE(RecognitionEventArgs);
    uint64_t m_offset;
};


} } } // Microsoft::CognitiveServices::Speech
