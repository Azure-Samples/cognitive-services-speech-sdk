// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "private-iot-cortana-sdk.h"
#include "parson/parson.h"

#if defined(__linux__) && !defined(IOT_CORTANA_NO_CDP)
#define USE_CDP
#endif
#ifdef USE_CDP
#include "cdp/CDPsdk.h"
#endif

const char* kString_version = "version";
const char* const kString_context = "context";
const char* const kString_deviceId = "deviceId";
const char* const kString_settings = "settings";
const char* const kString_state = "State";
const char* const kString_latitude = "latitude";
const char* const kString_longitude = "longitude";
const char* const kString_timezone = "windowsTimeZone";
const char* const kString_serviceTag = "serviceTag";
const char* const kString_queryText = "queryText";
const char* const kString_volume = "volume";

static int skill_volume_control_getcontext(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext);
static int skill_system_getcontext(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext);
static int skill_location_getcontext(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext);

extern struct JSON_CONTENT_HANDLER
{
    const char*                     pszPath;
    PPROPERTYBAG_OBJECT_CALLBACK    handler;
} gJsonContentHandlers[];

extern struct INTENT_HANDLERS
{
    const char*                  pszId;
    PPROPERTYBAG_OBJECT_CALLBACK Handler;
} gIntentHandlers[];

const PPROPERTYBAG_OBJECT_CALLBACK kOptional_Service_Serializers[] = {
    speech_serialize,
    card_serialize,
    spotify_serialize,
};

const struct
{
    const char* const            pszId;
    PPROPERTYBAG_OBJECT_CALLBACK Handler;
} kRequired_Service_Serializers[] = {
    { "audioPlayer",        audioplayer_serialize },
    { "volumeControl",      skill_volume_control_getcontext },
    { "bluetooth",          bluetooth_serialize },
    { "alarms",             skill_alarms_getcontext },
    { "system",             skill_system_getcontext },
    { "location",           skill_location_getcontext },
    { "calls",              skill_call_getcontext },
    { "timers",             skill_timers_getcontext },
};
///////////////////////////////////////////////////////////////////////////////

typedef struct _SkillContext
{
    const char* queryText;
    const char* serviceTag;
} SkillContext;

typedef struct _Skill
{
    const char* id;
} Skill;

// TODO: remove audio skill information.
typedef struct _SkillAudio
{
    const char* ssml;
} SkillAudio;

typedef struct _SkillView
{
    const char* displayText;
    SkillAudio  audio;
} SkillView;

typedef struct _SkillResponse
{
    SkillContext context;
    SkillView    view;

    void*        pContext;
} SkillResponse;

typedef struct _DeserializeContext
{
    const char * pszResponseJson;
    void*        pContext;
} DeserializeContext;

#define SKILL_UNHANDLED -1

///////////////////////////////////////////////////////////////////////////////

// TODO: Remove, for debugging only.
static int Entity_SkillContext(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext)
{
    SkillContext* context = (SkillContext*)pContext;
    if (pContext)
    {
        context->queryText = propertybag_getstringvalue(hProperty, kString_queryText);
        if (!context->queryText)
        {
            PROTOCOL_VIOLATION("%s is missing", kString_queryText);
        }
        context->serviceTag = propertybag_getstringvalue(hProperty, kString_serviceTag);
        if (!context->serviceTag)
        {
            PROTOCOL_VIOLATION("%s is missing", kString_serviceTag);
        }
        else
        {
            LogInfo("%s: '%s'", kString_serviceTag, context->serviceTag);
            metrics_transport_serviceid(context->serviceTag);
        }
    }
    return 0;
}

static int Execute_Skill(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext)
{
    Skill skill;

    skill.id = propertybag_getstringvalue(hProperty, "id");
    if (skill.id)
    {
        if (skill_dispatch(skill.id, hProperty, pContext))
        {
            LogError("skill %s failed", skill.id);
        }
    }
    else
    {
        LogError("no id");
    }
    return 0;
}

static int Entity_SkillAudio(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext)
{
    SkillAudio* audio = (SkillAudio*)pContext;
    if (pContext)
    {
        audio->ssml = propertybag_getstringvalue(hProperty, "ssml");
        if (!audio->ssml)
        {
            PROTOCOL_VIOLATION("audio object contains no properties");
        }
    }
    return 0;
}

static int Entity_SkillView(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext)
{
    SkillView* view = (SkillView*)pContext;
    if (pContext)
    {
        view->displayText = propertybag_getstringvalue(hProperty, "displayText");

        (void)propertybag_getchildvalue(
            hProperty,
            "audio",
            Entity_SkillAudio,
            &view->audio);
    }
    return 0;
}

static void Execute_SkillContext(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext)
{
    SkillResponse* response = (SkillResponse*)pContext;
    if (pContext)
    {
        // TODO: we don't care about the context property.
        (void)propertybag_getchildvalue(
            hProperty,
            kString_context,
            Entity_SkillContext,
            &response->context);

        (void)propertybag_getchildvalue(
            hProperty,
            "view",
            Entity_SkillView,
            &response->view);

        if (propertybag_enumarray(
            hProperty,
            "skills",
            Execute_Skill,
            response->pContext))
        {
            LogError("propertybag_getchildvalue(skills) failed");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int
skill_dispatch(
    const char*        pszId,
    PROPERTYBAG_HANDLE hProperty,
    void*              pContext)
{
    unsigned int i;
    int ret = -1;
    SPEECH_CONTEXT *pSC = (SPEECH_CONTEXT*)pContext;

    LogInfo("skill: %s", pszId);

    if (NULL == pContext || 
        NULL == pszId)
    {
        return -1;
    }

    if (pSC->mCallbacks && pSC->mCallbacks->OnSkill)
    {
        pSC->mCallbacks->OnSkill(pSC, pSC->mContext, pszId, hProperty);
    }

    int logSkillMetric = (strncmp(pszId, "skill:", 6) == 0);
    if (logSkillMetric)
    {
        metrics_skill_start(pszId);
    }
    for (i = 0; gIntentHandlers[i].pszId != NULL; i++)
    {
        if (!strcmp(gIntentHandlers[i].pszId, pszId))
        {
            int skillStatus = gIntentHandlers[i].Handler(hProperty, pSC);
            if (logSkillMetric)
            {
                metrics_skill_status(pszId, skillStatus);
            }

            ret = 0;
            break;
        }
    }
    if (logSkillMetric)
    {
        metrics_skill_end(pszId);
    }
    if (ret)
    {
        // Skill unhandled is of Status code -1
        metrics_skill_status(pszId, SKILL_UNHANDLED);
        LogError("Unhandled skill '%s'", pszId);
    }

    return ret;
}

static int context_system_settings_sdk_serialize(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    (void)pContext;
    propertybag_setstringvalue(
        hProperty, 
        kString_version, 
        get_cortana_sdk_version()
    );
    return 0;
}

const char* get_cortana_sdk_version()
{
    const char* version = CORTANA_SDK_VERSION
#if defined(CORTANA_BUILD_ID)
        "." CORTANA_BUILD_ID
#endif
#if defined(CORTANASDK_SDK_BRANCH)
        "." CORTANASDK_SDK_BRANCH
#endif
        ;
    return version;
}

static int context_system_settings_os_serialize(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    (void)pContext;
#if defined(CORTANASDK_PLATFORM)
    propertybag_setstringvalue(hProperty, "platform", CORTANASDK_PLATFORM);
#endif
#if defined(CORTANASDK_PLATFORM_NAME)
    propertybag_setstringvalue(hProperty, "name", CORTANASDK_PLATFORM_NAME);
#endif
#if defined(CORTANASDK_PLATFORM_VERSION)
    propertybag_setstringvalue(hProperty, "version", CORTANASDK_PLATFORM_VERSION);
#endif
    return 0;
}

static int context_system_settings_device_serialize(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;
    STRING_HANDLE   hString;
    if (pSC)
    {
        hString = STRING_new();
        if (hString)
        {
            if (!cortana_get_oem_property(pSC, CORTANA_OEM_PROPERTY_MANUFACTURER, hString))
            {
                propertybag_setstringvalue(hProperty, "manufacturer", STRING_c_str(hString));
            }

            if (!cortana_get_oem_property(pSC, CORTANA_OEM_PROPERTY_MODEL, hString))
            {
                propertybag_setstringvalue(hProperty, "model", STRING_c_str(hString));
            }

            if (!cortana_get_oem_property(pSC, CORTANA_OEM_PROPERTY_VERSION, hString))
            {
                propertybag_setstringvalue(hProperty, "version", STRING_c_str(hString));
            }
            STRING_delete(hString);
        }
    }
    return 0;
}

// Device thumbprint doesn't change when user account switch.
// First, try to hit memory cache.
// If fails, try to read from local storage.
// If fails, require it from CDP API.
// If fails, leave it empty.
const char* get_cdp_device_thumbprint()
{
#if !defined(ThumbprintBufferSize)
#define ThumbprintBufferSize 50
#endif

    static char thumbprint[ThumbprintBufferSize] = {0};
    const char *kKey_device_thumbprint = "device_thumbprint";

    // if memory cache is empty, try to read from local storage
    if (*thumbprint == 0)
    {
        BUFFER_HANDLE hStore = cortana_storage_read(kKey_device_thumbprint);
        if (hStore)
        {
            // Null terminate the end of the string
            size_t size = BUFFER_length(hStore);
            if (!BUFFER_enlarge(hStore, 1))
            {
                BUFFER_u_char(hStore)[size] = 0;
            }

            if (size < sizeof(thumbprint))
            {
                strcpy_s(thumbprint, sizeof(thumbprint), (const char*)BUFFER_u_char(hStore));
            }
            else
            {
                LogError("The file %s's contents are too large, %d chars. Max size is %d. New thumbprint will be automatically generated.", kKey_device_thumbprint, size, sizeof(thumbprint) - 1);
            }
            BUFFER_delete(hStore);
        }
    }

    if (*thumbprint == 0)
    {
        int result = -1;
#ifdef USE_CDP
        // if still empty, require from CDP API
        if (CDPInitialized)
        {
            result = CDPGetDeviceThumbprint(thumbprint, sizeof(thumbprint));
            LogInfo("return code of CDPGetDeviceThumbprint: 0x%08lx", result);
            if (result == 0)
            {
                // only save off for CDP generated thumbprints until we support CDP in Windows build.
                cortana_storage_write(kKey_device_thumbprint, thumbprint, strlen(thumbprint) + 1);
            }
        }
#else
        // generate one.
        uint8_t bThumbPrint[32];
        for (int i = 0; i < sizeof(bThumbPrint); i++)
        {
            bThumbPrint[i] = (unsigned char)rand();
        }

        STRING_HANDLE b64 = Base64_Encode_Bytes(bThumbPrint, sizeof(bThumbPrint));
        if (b64)
        {
            if (STRING_length(b64) < sizeof(thumbprint))
            {
                strcpy_s(thumbprint, sizeof(thumbprint), STRING_c_str(b64));
                result = 0;
            }

            STRING_delete(b64);
        }
#endif // USE_CDP

        metrics_cdp_device_thumbprint(result, thumbprint);
        if (result == 0)
        {
            LogInfo("result from GetDeviceThumbprint: %s", thumbprint);
        }
    }

    return thumbprint;
}

static int skill_settings_getcontext(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    SPEECH_CONTEXT *pSC = (SPEECH_CONTEXT*)pContext;
    
    const char* thumbprint = get_cdp_device_thumbprint();
    propertybag_setstringvalue(hProperty, kString_timezone, get_device_location_info(pContext)->timezone);
    propertybag_setstringvalue(hProperty, "deviceName", pSC->deviceName);
    propertybag_setstringvalue(hProperty, kString_deviceId, thumbprint);
    propertybag_serialize_object(hProperty, "sdk", context_system_settings_sdk_serialize, pContext);
    propertybag_serialize_object(hProperty, "os", context_system_settings_os_serialize, pContext);
    propertybag_serialize_object(hProperty, "device", context_system_settings_device_serialize, pContext);

    return 0;
}

static int skill_system_getcontext(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    (void)pContext;
    propertybag_setnumbervalue(hProperty, kString_version, 1);
    propertybag_serialize_object(hProperty, kString_settings, skill_settings_getcontext, pContext);
    return 0;
}

static int skill_state_getcontext(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    (void)pContext;
    LOCATION_INFO* locInfo = get_device_location_info(pContext);
    propertybag_setnumbervalue(hProperty, kString_latitude, locInfo->latitude);
    propertybag_setnumbervalue(hProperty, kString_longitude, locInfo->longitude);
    return 0;
}

static int skill_location_getcontext(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    (void)pContext;
    propertybag_setnumbervalue(hProperty, kString_version, 1);
    propertybag_serialize_object(hProperty, kString_state, skill_state_getcontext, pContext);
    return 0;
}

static int volume_control_state_getcontext(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;
    Lock(pSC->DataLock);
    propertybag_setnumbervalue(hProperty, kString_volume, pSC->volume.current_volume);
    Unlock(pSC->DataLock);
    return 0;
}

static int skill_volume_control_getcontext(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    propertybag_setnumbervalue(hProperty, kString_version, 1);
    propertybag_serialize_object(hProperty, "state", volume_control_state_getcontext, pContext);
    return 0;
}

static int skills_context(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    int i;

    for (i = 0; i < ARRAYSIZE(kOptional_Service_Serializers); i++)
    {
        kOptional_Service_Serializers[i](hProperty, pContext);
    }

    for (i = 0; i < ARRAYSIZE(kRequired_Service_Serializers); i++)
    {
        (void)propertybag_serialize_object(
            hProperty,
            kRequired_Service_Serializers[i].pszId,
            kRequired_Service_Serializers[i].Handler,
            pContext);
    }

    return 0;
}

typedef struct _SKILL_SERIALIZE_CONTEXT_INFO
{
    CORTANA_HANDLE hCortana;

    // Non-zero to send queued agent events, or zero to skip that.
    int sendEvents;
} SKILL_SERIALIZE_CONTEXT_INFO;

static int skills_devicecontext(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    SKILL_SERIALIZE_CONTEXT_INFO const* const info =
        (SKILL_SERIALIZE_CONTEXT_INFO const*)pContext;

    if (info->sendEvents)
    {
        agent_send_queued_event(
            info->hCortana,
            cortana_system_current_state(),
            hProperty);
    }

    return propertybag_serialize_object(
        hProperty, 
        kString_context, 
        skills_context, 
        info->hCortana);
}

STRING_HANDLE skill_serialize_context(void *pContext, int sendEvents)
{
    SKILL_SERIALIZE_CONTEXT_INFO info = {0};
    info.hCortana = pContext;
    info.sendEvents = sendEvents;

    metrics_skill_contextsent();
    return propertybag_serialize(skills_devicecontext, &info);
}

void cortana_delight_me(CORTANA_HANDLE hCortana)
{
    cortana_system_state current_state = cortana_system_current_state();
    if (current_state == ST_ONLINE_RUN || current_state == ST_OFFLINE_RUN)
    {
        cortana_cancel(hCortana);
        agent_queue_event(hCortana, CORTANA_EVENT_DELIGHT_ME);
    }
    else if (current_state == ST_ERROR)
    {
        cortana_playfile(hCortana, CORTANA_AUDIO_TYPE_VOICE, "C_406_o_oobeerror.wav");
    }
    else if (current_state == ST_AGENT_SETUP)
    {
        cortana_playfile(hCortana, CORTANA_AUDIO_TYPE_VOICE, MAKESOUNDNAME("302_d_wifisetup.wav"));
    }
}

SPEECH_RESULT Content_DispatchBuffer(
    void*       pContext,
    const char* pszPath,
    const char* pszMime,
    const void* pBuffer,
    size_t      bufferSize)
{
    int ret = -1;
    BUFFER_HANDLE hResponseContent = BUFFER_create(
        (unsigned char*)pBuffer,
        bufferSize + 1);
    if (!hResponseContent)
    {
        LogError("BUFFER_create failed");
        return -1;
    }

    BUFFER_u_char(hResponseContent)[bufferSize] = 0;

    ret = Content_Dispatch(
        pContext,
        pszPath,
        pszMime,
        0,
        hResponseContent,
        bufferSize);
    if (ret != CORTANA_ERROR_PENDING)
    {
        BUFFER_delete(hResponseContent);
    }

    return ret;
}

static void Extract_View_And_Callback(const char * pBuffer, SPEECH_CONTEXT* pSC, CORTANA_RESPONSE_DATA* responseData)
{
    JSON_Object * pObject;
    JSON_Value * pVal, *pViewValue;

    if(NULL == pSC->mCallbacks || NULL == pSC->mCallbacks->OnResponse)
    {
        return;
    }

    pVal = json_parse_string(pBuffer);
    if (pVal == NULL)
    {
        return;
    }

    pObject = json_object(pVal);
    pViewValue = json_object_get_value(pObject, "view");

    if(pViewValue != NULL)
    {
        char * string = json_serialize_to_string(pViewValue);
        responseData->viewJSON = string;
        pSC->mCallbacks->OnResponse(pSC, pSC->mContext, responseData);
        json_free_serialized_string(string);
    }

    json_value_free(pVal);
}

// handles "response" API path
static int Handle_Json_Intent_Response(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext)
{
    SkillResponse response = { { 0 }, { 0 }, NULL };
    DeserializeContext * deserializeContext = (DeserializeContext*)pContext;
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)deserializeContext->pContext;

    if (NULL == pContext)
    {
        return -1;
    }

    // required for skill execution.
    response.pContext = pSC;

    Execute_SkillContext(hProperty, &response);

    if (NULL != pSC->mCallbacks && 
        NULL != pSC->mCallbacks->OnResponse)
    {
        CORTANA_RESPONSE_DATA data;
        data.queryText = response.context.queryText;
        data.responseText = response.view.displayText;
        data.ssml = response.view.audio.ssml;
        Extract_View_And_Callback(deserializeContext->pszResponseJson, pSC, &data);
    }

    return 0;
}

// handles "speech.phrase" API path
static int Handle_Json_Speech_Phrase(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext)
{
    if (NULL == pContext)
    {
        return -1;
    }

    DeserializeContext * deserializeContext = (DeserializeContext*)pContext;
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)deserializeContext->pContext;

    if (pSC->mCallbacks  &&  pSC->mCallbacks->OnSpeech)
    {
        const char *displayText = propertybag_getstringvalue(hProperty, "DisplayText");
        if (displayText != NULL)
        {
            pSC->mCallbacks->OnSpeech(pSC, pSC->mContext, displayText, SPEECH_PHRASE_STATE_FINAL);
        }
        else
        {
            // V2 of the speech protocol for partial results
            displayText = propertybag_getstringvalue(hProperty, "Text");
            if (NULL != displayText)
            {
                pSC->mCallbacks->OnSpeech(pSC, pSC->mContext, displayText, SPEECH_PHRASE_STATE_PARTIAL);
            }
        }
    }

    return 0;
}

// handles "turn.start" API path
static int Handle_Json_Turn_Start(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext)
{
    if (NULL == pContext)
    {
        return -1;
    }

    DeserializeContext * deserializeContext = (DeserializeContext*)pContext;

    SkillResponse response = { { 0 }, { 0 }, NULL };
    response.pContext = deserializeContext->pContext;
    Execute_SkillContext(hProperty, &response);

    return 0;
}

SPEECH_RESULT Json_ResponseHandler(
    void*           pContext,
    const char*     pszPath,
    uint8_t*        pBuffer,
    size_t          bufferSize,
    IOBUFFER*       pIoBuffer,
    PCONTENT_ASYNCCOMPLETE_CALLBACK pCB,
    void*           pAsyncContext)
{
    SPEECH_RESULT ret = -1;

    (void)pIoBuffer;
    (void)pCB;
    (void)pAsyncContext;

    if (!pContext || !pBuffer || !pszPath)
    {
        return ret;
    }

    struct JSON_CONTENT_HANDLER *ch;
    for (ch = gJsonContentHandlers; ch->handler != NULL; ++ch)
    {
        if (strcmp(ch->pszPath, pszPath) == 0)
        {
            break;
        }
    }

    if (ch->handler == NULL)
    {
        LogError("The path \"%s\" has no registered JSON handler.  The request was ignored.", pszPath);
        return ret;
    }

    DeserializeContext context;
    context.pContext = pContext;
    context.pszResponseJson = (const char*)pBuffer;

    ret = propertybag_deserialize_json(
        (const char*)pBuffer,
        bufferSize,
        ch->handler,
        &context);

    if (ret)
    {
        LogError("propertybag_deserialize_json failed, path=%s", pszPath);
    }

    return ret;
}

struct INTENT_HANDLERS gIntentHandlers[] =
{
    { "skill:audioPlayer", skill_audioPlayer },
    { "skill:timezone", skill_deviceSettings },
    { kApiPath_SpeechEndDetected, skill_EndOfSpeech },
    { kApiPath_TurnEnd, skill_TurnEnd },
    { "skill:bluetooth", skill_bluetooth },
    { "skill:diagnostics", skill_diagnostics },
    { "skill:timers", skill_timers },
    { "skill:speechRecognizer", skill_speechRecognizer },
    { "skill:call", skill_call },
    { "skill:text", skill_text },
    { "skill:agent", skill_agent },
    { "skill:Alarm", skill_alarms },
    { "skill:notification", skill_notification },
    { "skill:spotify", skill_spotify},
    { NULL, NULL } // terminator
};

struct JSON_CONTENT_HANDLER gJsonContentHandlers[] =
{
    { kApiPath_Response,            Handle_Json_Intent_Response },
    { kApiPath_Speech_Phrase,       Handle_Json_Speech_Phrase },
    { kApiPath_Speech_Hypothesis,   Handle_Json_Speech_Phrase },
    { kApiPath_TurnStart,           Handle_Json_Turn_Start },
    { NULL, NULL } // terminator
};
