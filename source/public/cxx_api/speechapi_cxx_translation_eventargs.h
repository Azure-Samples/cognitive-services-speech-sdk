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
* Defines the arguments passed with translation events.
*/
template <class ResultT>
class TranslationEventArgs final : public SessionEventArgs
{
public:

    TranslationEventArgs(SPXEVENTHANDLE hevent) :
        SessionEventArgs(hevent),
        Result(*m_result.get()),
        m_hevent(hevent)
    {
        SPX_THROW_ON_FAIL(SPXERR_NOT_IMPL);
    };

    virtual ~TranslationEventArgs() { };

    const ResultT& Result;

private:

    TranslationEventArgs(const TranslationEventArgs&) = delete;
    TranslationEventArgs(const TranslationEventArgs&&) = delete;

    TranslationEventArgs& operator=(const TranslationEventArgs&) = delete;

    SPXEVENTHANDLE m_hevent;
    std::shared_ptr<ResultT> m_result;
};


} } } // CARBON_NAMESPACE_ROOT::Recognition::Translation
