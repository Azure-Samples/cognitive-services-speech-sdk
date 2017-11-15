//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_common.h: Public API declarations for global C definitions and typedefs
//

#pragma once

#include <stdbool.h>
#include <spxdebug.h>

#ifdef __cplusplus
#define SPX_EXTERN_C        extern "C"
#else
#define SPX_EXTERN_C        extern
#endif

#ifdef SPX_CONFIG_EXPORTAPIS
#define SPXAPI_EXPORT       __declspec(dllexport)
#endif

#ifdef SPX_CONFIG_IMPORTAPIS
#define SPXAPI_EXPORT       __declspec(dllimport)
#endif

#ifndef SPXAPI_EXPORT
#define SPXAPI_EXPORT __declspec(dllimport)
#endif

#define SPXAPI_NOTHROW      __declspec(nothrow)
#define SPXAPI_RESULTTYPE   SPXHR
#define SPXAPI_CALLTYPE     __stdcall
#define SPXAPI_VCALLTYPE    __cdecl

#define SPXAPI              SPX_EXTERN_C SPXAPI_EXPORT SPXAPI_RESULTTYPE SPXAPI_NOTHROW SPXAPI_CALLTYPE 
#define SPXAPI_(type)       SPX_EXTERN_C SPXAPI_EXPORT type SPXAPI_NOTHROW SPXAPI_CALLTYPE 

#define SPXAPIV             SPX_EXTERN_C SPXAPI_EXPORT SPXAPI_NOTHROW SPXAPI_RESULTTYPE SPXAPI_VCALLTYPE
#define SPXAPIV_(type)      SPX_EXTERN_C SPXAPI_EXPORT SPXAPI_NOTHROW type SPXAPI_VCALLTYPE

#define SPXHANDLE           void*
#define SPXRECOHANDLE       SPXHANDLE
#define SPXASYNCHANDLE      SPXHANDLE
#define SPXRESULTHANDLE     SPXHANDLE
#define SPXEVENTHANDLE      SPXHANDLE

#define SPXHANDLE_INVALID   ((void*)-1)
