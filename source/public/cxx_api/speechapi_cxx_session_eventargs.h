//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_session_eventargs.h: Public API declarations for SessionEventArgs C++ class
//

#pragma once
#include <string>
#include <speechapi_c_recognizer.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_eventargs.h>


namespace CARBON_NAMESPACE_ROOT {
   
    
class SessionEventArgs : public EventArgs
{
public:

    SessionEventArgs(SPXEVENTHANDLE hevent) :
        SessionId(m_sessionId),
        m_sessionId(GetSessionId(hevent))
    {
    };

    virtual ~SessionEventArgs() {}

    const std::wstring& SessionId;


protected:

    static const std::wstring GetSessionId(SPXEVENTHANDLE hevent)
    {
        static const auto cchMaxUUID = 36;
        static const auto cchMaxSessionId = cchMaxUUID + 1;
        wchar_t sessionId[cchMaxSessionId] = {};

        SPX_THROW_ON_FAIL(Recognizer_SessionEvent_GetSessionId(hevent, sessionId, cchMaxSessionId));
        return std::wstring(sessionId);
    };


private:

    SessionEventArgs() = delete;
    SessionEventArgs(SessionEventArgs&&) = delete;
    SessionEventArgs(const SessionEventArgs&) = delete;
    SessionEventArgs& operator=(SessionEventArgs&&) = delete;
    SessionEventArgs& operator=(const SessionEventArgs&) = delete;

    std::wstring m_sessionId;
};


} // CARBON_NAMESPACE_ROOT
