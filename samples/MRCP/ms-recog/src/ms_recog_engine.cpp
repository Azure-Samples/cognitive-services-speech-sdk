/*
 *
 * Copyright Microsoft
 *
 */

/*
 * Mandatory rules concerning plugin implementation.
 * 1. Each plugin MUST implement a plugin/engine creator function
 *    with the exact signature and name (the main entry point)
 *        MRCP_PLUGIN_DECLARE(mrcp_engine_t*) mrcp_plugin_create(apr_pool_t *pool)
 * 2. Each plugin MUST declare its version number
 *        MRCP_PLUGIN_VERSION_DECLARE
 * 3. One and only one response MUST be sent back to the received request.
 * 4. Methods (callbacks) of the MRCP engine channel MUST not block.
 *   (asynchronous response can be sent from the context of other thread)
 * 5. Methods (callbacks) of the MPF engine stream MUST not block.
 */

#include "apt_consumer_task.h"
#include "apt_log.h"
#include "mpf_activity_detector.h"
#include "mrcp_recog_engine.h"

#include "config_manager.h"
#include <cassert>
#include <common.h>
#include <condition_variable>
#include <functional>
#include <locale>
#include <memory>
#include <speechapi_cxx.h>
#include <string>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::speechlib;

#define RECOG_ENGINE_TASK_NAME "MS Recog Engine"

typedef struct ms_recog_engine_t ms_recog_engine_t;
typedef struct ms_recog_channel_t ms_recog_channel_t;
typedef struct ms_recog_msg_t ms_recog_msg_t;

/** Declaration of recognizer engine methods */
static apt_bool_t ms_recog_engine_destroy(mrcp_engine_t* engine);
static apt_bool_t ms_recog_engine_open(mrcp_engine_t* engine);
static apt_bool_t ms_recog_engine_close(mrcp_engine_t* engine);
static mrcp_engine_channel_t*
ms_recog_engine_channel_create(mrcp_engine_t* engine, apr_pool_t* pool);

static const struct mrcp_engine_method_vtable_t engine_vtable = {
    ms_recog_engine_destroy, ms_recog_engine_open, ms_recog_engine_close, ms_recog_engine_channel_create
};

/** Declaration of recognizer channel methods */
static apt_bool_t ms_recog_channel_destroy(mrcp_engine_channel_t* channel);
static apt_bool_t ms_recog_channel_open(mrcp_engine_channel_t* channel);
static apt_bool_t ms_recog_channel_close(mrcp_engine_channel_t* channel);
static apt_bool_t ms_recog_channel_request_process(mrcp_engine_channel_t* channel,
                                                   mrcp_message_t* request);

static const struct mrcp_engine_channel_method_vtable_t channel_vtable = {
    ms_recog_channel_destroy, ms_recog_channel_open, ms_recog_channel_close, ms_recog_channel_request_process
};

/** Declaration of recognizer audio stream methods */
static apt_bool_t ms_recog_stream_destroy(mpf_audio_stream_t* stream);
static apt_bool_t ms_recog_stream_open(mpf_audio_stream_t* stream, mpf_codec_t* codec);
static apt_bool_t ms_recog_stream_close(mpf_audio_stream_t* stream);
static apt_bool_t ms_recog_stream_write(mpf_audio_stream_t* stream, const mpf_frame_t* frame);

static apt_bool_t ms_recog_recognition_complete(ms_recog_channel_t* recog_channel,
                                                mrcp_recog_completion_cause_e cause);

static const mpf_audio_stream_vtable_t audio_stream_vtable = {
    ms_recog_stream_destroy, nullptr, nullptr, nullptr, ms_recog_stream_open, ms_recog_stream_close, ms_recog_stream_write, nullptr
};

/** Declaration of ms recognizer engine */
struct ms_recog_engine_t
{
    apt_consumer_task_t* task;
};

// static std::shared_ptr<SpeechRecognizer> createSpeechRecognizer();

struct RecogResource
{
    int count = 0;
    std::string result;

    // std::shared_ptr<SpeechRecognizer> recognizer;
    std::shared_ptr<SpeechConfig> config;
    char* channelId{};
    std::shared_ptr<Audio::PushAudioInputStream> pushStream;
    std::shared_ptr<SpeechRecognizer> recognizer;
    bool recognized;

    RecogResource()
    {
        if(ConfigManager::GetBoolValue(Common::SPEECH_SECTION, Common::SR_USE_LOCAL_CONTAINER))
        {
            static auto localKey =
            ConfigManager::GetStrValue(Common::SPEECH_SECTION, Common::SR_LOCAL_KEY);
            static auto endpoint =
            ConfigManager::GetStrValue(Common::SPEECH_SECTION, Common::SR_LOCAL_ENDPOINT);

            config = SpeechConfig::FromEndpoint(endpoint, localKey);
        }
        else
        {
            // get speech SDK subscription key and region from config.json
            static auto subscriptionKey =
            ConfigManager::GetStrValue(Common::SPEECH_SECTION, Common::SPEECH_SDK_KEY);
            static auto region =
            ConfigManager::GetStrValue(Common::SPEECH_SECTION, Common::SPEECH_SDK_REGION);

            config = SpeechConfig::FromSubscription(subscriptionKey, region);
        }
        
        recognized = false;
    }
};

/** Declaration of ms recognizer channel */
struct ms_recog_channel_t
{
    /** Back pointer to engine */
    ms_recog_engine_t* ms_engine;
    /** Engine channel base */
    mrcp_engine_channel_t* channel;

    /** Active (in-progress) recognition request */
    mrcp_message_t* recog_request;
    /** Pending stop response */
    mrcp_message_t* stop_response;
    /** Indicates whether input timers are started */
    apt_bool_t timers_started;
    /** Voice activity detector */
    mpf_activity_detector_t* detector;
    /** File to write utterance to */
    FILE* audio_out;

    RecogResource* resource;
};

typedef enum
{
    MS_RECOG_MSG_OPEN_CHANNEL,
    MS_RECOG_MSG_CLOSE_CHANNEL,
    MS_RECOG_MSG_REQUEST_PROCESS
} ms_recog_msg_type_e;

/** Declaration of ms recognizer task message */
struct ms_recog_msg_t
{
    ms_recog_msg_type_e type;
    mrcp_engine_channel_t* channel;
    mrcp_message_t* request;
};

static apt_bool_t ms_recog_msg_signal(ms_recog_msg_type_e type,
                                      mrcp_engine_channel_t* channel,
                                      mrcp_message_t* request);
static apt_bool_t ms_recog_msg_process(apt_task_t* task, apt_task_msg_t* msg);

/** Declare this macro to set plugin version */
MRCP_PLUGIN_VERSION_DECLARE

/**
 * Declare this macro to use log routine of the server, plugin is loaded from.
 * Enable/add the corresponding entry in logger.xml to set a cutsom log source
 * priority. <source name="RECOG-PLUGIN" priority="DEBUG" masking="NONE"/>
 */
MRCP_PLUGIN_LOG_SOURCE_IMPLEMENT(RECOG_PLUGIN, "RECOG-PLUGIN")

/** Use custom log source mark */
#define RECOG_LOG_MARK APT_LOG_MARK_DECLARE(RECOG_PLUGIN)


/** Create ms recognizer engine */
MRCP_PLUGIN_DECLARE(mrcp_engine_t*) mrcp_plugin_create(apr_pool_t* pool)
{
    auto* ms_engine =
    static_cast<ms_recog_engine_t*>(apr_palloc(pool, sizeof(ms_recog_engine_t)));

    apt_task_msg_pool_t* msg_pool =
    apt_task_msg_pool_create_dynamic(sizeof(ms_recog_msg_t), pool);
    ms_engine->task = apt_consumer_task_create(ms_engine, msg_pool, pool);
    if(!ms_engine->task)
    {
        return nullptr;
    }
    apt_task_t* task = apt_consumer_task_base_get(ms_engine->task);
    apt_task_name_set(task, RECOG_ENGINE_TASK_NAME);
    apt_task_vtable_t* vtable = apt_task_vtable_get(task);
    if(vtable)
    {
        vtable->process_msg = ms_recog_msg_process;
    }

    /* create engine base */
    return mrcp_engine_create(MRCP_RECOGNIZER_RESOURCE, /* MRCP resource identifier */
                              ms_engine,      /* object to associate */
                              &engine_vtable, /* virtual methods table of engine */
                              pool);          /* pool to allocate memory from */
}

/** Destroy recognizer engine */
static apt_bool_t ms_recog_engine_destroy(mrcp_engine_t* engine)
{
    apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "Microsoft recognition engine destroyed");

    ms_recog_engine_t* ms_engine = static_cast<ms_recog_engine_t*>(engine->obj);
    if(ms_engine->task)
    {
        apt_task_t* task = apt_consumer_task_base_get(ms_engine->task);
        apt_task_destroy(task);
        ms_engine->task = nullptr;
    }
    return true;
}

/** Open recognizer engine */
static apt_bool_t ms_recog_engine_open(mrcp_engine_t* engine)
{
    apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "Microsoft Recognition engine started");

    auto* ms_engine = static_cast<ms_recog_engine_t*>(engine->obj);
    if(ms_engine->task)
    {
        apt_task_t* task = apt_consumer_task_base_get(ms_engine->task);
        apt_task_start(task);
    }
    return mrcp_engine_open_respond(engine, TRUE);
}

/** Close recognizer engine */
static apt_bool_t ms_recog_engine_close(mrcp_engine_t* engine)
{
    ms_recog_engine_t* ms_engine = static_cast<ms_recog_engine_t*>(engine->obj);
    if(ms_engine->task)
    {
        apt_task_t* task = apt_consumer_task_base_get(ms_engine->task);
        apt_task_terminate(task, TRUE);
    }
    return mrcp_engine_close_respond(engine);
}

static mrcp_engine_channel_t*
ms_recog_engine_channel_create(mrcp_engine_t* engine, apr_pool_t* pool)
{
    mpf_stream_capabilities_t* capabilities;
    mpf_termination_t* termination;

    /* create recog channel */
    auto* recog_channel =
    static_cast<ms_recog_channel_t*>(apr_palloc(pool, sizeof(ms_recog_channel_t)));
    recog_channel->ms_engine = static_cast<ms_recog_engine_t*>(engine->obj);
    recog_channel->recog_request = nullptr;
    recog_channel->stop_response = nullptr;
    recog_channel->detector = mpf_activity_detector_create(pool);
    recog_channel->audio_out = nullptr;

    recog_channel->resource = nullptr;
    recog_channel->resource = new RecogResource();

    apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "recognition engine: channel is created");

    capabilities = mpf_sink_stream_capabilities_create(pool);
    mpf_codec_capabilities_add(&capabilities->codecs, MPF_SAMPLE_RATE_16000, "LPCM");
    // Microsoft Cognitive Speech Recognition service only supports 16k 16bit now
    // see https://docs.microsoft.com/en-us/cpp/cognitive-services/speech/audio-audiostreamformat#getwaveformatpcm

    /* create media termination */
    termination =
    mrcp_engine_audio_termination_create(recog_channel, /* object to associate */
                                         &audio_stream_vtable, /* virtual methods table of audio stream */
                                         capabilities, /* stream capabilities */
                                         pool); /* pool to allocate memory from */

    /* create engine channel base */
    recog_channel->channel =
    mrcp_engine_channel_create(engine, /* engine */
                               &channel_vtable, /* virtual methods table of engine channel */
                               recog_channel, /* object to associate */
                               termination,   /* associated media termination */
                               pool);         /* pool to allocate memory from */

    return recog_channel->channel;
}

/** Destroy engine channel */
static apt_bool_t ms_recog_channel_destroy(mrcp_engine_channel_t* channel)
{
    auto* recog_channel = static_cast<ms_recog_channel_t*>(channel->method_obj);

    // Destroy the recognizer
    if(recog_channel->resource != nullptr)
    {
        recog_channel->resource->recognizer.reset();
    }
    return TRUE;
}


/** Open engine channel (asynchronous response MUST be sent)*/
static apt_bool_t ms_recog_channel_open(mrcp_engine_channel_t* channel)
{
    return ms_recog_msg_signal(MS_RECOG_MSG_OPEN_CHANNEL, channel, nullptr);
}

/** Close engine channel (asynchronous response MUST be sent)*/
static apt_bool_t ms_recog_channel_close(mrcp_engine_channel_t* channel)
{
    return ms_recog_msg_signal(MS_RECOG_MSG_CLOSE_CHANNEL, channel, nullptr);
}

/** Process MRCP channel request (asynchronous response MUST be sent)*/
static apt_bool_t ms_recog_channel_request_process(mrcp_engine_channel_t* channel,
                                                   mrcp_message_t* request)
{
    return ms_recog_msg_signal(MS_RECOG_MSG_REQUEST_PROCESS, channel, request);
}

/** Process RECOGNIZE request */
static apt_bool_t ms_recog_channel_recognize(mrcp_engine_channel_t* channel,
                                             mrcp_message_t* request,
                                             mrcp_message_t* response)
{
    auto* recog_channel = static_cast<ms_recog_channel_t*>(channel->method_obj);
    const mpf_codec_descriptor_t* descriptor = mrcp_engine_sink_stream_codec_get(channel);

    if(!descriptor)
    {
        apt_log(RECOG_LOG_MARK, APT_PRIO_WARNING, "Failed to Get Codec Descriptor " APT_SIDRES_FMT,
                MRCP_MESSAGE_SIDRES(request));
        response->start_line.status_code = MRCP_STATUS_CODE_METHOD_FAILED;
        return FALSE;
    }

    recog_channel->timers_started = TRUE;

    /* get recognizer header */
    mrcp_recog_header_t* recog_header =
    static_cast<mrcp_recog_header_t*>(mrcp_resource_header_get(request));
    if(recog_header)
    {
        if(mrcp_resource_header_property_check(request, RECOGNIZER_HEADER_START_INPUT_TIMERS) == TRUE)
        {
            recog_channel->timers_started = recog_header->start_input_timers;
        }
        if(mrcp_resource_header_property_check(request, RECOGNIZER_HEADER_NO_INPUT_TIMEOUT) == TRUE)
        {
            mpf_activity_detector_noinput_timeout_set(recog_channel->detector,
                                                      recog_header->no_input_timeout);
        }
        if(mrcp_resource_header_property_check(request, RECOGNIZER_HEADER_SPEECH_COMPLETE_TIMEOUT) == TRUE)
        {
            mpf_activity_detector_silence_timeout_set(recog_channel->detector,
                                                      recog_header->speech_complete_timeout);
        }
    }

    // Get generic headers
    mrcp_generic_header_t* generic_header =
    static_cast<mrcp_generic_header_t*>(mrcp_generic_header_get(request));
    if(generic_header)
    {
        apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "Processing generic header");

        if(mrcp_generic_header_property_check(request, GENERIC_HEADER_CONTENT_LENGTH) == TRUE)
        {
            apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "Header Content Length is [%d]",
                    generic_header->content_length);
        }

        if(mrcp_generic_header_property_check(request, GENERIC_HEADER_VENDOR_SPECIFIC_PARAMS) == TRUE)
        {
            apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "Vendor specific param is [%s]",
                    generic_header->vendor_specific_params);
        }
    }

    if(recog_channel->resource != nullptr)
    {
        apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "Receive new recognize request");
    }

    response->start_line.request_state = MRCP_REQUEST_STATE_INPROGRESS;

    /* send asynchronous response */
    mrcp_engine_channel_message_send(channel, response);

    recog_channel->recog_request = request;
    return TRUE;
}

/** Process STOP request */
static apt_bool_t ms_recog_channel_stop(mrcp_engine_channel_t* channel,
                                        mrcp_message_t* request,
                                        mrcp_message_t* response)
{
    /* process STOP request */
    auto* recog_channel = static_cast<ms_recog_channel_t*>(channel->method_obj);

    /* store STOP request, make sure there is no more activity and only then send the response */
    recog_channel->stop_response = response;
    return TRUE;
}

/** Process START-INPUT-TIMERS request */
static apt_bool_t ms_recog_channel_timers_start(mrcp_engine_channel_t* channel,
                                                mrcp_message_t* request,
                                                mrcp_message_t* response)
{
    auto* recog_channel = static_cast<ms_recog_channel_t*>(channel->method_obj);
    recog_channel->timers_started = TRUE;
    return mrcp_engine_channel_message_send(channel, response);
}

/** Dispatch MRCP request */
static apt_bool_t ms_recog_channel_request_dispatch(mrcp_engine_channel_t* channel,
                                                    mrcp_message_t* request)
{
    apt_bool_t processed = FALSE;
    mrcp_message_t* response = mrcp_response_create(request, request->pool);
    switch(request->start_line.method_id)
    {
    case RECOGNIZER_SET_PARAMS:
        break;
    case RECOGNIZER_GET_PARAMS:
        break;
    case RECOGNIZER_DEFINE_GRAMMAR:
        break;
    case RECOGNIZER_RECOGNIZE:
        processed = ms_recog_channel_recognize(channel, request, response);
        break;
    case RECOGNIZER_GET_RESULT:
        break;
    case RECOGNIZER_START_INPUT_TIMERS:
        processed = ms_recog_channel_timers_start(channel, request, response);
        break;
    case RECOGNIZER_STOP:
        processed = ms_recog_channel_stop(channel, request, response);
        break;
    default:
        break;
    }
    if(processed == FALSE)
    {
        /* send asynchronous response for not handled request */
        mrcp_engine_channel_message_send(channel, response);
    }
    return TRUE;
}

/** Callback is called from MPF engine context to destroy any additional data associated with audio stream */
static apt_bool_t ms_recog_stream_destroy(mpf_audio_stream_t* stream)
{
    return TRUE;
}

/** Callback is called from MPF engine context to perform any action before open */
static apt_bool_t ms_recog_stream_open(mpf_audio_stream_t* stream, mpf_codec_t* codec)
{
    auto recog_channel = static_cast<ms_recog_channel_t*>(stream->obj);
    auto resource = recog_channel->resource;
    resource->pushStream = Audio::AudioInputStream::CreatePushStream();
    const auto audioInput = Audio::AudioConfig::FromStreamInput(resource->pushStream);
    resource->recognizer = SpeechRecognizer::FromConfig(resource->config, audioInput);

    resource->recognizer->Recognizing.Connect([](const SpeechRecognitionEventArgs& e) noexcept {
        apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "Recognizing: %s", e.Result->Text.c_str());
    });

    // Note the recog_channel must be captured as a variable
    resource->recognizer->Recognized.Connect(
    [recog_channel](const SpeechRecognitionEventArgs& e) {
        if(e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "Recognized: %s",
                    e.Result->Text.c_str());
            if(e.Result->Text.empty() ||
               (e.Result->Text.find_first_not_of(' ') == std::string::npos))
            {
                // empty
                // use continuous recognition to avoid empty result
            }
            else
            {
                recog_channel->resource->result = e.Result->Text;
                ms_recog_recognition_complete(recog_channel, RECOGNIZER_COMPLETION_CAUSE_SUCCESS);
                try
                {
                    recog_channel->resource->recognizer->StopContinuousRecognitionAsync();
                }
                catch(...)
                {
                    apt_log(RECOG_LOG_MARK, APT_PRIO_DEBUG,
                            "Stop recognizer failed. Maybe reset already.");
                }
            }
        }
        else if(e.Result->Reason == ResultReason::NoMatch)
        {
            apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "NO MATCH: Speech could not be recognized.");
            ms_recog_recognition_complete(recog_channel, RECOGNIZER_COMPLETION_CAUSE_NO_MATCH);
        }
    });

    resource->recognizer->Canceled.Connect(
    [recog_channel](const SpeechRecognitionCanceledEventArgs& e) {
        apt_log(RECOG_LOG_MARK, APT_PRIO_WARNING, "Speech recognition cancelled.");
        if(e.Reason == CancellationReason::Error)
        {
            apt_log(RECOG_LOG_MARK, APT_PRIO_ERROR, "Speech recognition error, Error Code: [%d], Error Details: [%s].",
                    static_cast<int>(e.ErrorCode), e.ErrorDetails.c_str());
            apt_log(RECOG_LOG_MARK, APT_PRIO_WARNING, "Did you update the subscription info?");
        }
        ms_recog_recognition_complete(recog_channel, RECOGNIZER_COMPLETION_CAUSE_ERROR);
    });

    resource->recognizer->StartContinuousRecognitionAsync();
    return TRUE;
}

/** Callback is called from MPF engine context to perform any action after close */
static apt_bool_t ms_recog_stream_close(mpf_audio_stream_t* stream)
{
    return TRUE;
}

/* Load recognition result */
static apt_bool_t ms_recog_result_load(ms_recog_channel_t* recog_channel, mrcp_message_t* message)
{
    if(recog_channel->resource == nullptr)
    {
        return false;
    }

    apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "load recognized result.");


    const auto body = &message->body;

    const auto result = recog_channel->resource->result.c_str();
    body->buf = apr_psprintf(message->pool,
                             "<?xml version=\"1.0\"?>\n"
                             "<result>\n"
                             "  <interpretation confidence=\"%d\">\n"
                             "    <instance>%s</instance>\n"
                             "    <input mode=\"speech\">%s</input>\n"
                             "  </interpretation>\n"
                             "</result>\n",
                             99, result, result);

    if(body->buf)
    {
        mrcp_generic_header_t* generic_header;
        generic_header = mrcp_generic_header_prepare(message);
        if(generic_header)
        {
            /* set content type */
            apt_string_assign(&generic_header->content_type,
                              "application/x-nlsml", message->pool);
            mrcp_generic_header_property_add(message, GENERIC_HEADER_CONTENT_TYPE);
        }

        body->length = strlen(body->buf);
    }

    return TRUE;
}

/* Raise RECOGNITION-COMPLETE event */
static apt_bool_t ms_recog_recognition_complete(ms_recog_channel_t* recog_channel,
                                                mrcp_recog_completion_cause_e cause)
{
    apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "complete recognition");

    if(recog_channel->recog_request == nullptr)
    {
        return false;
    }
    /* create RECOGNITION-COMPLETE event */
    mrcp_message_t* message =
    mrcp_event_create(recog_channel->recog_request, RECOGNIZER_RECOGNITION_COMPLETE,
                      recog_channel->recog_request->pool);
    if(!message)
    {
        return FALSE;
    }

    /* get/allocate recognizer header */
    mrcp_recog_header_t* recog_header =
    static_cast<mrcp_recog_header_t*>(mrcp_resource_header_prepare(message));
    if(recog_header)
    {
        /* set completion cause */
        recog_header->completion_cause = cause;
        mrcp_resource_header_property_add(message, RECOGNIZER_HEADER_COMPLETION_CAUSE);
    }

    /* set request state */
    message->start_line.request_state = MRCP_REQUEST_STATE_COMPLETE;

    if(cause == RECOGNIZER_COMPLETION_CAUSE_SUCCESS)
    {
        apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "recognition complete: try to load recog results");
        ms_recog_result_load(recog_channel, message);
    }

    recog_channel->recog_request = nullptr;
    /* send async event */
    return mrcp_engine_channel_message_send(recog_channel->channel, message);
}

/** Callback is called from MPF engine context to write/send new frame */
static apt_bool_t ms_recog_stream_write(mpf_audio_stream_t* stream, const mpf_frame_t* frame)
{
    auto* recog_channel = static_cast<ms_recog_channel_t*>(stream->obj);
    if(recog_channel->stop_response)
    {
        /* send asynchronous response to STOP request */
        mrcp_engine_channel_message_send(recog_channel->channel, recog_channel->stop_response);
        recog_channel->stop_response = nullptr;
        recog_channel->recog_request = nullptr;
        return TRUE;
    }
    if(frame->codec_frame.size)
    {
        recog_channel->resource->pushStream->Write(static_cast<uint8_t*>(
                                                   frame->codec_frame.buffer),
                                                   frame->codec_frame.size);
    }
    return TRUE;
}

static apt_bool_t ms_recog_msg_signal(ms_recog_msg_type_e type,
                                      mrcp_engine_channel_t* channel,
                                      mrcp_message_t* request)
{
    apt_bool_t status = FALSE;
    auto* ms_channel = static_cast<ms_recog_channel_t*>(channel->method_obj);
    auto ms_engine = ms_channel->ms_engine;
    apt_task_t* task = apt_consumer_task_base_get(ms_engine->task);
    apt_task_msg_t* msg = apt_task_msg_get(task);
    if(msg)
    {
        msg->type = TASK_MSG_USER;
        auto* ms_msg = (ms_recog_msg_t*)msg->data;

        ms_msg->type = type;
        ms_msg->channel = channel;
        ms_msg->request = request;
        status = apt_task_msg_signal(task, msg);
    }
    return status;
}

static apt_bool_t ms_recog_msg_process(apt_task_t* task, apt_task_msg_t* msg)
{
    auto* ms_msg = (ms_recog_msg_t*)msg->data;
    switch(ms_msg->type)
    {
    case MS_RECOG_MSG_OPEN_CHANNEL:
        /* open channel and send async response */
        mrcp_engine_channel_open_respond(ms_msg->channel, TRUE);
        break;
    case MS_RECOG_MSG_CLOSE_CHANNEL:
    {
        apt_log(RECOG_LOG_MARK, APT_PRIO_INFO, "ms_recog_msg_process: channel is closed");

        /* close channel, make sure there is no activity and send async response */
        auto* recog_channel = static_cast<ms_recog_channel_t*>(ms_msg->channel->method_obj);

        if(recog_channel->resource != nullptr && recog_channel->resource->recognizer != nullptr)
        {
            recog_channel->resource->recognizer.reset();
        }

        mrcp_engine_channel_close_respond(ms_msg->channel);
        break;
    }
    case MS_RECOG_MSG_REQUEST_PROCESS:
        ms_recog_channel_request_dispatch(ms_msg->channel, ms_msg->request);
        break;
    default:
        break;
    }
    return TRUE;
}
