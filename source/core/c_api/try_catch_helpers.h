//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// try_catch_helpers.h: Helper macros for all SPXAPI methods; use these in each public API function to
//                      ensure no C++ exceptions leak out of our C API implementation
//

#pragma once
#include <speechapi_c_common.h>
#include <exception.h>

using Microsoft::CognitiveServices::Speech::Impl::StoreException;

#define SPXAPI_INIT_HR_TRY(hr)                              \
{                                                           \
    SPX_INIT_HR(hr);                                        \
    try

#define SPXAPI_TRY()                                        \
{                                                           \
    try

#define SPXAPI_CATCH(x)                                     \
    catch (SPXHR hrx)                                       \
    {                                                       \
        SPX_REPORT_ON_FAIL(hrx);                            \
        x = hrx;                                            \
    }                                                       \
    catch (ExceptionWithCallStack ex)                       \
    {                                                       \
       x = StoreException(std::move(ex));                   \
    }                                                       \
    catch (...)                                             \
    {                                                       \
        SPX_REPORT_ON_FAIL(SPXERR_UNHANDLED_EXCEPTION);     \
        x = SPXERR_UNHANDLED_EXCEPTION;                     \
    }

#define SPXAPI_CATCH_AND_RETURN_HR(hr)                      \
    SPXAPI_CATCH(hr);                                       \
    SPX_RETURN_HR(hr);                                      \
}

#define SPXAPI_CATCH_AND_RETURN_HR_EXCLUDE(hr, excludeHr)   \
    SPXAPI_CATCH(hr);                                       \
    do {                                                    \
        SPXHR x = hr;                                       \
        if (SPX_FAILED(x) && (x != (excludeHr))) {          \
            __SPX_TRACE_HR("SPX_RETURN_ON_FAIL: ", hr, x);  \
        }                                                   \
        return x;                                           \
    } while (0);                                            \
}

#define SPXAPI_CATCH_AND_RETURN(hr, x)                      \
    SPXAPI_CATCH(hr);                                       \
    return x;                                               \
}
