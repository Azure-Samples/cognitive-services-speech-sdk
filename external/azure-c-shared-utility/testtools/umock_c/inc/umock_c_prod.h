// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#undef MOCKABLE_FUNCTION

/* This header is meant to be included by production code headers, so that the MOCKABLE_FUNCTION gets enabled. */
/* 
    If you are porting to a new platform and do not want to build the tests, but only the production code,
    simply make sure that this file is in the include path (either by copying it to your inc folder or 
    by adjusting the include paths).
*/

#ifdef ENABLE_MOCKS

/* Codes_SRS_UMOCK_C_LIB_01_001: [MOCKABLE_FUNCTION shall be used to wrap function definition allowing the user to declare a function that can be mocked.]*/
#define MOCKABLE_FUNCTION(modifiers, result, function, ...) \
    MOCKABLE_FUNCTION_UMOCK_INTERNAL_WITH_MOCK(modifiers, result, function, __VA_ARGS__)

#include "umock_c.h"

#else

#include "azure_c_shared_utility/macro_utils.h"

#define UMOCK_C_PROD_ARG_IN_SIGNATURE(count, arg_type, arg_name) arg_type arg_name IFCOMMA(count)

/* Codes_SRS_UMOCK_C_LIB_01_002: [The macro shall generate a function signature in case ENABLE_MOCKS is not defined.] */
/* Codes_SRS_UMOCK_C_LIB_01_005: [**If ENABLE_MOCKS is not defined, MOCKABLE_FUNCTION shall only generate a declaration for the function.] */
/* Codes_SRS_UMOCK_C_LIB_01_001: [MOCKABLE_FUNCTION shall be used to wrap function definition allowing the user to declare a function that can be mocked.]*/
#define MOCKABLE_FUNCTION(modifiers, result, function, ...) \
    result modifiers function(IF(COUNT_ARG(__VA_ARGS__),,void) FOR_EACH_2_COUNTED(UMOCK_C_PROD_ARG_IN_SIGNATURE, __VA_ARGS__));

#endif
