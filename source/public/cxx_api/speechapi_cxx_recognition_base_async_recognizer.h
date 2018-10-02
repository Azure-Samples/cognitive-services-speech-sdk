//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_recognition_base_async_recognizer.h: Public API declarations for BaseAsyncRecognizer C++ class
//

#pragma once
#include <future>
#include <memory>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_eventsignal.h>
#include <speechapi_cxx_session_eventargs.h>
#include <speechapi_cxx_recognition_base_async_recognizer.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// BaseAsyncRecognizer class.
/// </summary>
class BaseAsyncRecognizer : public AsyncRecognizer<RecognitionResult, RecognitionEventArgs, RecognitionEventArgs>
{
public:

    /// <summary>
    /// Extracts a shared pointer to BaseAsyncRecognizer from the provided recognizer shared pointer.
    /// </summary>
    /// <param name="recognizer">Recognizer shared pointer.</param>
    /// <returns>A shared pointer to BaseAsyncRecognizer.</returns>
    template <class T>
    static std::shared_ptr<BaseAsyncRecognizer> FromRecognizer(const std::shared_ptr<T> &recognizer)
    {
        UNUSED(recognizer);
        SPX_STATIC_ASSERT_IS_BASE_OF(Recognizer, T);
        return nullptr;
    }

protected:

    /*! \cond PROTECTED */

    using BaseType = AsyncRecognizer<RecognitionResult, RecognitionEventArgs, RecognitionEventArgs>;

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit BaseAsyncRecognizer(SPXRECOHANDLE hreco) :
        BaseType(hreco)
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    ~BaseAsyncRecognizer()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        TermRecognizer();
    }

    DISABLE_DEFAULT_CTORS(BaseAsyncRecognizer);

    /*! \endcond */
};


} } } // Microsoft::CognitiveServices::Speech
