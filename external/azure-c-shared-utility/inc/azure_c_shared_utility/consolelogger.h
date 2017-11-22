// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "azure_c_shared_utility/xlogging.h"

	extern void consolelogger_log(LOG_CATEGORY log_category, unsigned int options, const char* format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CONSOLELOGGER_H */
