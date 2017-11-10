// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UNIQUEID_H
#define UNIQUEID_H

#include "azure_c_shared_utility/macro_utils.h"

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

#include "azure_c_shared_utility/umock_c_prod.h"

#define UNIQUEID_RESULT_VALUES    \
    UNIQUEID_OK,                  \
    UNIQUEID_INVALID_ARG,         \
    UNIQUEID_ERROR

    DEFINE_ENUM(UNIQUEID_RESULT, UNIQUEID_RESULT_VALUES)

        MOCKABLE_FUNCTION(, UNIQUEID_RESULT, UniqueId_Generate, char*, uid, size_t, bufferSize);

#ifdef __cplusplus
}
#endif

#endif /* UNIQUEID_H */
