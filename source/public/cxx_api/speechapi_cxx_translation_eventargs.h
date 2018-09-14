//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_translation_eventargs.h: Public API declarations for TranslationEventArgs C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_session_eventargs.h>
#include <speechapi_cxx_translation_result.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Translation {


/// <summary>
/// Defines payload that is sent with the event <see cref="Recognizing"/> or <see cref="Recognized"/>.
/// </summary>
class TranslationTextResultEventArgs : public RecognitionEventArgs
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
        RecognitionEventArgs(hevent),
        m_hevent(hevent),
        m_result(std::make_shared<TranslationTextResult>(ResultHandleFromEventHandle(hevent))),
        Result(m_result)
    {
        UNUSED(m_hevent);
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
    };

    /// <summary>
    /// Destructs the instance.
    /// </summary>
    virtual ~TranslationTextResultEventArgs() { };

#if defined(SWIG) || defined(BINDING_OBJECTIVE_C)
private:
#endif
    /// <summary>
    /// Contains the translation text result.
    /// </summary>
    std::shared_ptr<TranslationTextResult> Result;

#if defined(SWIG) || defined(BINDING_OBJECTIVE_C)
public:
#else
protected:
#endif
    /// <summary>
    /// Contains the translation text result.
    /// </summary>
    std::shared_ptr<TranslationTextResult> GetResult() const { return m_result; }

private:
    DISABLE_DEFAULT_CTORS(TranslationTextResultEventArgs);

    SPXRESULTHANDLE ResultHandleFromEventHandle(SPXEVENTHANDLE hevent)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(recognizer_recognition_event_get_result(hevent, &hresult));
        return hresult;
    }
};


/// <summary>
/// Class for translation recognition canceled event arguments.
/// </summary>
class TranslationTextResultCanceledEventArgs final : public TranslationTextResultEventArgs
{
private:

    std::shared_ptr<CancellationDetails> m_cancellation;
    CancellationReason m_cancellationReason;

public:

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="hevent">Event handle</param>
    explicit TranslationTextResultCanceledEventArgs(SPXEVENTHANDLE hevent) :
        TranslationTextResultEventArgs(hevent),
        m_cancellation(CancellationDetails::FromResult(GetResult())),
        m_cancellationReason(m_cancellation->Reason),
        Reason(m_cancellationReason),
        ErrorDetails(m_cancellation->ErrorDetails)
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x)", __FUNCTION__, this);
    };

    /// <inheritdoc/>
    virtual ~TranslationTextResultCanceledEventArgs()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x)", __FUNCTION__, this);
    };

#if defined(SWIG) || defined(BINDING_OBJECTIVE_C)
private:
#endif

    /// <summary>
    /// The reason the result was canceled.
    /// </summary>
    const CancellationReason& Reason;

    /// <summary>
    /// In case of an unsuccessful recognition, provides a details of why the occurred error.
    /// This field is only filled-out if the reason canceled (<see cref="Reason"/>) is set to Error.
    /// </summary>
    const std::string ErrorDetails;

#if defined(SWIG) || defined(BINDING_OBJECTIVE_C)
public:
#else
private:
#endif
    /// <summary>
    /// CancellationDetails.
    /// </summary>
    std::shared_ptr<CancellationDetails> GetCancellationDetails() const { return m_cancellation; }

private:

    DISABLE_DEFAULT_CTORS(TranslationTextResultCanceledEventArgs);
};



/// <summary>
/// Defines payload that is sent with the event <see cref="Synthesizing"/>.
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
        Result(m_result)
    {
        UNUSED(m_hevent);
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
    };

    /// <summary>
    /// Destructs the instance.
    /// </summary>
    virtual ~TranslationSynthesisResultEventArgs() { };

#if defined(SWIG) || defined(BINDING_OBJECTIVE_C)
private:
#endif
    /// <summary>
    /// Contains the translation synthesis result.
    /// </summary>
    std::shared_ptr<TranslationSynthesisResult> Result;

#if defined(SWIG) || defined(BINDING_OBJECTIVE_C)
public:
#else
private:
#endif
    /// <summary>
    /// Contains the translation synthesis result.
    /// </summary>
    std::shared_ptr<TranslationSynthesisResult> GetResult() const { return m_result; }

private:

    DISABLE_DEFAULT_CTORS(TranslationSynthesisResultEventArgs);

    SPXRESULTHANDLE SynthesisResultHandleFromEventHandle(SPXEVENTHANDLE hevent)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(recognizer_recognition_event_get_result(hevent, &hresult));
        return hresult;
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Translation
