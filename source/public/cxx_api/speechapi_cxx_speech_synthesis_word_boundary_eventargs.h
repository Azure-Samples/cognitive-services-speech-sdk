//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_speech_synthesis_word_boundary_eventargs.h: Public API declarations for SpeechSynthesisWordBoundaryEventArgs C++ class
//

#pragma once
#include <string>
#include <speechapi_cxx_eventargs.h>
#include <speechapi_c_synthesizer.h>
#include <spxdebug.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


/// <summary>
/// Class for speech synthesis word boundary event arguments.
/// Added in version 1.7.0
/// </summary>
class SpeechSynthesisWordBoundaryEventArgs : public EventArgs
{
public:

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="hevent">Event handle</param>
    explicit SpeechSynthesisWordBoundaryEventArgs(SPXEVENTHANDLE hevent) : m_hevent(hevent)
    {
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%p, handle=0x%p)", __FUNCTION__, (void*)this, (void*)m_hevent);
        synthesizer_word_boundary_event_get_values(hevent, &m_audioOffset, &m_textOffset, &m_wordLength);
        AudioOffset = m_audioOffset;
        TextOffset = m_textOffset;
        WordLength = m_wordLength;
    };

    /// <inheritdoc/>
    virtual ~SpeechSynthesisWordBoundaryEventArgs()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%p, handle=0x%p)", __FUNCTION__, (void*)this, (void*)m_hevent);
        SPX_THROW_ON_FAIL(synthesizer_event_handle_release(m_hevent));
    }

    /// <summary>
    /// Word boundary audio offset.
    /// </summary>
    uint64_t AudioOffset;

    /// <summary>
    /// Word boundary text offset.
    /// </summary>
    uint32_t TextOffset;

    /// <summary>
    /// Word boundary word length.
    /// </summary>
    uint32_t WordLength;


private:

    DISABLE_DEFAULT_CTORS(SpeechSynthesisWordBoundaryEventArgs);

    SPXEVENTHANDLE m_hevent;
    uint64_t m_audioOffset{ 0 };
    uint32_t m_textOffset{ 0 };
    uint32_t m_wordLength{ 0 };
};


} } } // Microsoft::CognitiveServices::Speech
