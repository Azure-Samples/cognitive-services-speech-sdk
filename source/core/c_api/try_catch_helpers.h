//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// try_catch_helpers.h: Helper macros for all SPXAPI methods; use these in each public API function to
//                      ensure no C++ exceptions leak out of our C API implementation
//

#pragma once
#include <speechapi_c_common.h>


#define SPXAPI_INIT_TRY(hr)                         \
{                                                   \
    SPX_INIT_HR(hr);                                \
    try
#define SPXAPI_CATCH_AND_RETURN(hr)                 \
    catch (SPXHR hr)                                \
    {                                               \
        SPX_RETURN_HR(hr);                          \
    }                                               \
    catch (std::exception ex)                       \
    {                                               \
        SPX_RETURN_HR(SPXERR_UNHANDLED_EXCEPTION);  \
    }                                               \
    SPX_RETURN_HR(hr);                              \
}
