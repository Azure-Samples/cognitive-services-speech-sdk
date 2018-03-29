//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_session.h: Public API declarations for Session C++ class
//

#pragma once
#include <exception>
#include <future>
#include <memory>
#include <string>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_session_parameter_collection.h>


namespace CARBON_NAMESPACE_ROOT {


class Session
{
public:

    template <class T>
    static std::shared_ptr<Session> FromRecognizer(std::shared_ptr<T> recognizer)
    {
        SPX_INIT_HR(hr);

        SPXSESSIONHANDLE hsession = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(hr = ::Session_From_Recognizer(recognizer->m_hreco, &hsession));

        return std::make_shared<Session>(hsession);
    }

    Session() :
        Parameters(SPXHANDLE_INVALID),
        m_hsession(SPXHANDLE_INVALID)
    {
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }

    Session(SPXSESSIONHANDLE hsession) :
        Parameters(hsession),
        m_hsession(hsession)
    {
        SPX_DBG_TRACE_FUNCTION();
    }

    virtual ~Session()
    {
        SPX_DBG_TRACE_FUNCTION();

        if (m_hsession != SPXHANDLE_INVALID)
        {
            ::Session_Handle_Close(m_hsession);
            m_hsession = SPXHANDLE_INVALID;
        }
    }

    SessionParameterValueCollection Parameters;


private:

    Session(const Session&) = delete;
    Session(const Session&&) = delete;
    const Session& operator=(const Session&) = delete;
    const Session& operator=(const Session&&) = delete;

    SPXSESSIONHANDLE m_hsession;
};


} // CARBON_NAMESPACE_ROOT
