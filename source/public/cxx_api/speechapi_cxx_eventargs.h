//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_eventargs.h: Public API declarations for EventArgs C++ base class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {

    
class EventArgs
{
public:

    virtual ~EventArgs() {}

protected:

    EventArgs() {};


private:

    EventArgs(const EventArgs&) = delete;
    EventArgs(const EventArgs&&) = delete;

    EventArgs& operator=(const EventArgs&) = delete;
};


} // CARBON_NAMESPACE_ROOT
