//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_translation_eventargs.h: Public API declarations for TranslationEventArgs C++ class
//

#pragma once
#include <speechapi_c_common.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_session_eventargs.h>
#include <speechapi_cxx_translation_result.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Translation {

/*
* Defines the arguments passed with translation text result events.
*/
class TranslationTextResultEventArgs final : public SessionEventArgs
{
private:
    SPXEVENTHANDLE m_hevent;
    std::shared_ptr<TranslationTextResult> m_result;

public:

    TranslationTextResultEventArgs(SPXEVENTHANDLE hevent) :
        SessionEventArgs(hevent),
        m_hevent(hevent),
        m_result(std::make_shared<TranslationTextResult>(ResultHandleFromEventHandle(hevent))),
        Result(*m_result.get())
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
    };

    virtual ~TranslationTextResultEventArgs() { };

    const TranslationTextResult& Result;

private:
    TranslationTextResultEventArgs() = delete;
    TranslationTextResultEventArgs(TranslationTextResultEventArgs&&) = delete;
    TranslationTextResultEventArgs(const TranslationTextResultEventArgs&) = delete;
    TranslationTextResultEventArgs& operator=(TranslationTextResultEventArgs&&) = delete;
    TranslationTextResultEventArgs& operator=(const TranslationTextResultEventArgs&) = delete;

    SPXRESULTHANDLE ResultHandleFromEventHandle(SPXEVENTHANDLE hevent)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(Recognizer_RecognitionEvent_GetResult(hevent, &hresult));
        return hresult;
    }
};


/*
* Defines the arguments passed with translation synthesis result events.
*/
class TranslationSynthesisResultEventArgs final : public SessionEventArgs
{
private:

    SPXEVENTHANDLE m_hevent;
    std::shared_ptr<TranslationSynthesisResult> m_result;

public:

    TranslationSynthesisResultEventArgs(SPXEVENTHANDLE hevent) :
        SessionEventArgs(hevent),
        m_hevent(hevent),
        m_result(std::make_shared<TranslationSynthesisResult>(ResultHandleFromEventHandle(hevent))),
        Result(*m_result.get())
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hevent);
    };

    virtual ~TranslationSynthesisResultEventArgs() { };

    const TranslationSynthesisResult& Result;

private:

    SPXRESULTHANDLE ResultHandleFromEventHandle(SPXEVENTHANDLE hevent)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(Recognizer_RecognitionEvent_GetResult(hevent, &hresult));
        return hresult;
    }

    TranslationSynthesisResultEventArgs() = delete;
    TranslationSynthesisResultEventArgs(TranslationSynthesisResultEventArgs&&) = delete;
    TranslationSynthesisResultEventArgs(const TranslationSynthesisResultEventArgs&) = delete;
    TranslationSynthesisResultEventArgs& operator=(TranslationSynthesisResultEventArgs&&) = delete;
    TranslationSynthesisResultEventArgs& operator=(const TranslationSynthesisResultEventArgs&) = delete;

};

} } } // CARBON_NAMESPACE_ROOT::Recognition::Translation
