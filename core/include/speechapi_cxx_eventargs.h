#pragma once
#include <string>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {

    
class EventArgs
{
public:

    virtual ~EventArgs() = 0;

protected:

    EventArgs() {};


private:

    EventArgs(const EventArgs&) = delete;
    EventArgs(const EventArgs&&) = delete;

    EventArgs& operator=(const EventArgs&) = delete;
};


}; // CARBON_NAMESPACE_ROOT
