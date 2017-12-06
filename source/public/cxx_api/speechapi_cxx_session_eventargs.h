//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_session_eventargs.h: Public API declarations for SessionEventArgs C++ class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {
   
    
class SessionEventArgs : public EventArgs
{
public:

    virtual ~SessionEventArgs() {}

    const std::wstring& SessionId;
    
protected:

    SessionEventArgs(const std::wstring& sessionId) : 
        SessionId(sessionId) 
    {    
    };


private:

    SessionEventArgs(const SessionEventArgs&) = delete;
    SessionEventArgs(const SessionEventArgs&&) = delete;

    SessionEventArgs& operator=(const SessionEventArgs&) = delete;
};


}; // CARBON_NAMESPACE_ROOT
