#pragma once

#include <stdbool.h>
#include <spxdebug.h>

#ifndef SPX_EXTERN_C
#ifdef __cplusplus
#define SPX_EXTERN_C        extern "C"
#else
#define SPX_EXTERN_C        extern
#endif
#endif

#define SPXAPI_RESULTTYPE   SPXHR
#define SPXAPI_NOTHROW      __declspec(nothrow)
#define SPXAPI_CALLTYPE     __stdcall

#define SPXAPI              SPXAPI_NOTHROW SPXAPI_RESULTTYPE SPXAPI_CALLTYPE
#define SPXAPI_(type)       SPXAPI_NOTHROW type SPXAPI_CALLTYPE

#define SPXHANDLE           void*
#define SPXRECOHANDLE       SPXHANDLE
#define SPXASYNCHANDLE      SPXHANDLE
#define SPXRESULTHANDLE     SPXHANDLE
#define SPXEVENTHANDLE      SPXHANDLE

#define SPXHANDLE_INVALID   ((void*)-1)

