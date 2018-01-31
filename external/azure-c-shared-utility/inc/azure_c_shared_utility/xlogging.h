// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef XLOGGING_H
#define XLOGGING_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdio.h>
#include <time.h>

/*no logging is useful when time and fprintf are mocked*/
#ifdef NO_LOGGING
#define LOG(...)
#define LogInfo(...)
#define LogError(...)
#define xlogging_get_log_function() NULL
#else

typedef enum LOG_CATEGORY_TAG
{
    LOG_ERROR,
    LOG_INFO,
    LOG_TRACE
} LOG_CATEGORY;

typedef void(*LOGGER_LOG)(LOG_CATEGORY log_category, unsigned int options, const char* format, ...);

#define LOG_LINE 0x01

#define LOG(log_category, log_options, format, ...) { LOGGER_LOG l = xlogging_get_log_function(); if (l != NULL) l(log_category, log_options, format, ##__VA_ARGS__); }

#define LogInfo(FORMAT, ...) do { LOG(LOG_INFO, LOG_LINE, "Info: %s(): " FORMAT, __FUNCTION__, ##__VA_ARGS__); } while(0)

#define ISOPRINTARGS(__ptm) ((__ptm)->tm_year + 1900), ((__ptm)->tm_mon + 1), (__ptm)->tm_mday, (__ptm)->tm_hour, (__ptm)->tm_min, (__ptm)->tm_sec
#define ISOPRINTFMT "%d-%02d-%02dT%02d:%02d:%02d"
#if defined _MSC_VER
#define LogError(FORMAT, ...) do{ time_t t = time(NULL); struct tm ltm; localtime_s(&ltm, &t); LOG(LOG_ERROR, LOG_LINE, "Error: Time:" ISOPRINTFMT " File:%s Func:%s Line:%d " FORMAT, ISOPRINTARGS(&ltm), __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); }while(0)
#else
#define LogError(FORMAT, ...) do{ time_t t = time(NULL); struct tm *ptm = localtime(&t); LOG(LOG_ERROR, LOG_LINE, "Error: Time:" ISOPRINTFMT " File:%s Func:%s Line:%d " FORMAT, ISOPRINTARGS(ptm), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); }while(0)
#endif

extern void xlogging_set_log_function(LOGGER_LOG log_function);
extern LOGGER_LOG xlogging_get_log_function(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XLOGGING_H */
