//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_common.h: Public API declarations for global C++ APIs/namespaces
//

#pragma once

#ifdef SWIG
#define final
#endif

#define CARBON_NAMESPACE_ROOT   Carbon

#define DISABLE_COPY_AND_MOVE(T)     \
    T(const T&) = delete;            \
    T& operator=(const T&) = delete; \
    T(T&&) = delete;                 \
    T& operator=(T&&) = delete

#define DISABLE_DEFAULT_CTORS(T)     \
    T() = delete;                    \
    DISABLE_COPY_AND_MOVE(T)

