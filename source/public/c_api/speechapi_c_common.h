//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_common.h: Public API declarations for global C definitions and typedefs
//

#pragma once

#include <stdbool.h>
#include <spxerror.h>

#ifdef __cplusplus
#define SPX_EXTERN_C        extern "C"
#else
#define SPX_EXTERN_C        extern
#endif

#ifdef _WIN32

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

#define SPXDLL_EXPORT       __declspec(dllexport)

#elif defined(SWIG)

#define SPXAPI_EXPORT
#define SPXAPI_NOTHROW
#define SPXAPI_RESULTTYPE   SPXHR
#define SPXAPI_CALLTYPE
#define SPXAPI_VCALLTYPE
#define SPXDLL_EXPORT

#else

#define SPXAPI_EXPORT       __attribute__ ((__visibility__("default")))

#define SPXAPI_NOTHROW      __attribute__((nothrow))
#define SPXAPI_RESULTTYPE   SPXHR
// when __attribute__((stdcall)) is set, gcc generates a warning : stdcall attribute ignored.
#define SPXAPI_CALLTYPE
#define SPXAPI_VCALLTYPE    __attribute__((cdecl))

#define SPXDLL_EXPORT       __attribute__ ((__visibility__("default")))

#endif

#define SPXAPI              SPX_EXTERN_C SPXAPI_EXPORT SPXAPI_RESULTTYPE SPXAPI_NOTHROW SPXAPI_CALLTYPE
#define SPXAPI_(type)       SPX_EXTERN_C SPXAPI_EXPORT type SPXAPI_NOTHROW SPXAPI_CALLTYPE
#define SPXAPI__(type)      SPX_EXTERN_C SPXAPI_EXPORT SPXAPI_NOTHROW type SPXAPI_CALLTYPE

#define SPXAPIV             SPX_EXTERN_C SPXAPI_EXPORT SPXAPI_NOTHROW SPXAPI_RESULTTYPE SPXAPI_VCALLTYPE
#define SPXAPIV_(type)      SPX_EXTERN_C SPXAPI_EXPORT SPXAPI_NOTHROW type SPXAPI_VCALLTYPE

#define SPXAPI_PRIVATE          SPX_EXTERN_C SPXAPI_RESULTTYPE SPXAPI_NOTHROW SPXAPI_CALLTYPE
#define SPXAPI_PRIVATE_(type)   SPX_EXTERN_C type SPXAPI_NOTHROW SPXAPI_CALLTYPE

struct _spx_empty {};
typedef struct _spx_empty* _spxhandle;
typedef _spxhandle SPXHANDLE;

typedef SPXHANDLE SPXASYNCHANDLE;
typedef SPXHANDLE SPXFACTORYHANDLE;
typedef SPXHANDLE SPXRECOHANDLE;
typedef SPXHANDLE SPXRESULTHANDLE;
typedef SPXHANDLE SPXEVENTHANDLE;
typedef SPXHANDLE SPXSESSIONHANDLE;
typedef SPXHANDLE SPXTRIGGERHANDLE;
typedef SPXHANDLE SPXLUMODELHANDLE;
typedef SPXHANDLE SPXKEYWORDHANDLE;
typedef SPXHANDLE SPXERRORHANDLE;
typedef SPXHANDLE SPXPROPERTYBAGHANDLE;

#define SPXHANDLE_INVALID   ((SPXHANDLE)-1)

SPXAPI_(bool) property_bag_is_valid(SPXPROPERTYBAGHANDLE hpropbag);
SPXAPI_(bool) property_bag_close(SPXPROPERTYBAGHANDLE hpropbag);

SPXAPI__(const char*) property_bag_get_string(SPXPROPERTYBAGHANDLE hpropbag, int id, const char* name, const char* defaultValue);
SPXAPI property_bag_free_string(const char* value);
SPXAPI property_bag_set_string(SPXPROPERTYBAGHANDLE hpropbag, int id, const char* name, const char* value);

#ifndef __cplusplus
enum SpeechPropertyId
{
    SpeechServiceConnection_Key = 1000,
    SpeechServiceConnection_Endpoint = 1001,
    SpeechServiceConnection_Region = 1002,
    SpeechServiceAuthorization_Token = 1003,
    SpeechServiceAuthorization_Type = 1004,
    SpeechServiceRps_Token = 1005,
    SpeechServiceConnection_DeploymentId = 1006,

    SpeechServiceConnection_TranslationFromLanguage = 2000,
    SpeechServiceConnection_TranslationToLanguages = 2001,
    SpeechServiceConnection_TranslationVoice = 2002,
    SpeechServiceConnection_TranslationFeatures = 2003,
    SpeechServiceConnection_IntentRegion = 2004,

    SpeechServiceConnection_RecoMode = 3000,
    SpeechServiceConnection_RecoMode_Interactive = 3001,
    SpeechServiceConnection_RecoMode_Conversation = 3002,
    SpeechServiceConnection_RecoMode_Dictation = 3004,
    SpeechServiceConnection_RecoLanguage = 3005,
    Speech_SessionId = 3006,

    SpeechServiceResponse_OutputFormat = 4000,
    SpeechServiceResponse_OutputFormat_Simple = 4001,
    SpeechServiceResponse_OutputFormat_Detailed = 4002,
    SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4003,

    SpeechServiceResponse_Json = 5000,
    SpeechServiceResponse_JsonResult = 5001,
    SpeechServiceResponse_JsonErrorDetails = 5002,

    CancellationDetails_ReasonCanceled = 6000,
    CancellationDetails_ReasonText = 6001,
    CancellationDetails_ReasonDetailedText = 6002
};
#endif
