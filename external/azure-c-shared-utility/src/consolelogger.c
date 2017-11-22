// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include "azure_c_shared_utility/xlogging.h"

void consolelogger_log(LOG_CATEGORY log_category,unsigned int options, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	(void)vprintf(format, args);
	va_end(args);

    (void)log_category;
	if (options & LOG_LINE)
	{
		(void)printf("\r\n");
	}
}
