//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_translation_eventargs.h: Public API declarations for TranslationEventArgs C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_c_common.h>
#include <speechapi_cxx_session_eventargs.h>
#include <speechapi_cxx_translation_result.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Translation {

/// <summary>
/// Defines payload that is sent with the event <see cref="IntermediateResult"/> or <see cref="FinalResult"/>.
/// </summary>
class TranslationTextResultEventArgs final : public SessionEventArgs
{
private:
    SPXEVENTHANDLE m_hevent;
    std::shared_ptr<TranslationTextResult> m_result;

public:
    /// <summary>
    /// It is intended for internal use only. It creates an instance of <see cref="TranslationTextResultEventArgs"/>.
    /// </summary>
    /// <param name="resultHandle">The handle returned by recognizer in C-API.</param>
    explicit TranslationTextResultEventArgs(SPXEVENTHANDLE hevent) :
        SessionEventArgs(hevent),
        m_hevent(hevent),
        m_result(std::make_shared<TranslationTextResult>(ResultHandleFromEventHandle(hevent))),
        Result(*m_result.get())
    {
        UNUSED(m_hevent);
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
    };

    /// <summary>
    /// Destructs the instance.
    /// </summary>
    virtual ~TranslationTextResultEventArgs() { };

    /// <summary>
    /// Contains the translation text result.
    /// </summary>
    const TranslationTextResult& Result;

private:
    DISABLE_DEFAULT_CTORS(TranslationTextResultEventArgs);

    SPXRESULTHANDLE ResultHandleFromEventHandle(SPXEVENTHANDLE hevent)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(Recognizer_RecognitionEvent_GetResult(hevent, &hresult));
        return hresult;
    }
};


/// <summary>
/// Defines payload that is sent with the event <see cref="TranslationSynthesisResultEvent"/>.
/// </summary>
class TranslationSynthesisResultEventArgs final : public SessionEventArgs
{
private:

    SPXEVENTHANDLE m_hevent;
    std::shared_ptr<TranslationSynthesisResult> m_result;

public:
    /// <summary>
    /// It is intended for internal use only. It creates an instance of <see cref="TranslationSynthesisResultEventArgs"/>.
    /// </summary>
    /// <param name="resultHandle">The handle returned by recognizer in C-API.</param>
    explicit TranslationSynthesisResultEventArgs(SPXEVENTHANDLE hevent) :
        SessionEventArgs(hevent),
        m_hevent(hevent),
        m_result(std::make_shared<TranslationSynthesisResult>(SynthesisResultHandleFromEventHandle(hevent))),
        Result(*m_result.get())
    {
        UNUSED(m_hevent);
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
    };

    /// <summary>
    /// Destructs the instance.
    /// </summary>
    virtual ~TranslationSynthesisResultEventArgs() { };

    /// <summary>
    /// Contains the translation synthesis result.
    /// </summary>
    const TranslationSynthesisResult& Result;

private:

    DISABLE_DEFAULT_CTORS(TranslationSynthesisResultEventArgs);

    SPXRESULTHANDLE SynthesisResultHandleFromEventHandle(SPXEVENTHANDLE hevent)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(Recognizer_RecognitionEvent_GetResult(hevent, &hresult));
        return hresult;
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Translation
