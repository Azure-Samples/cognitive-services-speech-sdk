//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_todo.h: Temporary header file // TODO: Remove
//

#pragma once
#include <stdexcept>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {


class NotYetImplementedException : public std::runtime_error
{
public:

    NotYetImplementedException() : std::runtime_error("CARBON: not yet implemented"){};
};


} // CARBON_NAMESPACE_ROOT
