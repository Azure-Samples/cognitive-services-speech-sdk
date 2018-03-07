//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_global.h: Public API declarations for GlobalParameters C++ class
//

#pragma once
#include <mutex>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_global_parameter_collection.h>


namespace CARBON_NAMESPACE_ROOT {


class GlobalParameters
{
public:

    static GlobalParameterCollection& Get()
    {
        static std::mutex mutex;
        std::unique_lock<std::mutex> lock(mutex);

        static std::shared_ptr<GlobalParameterCollection> parameters;
        if (parameters.get() == nullptr)
        {
            SPX_TRACE_SCOPE("Making the GlobalParameterCollection", "Done");
            parameters = std::make_shared<GlobalParameterCollection>();
        }

        return *parameters.get();
    };


private:

    GlobalParameters() = delete;
    GlobalParameters(GlobalParameters&&) = delete;
    GlobalParameters(const GlobalParameters&) = delete;
    GlobalParameters& operator=(GlobalParameters&&) = delete;
    GlobalParameters& operator=(const GlobalParameters&) = delete;
};


} // CARBON_NAMESPACE_ROOT
