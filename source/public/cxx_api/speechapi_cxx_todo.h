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


/// <summary>
/// Class that defines the type of objects thrown as exception if a feature is not implemented yet.
/// </summary>
class NotYetImplementedException : public std::runtime_error
{
public:

    /// <summary>
    /// Constructor.
    /// </summary>
    NotYetImplementedException() : std::runtime_error("CARBON: not yet implemented"){};
};


} // CARBON_NAMESPACE_ROOT
