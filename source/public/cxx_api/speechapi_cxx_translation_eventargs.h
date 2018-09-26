//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_cxx_session_eventargs.h>
#include <speechapi_cxx_translation_result.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Translation {


/// <summary>
/// Defines payload that is sent with the event <see cref="Recognizing"/> or <see cref="Recognized"/>.
/// </summary>
class TranslationRecognitionEventArgs : public RecognitionEventArgs
{
private:

    SPXEVENTHANDLE m_hevent;
    std::shared_ptr<TranslationRecognitionResult> m_result;

public:

    /// <summary>
    /// It is intended for internal use only. It creates an instance of <see cref="TranslationRecognitionEventArgs"/>.
    /// </summary>
    /// <param name="hevent">The handle returned by recognizer in C-API.</param>
    explicit TranslationRecognitionEventArgs(SPXEVENTHANDLE hevent) :
        RecognitionEventArgs(hevent),
        m_hevent(hevent),
        m_result(std::make_shared<TranslationRecognitionResult>(ResultHandleFromEventHandle(hevent))),
        Result(m_result)
    {
        UNUSED(m_hevent);
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
    };

    /// <summary>
    /// Destructs the instance.
    /// </summary>
    virtual ~TranslationRecognitionEventArgs() { };

#if defined(SWIG) || defined(BINDING_OBJECTIVE_C)
private:
#endif
    /// <summary>
    /// Contains the translation recognition result.
    /// </summary>
    std::shared_ptr<TranslationRecognitionResult> Result;

#if defined(SWIG) || defined(BINDING_OBJECTIVE_C)
public:
#else
protected:
#endif

    /*! \cond PROTECTED */

    /// <summary>
    /// Contains the translation text result.
    /// </summary>
    std::shared_ptr<TranslationRecognitionResult> GetResult() const { return m_result; }

    /*! \endcond */

private:
    DISABLE_DEFAULT_CTORS(TranslationRecognitionEventArgs);

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
class TranslationRecognitionCanceledEventArgs final : public TranslationRecognitionEventArgs
{
private:

    std::shared_ptr<CancellationDetails> m_cancellation;
    CancellationReason m_cancellationReason;

public:

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="hevent">Event handle</param>
    explicit TranslationRecognitionCanceledEventArgs(SPXEVENTHANDLE hevent) :
        TranslationRecognitionEventArgs(hevent),
        m_cancellation(CancellationDetails::FromResult(GetResult())),
        m_cancellationReason(m_cancellation->Reason),
        Reason(m_cancellationReason),
        ErrorDetails(m_cancellation->ErrorDetails)
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x)", __FUNCTION__, this);
    };

    /// <inheritdoc/>
    virtual ~TranslationRecognitionCanceledEventArgs()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x)", __FUNCTION__, this);
    };

#if defined(SWIG) || defined(BINDING_OBJECTIVE_C)
private:
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif
    /// <summary>
    /// The reason the result was canceled.
    /// </summary>
    const CancellationReason& Reason;
#ifdef __clang__
#pragma clang diagnostic pop
#endif

    /// <summary>
    /// In case of an unsuccessful recognition, provides a details of why the occurred error.
    /// This field is only filled-out if the reason canceled (<see cref="Reason"/>) is set to Error.
    /// </summary>
    const SPXSTRING ErrorDetails;

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

    DISABLE_DEFAULT_CTORS(TranslationRecognitionCanceledEventArgs);
};



/// <summary>
/// Defines payload that is sent with the event <see cref="Synthesizing"/>.
/// </summary>
class TranslationSynthesisEventArgs final : public SessionEventArgs
{
private:

    SPXEVENTHANDLE m_hevent;
    std::shared_ptr<TranslationSynthesisResult> m_result;

public:
    /// <summary>
    /// It is intended for internal use only. It creates an instance of <see cref="TranslationSynthesisEventArgs"/>.
    /// </summary>
    /// <param name="hevent">The handle returned by recognizer in C-API.</param>
    explicit TranslationSynthesisEventArgs(SPXEVENTHANDLE hevent) :
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
    virtual ~TranslationSynthesisEventArgs() { };

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

    DISABLE_DEFAULT_CTORS(TranslationSynthesisEventArgs);

    SPXRESULTHANDLE SynthesisResultHandleFromEventHandle(SPXEVENTHANDLE hevent)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(recognizer_recognition_event_get_result(hevent, &hresult));
        return hresult;
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Translation
