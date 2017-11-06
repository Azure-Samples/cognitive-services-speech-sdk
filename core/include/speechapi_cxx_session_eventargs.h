#pragma once
#include <string>
#include <speechapi_cxx_common.h>

namespace CARBON_NAMESPACE_ROOT {
    
class SessionEventArgs : public EventArgs
{
public:

    virtual ~SessionEventArgs() = 0;

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
