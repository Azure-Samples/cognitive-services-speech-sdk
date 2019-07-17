/*
 * Copyright Microsoft
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
#include "mpf_buffer.h"
#include "mrcp_synth_engine.h"
#include <memory>

#include "speechapi_cxx.h"
#include "synthesizer_pool.h"

using namespace Microsoft::CognitiveServices::Speech;

#define SYNTH_ENGINE_TASK_NAME "MS Synth Engine"

typedef struct ms_synth_engine_t ms_synth_engine_t;
typedef struct ms_synth_channel_t ms_synth_channel_t;
typedef struct ms_synth_msg_t ms_synth_msg_t;

/** Declaration of synthesizer engine methods */
static apt_bool_t ms_synth_engine_destroy(mrcp_engine_t* engine);
static apt_bool_t ms_synth_engine_open(mrcp_engine_t* engine) noexcept;
static apt_bool_t ms_synth_engine_close(mrcp_engine_t* engine);
static mrcp_engine_channel_t*
ms_synth_engine_channel_create(mrcp_engine_t* engine, apr_pool_t* pool);

static const struct mrcp_engine_method_vtable_t engine_vtable = {
    ms_synth_engine_destroy, ms_synth_engine_open, ms_synth_engine_close, ms_synth_engine_channel_create
};


/** Declaration of synthesizer channel methods */
static apt_bool_t ms_synth_channel_destroy(mrcp_engine_channel_t* channel);
static apt_bool_t ms_synth_channel_open(mrcp_engine_channel_t* channel);
static apt_bool_t ms_synth_channel_close(mrcp_engine_channel_t* channel);
static apt_bool_t ms_synth_channel_request_process(mrcp_engine_channel_t* channel,
                                                   mrcp_message_t* request);

static const struct mrcp_engine_channel_method_vtable_t channel_vtable = {
    ms_synth_channel_destroy, ms_synth_channel_open, ms_synth_channel_close, ms_synth_channel_request_process
};

/** Declaration of synthesizer audio stream methods */
static apt_bool_t ms_synth_stream_destroy(mpf_audio_stream_t* stream);
static apt_bool_t ms_synth_stream_open(mpf_audio_stream_t* stream, mpf_codec_t* codec);
static apt_bool_t ms_synth_stream_close(mpf_audio_stream_t* stream);
static apt_bool_t ms_synth_stream_read(mpf_audio_stream_t* stream, mpf_frame_t* frame);

static const mpf_audio_stream_vtable_t audio_stream_vtable = { ms_synth_stream_destroy,
                                                               ms_synth_stream_open,
                                                               ms_synth_stream_close,
                                                               ms_synth_stream_read,
                                                               nullptr,
                                                               nullptr,
                                                               nullptr,
                                                               nullptr };

/** Declaration of synthesizer engine */
struct ms_synth_engine_t
{
    apt_consumer_task_t* task;
};

struct SynthResource
{
    std::string caller;
};

/** Declaration of synthesizer channel */
struct ms_synth_channel_t
{
    /** Back pointer to engine */
    ms_synth_engine_t* ms_engine;
    /** Engine channel base */
    mrcp_engine_channel_t* channel;

    /** Active (in-progress) speak request */
    mrcp_message_t* speak_request;
    /** Pending stop response */
    mrcp_message_t* stop_response;
    /** Estimated time to complete */
    apr_size_t time_to_complete;
    /** Is paused */
    apt_bool_t paused;

    SynthResource* resource;

    mpf_buffer_t* audio_buffer;
    std::shared_ptr<AudioDataStream> audio_stream;

    /** Indicate if the synthesize is cancelled */
    apt_bool_t cancelled;
};

typedef enum
{
    MS_SYNTH_MSG_OPEN_CHANNEL,
    MS_SYNTH_MSG_CLOSE_CHANNEL,
    MS_SYNTH_MSG_REQUEST_PROCESS
} ms_synth_msg_type_e;

/** Declaration of synthesizer task message */
struct ms_synth_msg_t
{
    ms_synth_msg_type_e type;
    mrcp_engine_channel_t* channel;
    mrcp_message_t* request;
};

static apt_bool_t ms_synth_msg_signal(ms_synth_msg_type_e type,
                                      mrcp_engine_channel_t* channel,
                                      mrcp_message_t* request);
static apt_bool_t ms_synth_msg_process(apt_task_t* task, apt_task_msg_t* msg);

/** Declare this macro to set plugin version */
MRCP_PLUGIN_VERSION_DECLARE

/**
 * Declare this macro to use log routine of the server, plugin is loaded from.
 * Enable/add the corresponding entry in logger.xml to set a custom log source
 * priority. <source name="SYNTH-PLUGIN" priority="DEBUG" masking="NONE"/>
 */
MRCP_PLUGIN_LOG_SOURCE_IMPLEMENT(SYNTH_PLUGIN, "SYNTH-PLUGIN")

/** Use custom log source mark */
#define SYNTH_LOG_MARK APT_LOG_MARK_DECLARE(SYNTH_PLUGIN)

/** Create MS synthesizer engine */
MRCP_PLUGIN_DECLARE(mrcp_engine_t*) mrcp_plugin_create(apr_pool_t* pool)
{
    /* create synthesis engine */
    ms_synth_engine_t* ms_engine =
    static_cast<ms_synth_engine_t*>(apr_palloc(pool, sizeof(ms_synth_engine_t)));
    apt_task_t* task;
    apt_task_vtable_t* vtable;
    apt_task_msg_pool_t* msg_pool;

    /* create task/thread to run engine in the context of this task */
    msg_pool = apt_task_msg_pool_create_dynamic(sizeof(ms_synth_msg_t), pool);
    ms_engine->task = apt_consumer_task_create(ms_engine, msg_pool, pool);
    if(!ms_engine->task)
    {
        return nullptr;
    }
    task = apt_consumer_task_base_get(ms_engine->task);
    apt_task_name_set(task, SYNTH_ENGINE_TASK_NAME);
    vtable = apt_task_vtable_get(task);
    if(vtable)
    {
        vtable->process_msg = ms_synth_msg_process;
    }


    /* create engine base */
    return mrcp_engine_create(MRCP_SYNTHESIZER_RESOURCE, /* MRCP resource identifier */
                              ms_engine,      /* object to associate */
                              &engine_vtable, /* virtual methods table of engine */
                              pool);          /* pool to allocate memory from */
}

/** Destroy synthesizer engine */
static apt_bool_t ms_synth_engine_destroy(mrcp_engine_t* engine)
{
    ms_synth_engine_t* ms_engine = static_cast<ms_synth_engine_t*>(engine->obj);
    if(ms_engine->task)
    {
        apt_task_t* task = apt_consumer_task_base_get(ms_engine->task);
        apt_task_destroy(task);
        ms_engine->task = nullptr;
    }
    return TRUE;
}

/** Open synthesizer engine */
static apt_bool_t ms_synth_engine_open(mrcp_engine_t* engine) noexcept
{
    const auto ms_engine = static_cast<ms_synth_engine_t*>(engine->obj);
    if(ms_engine->task)
    {
        apt_task_t* task = apt_consumer_task_base_get(ms_engine->task);
        apt_task_start(task);
    }
    return mrcp_engine_open_respond(engine, TRUE);
}

/** Close synthesizer engine */
static apt_bool_t ms_synth_engine_close(mrcp_engine_t* engine)
{
    ms_synth_engine_t* ms_engine = static_cast<ms_synth_engine_t*>(engine->obj);
    if(ms_engine->task)
    {
        apt_task_t* task = apt_consumer_task_base_get(ms_engine->task);
        apt_task_terminate(task, TRUE);
    }
    return mrcp_engine_close_respond(engine);
}

/** Create ms synthesizer channel derived from engine channel base */
static mrcp_engine_channel_t*
ms_synth_engine_channel_create(mrcp_engine_t* engine, apr_pool_t* pool)
{
    mpf_stream_capabilities_t* capabilities;
    mpf_termination_t* termination;

    /* create synth channel */
    ms_synth_channel_t* synth_channel =
    static_cast<ms_synth_channel_t*>(apr_palloc(pool, sizeof(ms_synth_channel_t)));
    // the memset is required for the shared_ptr
    memset(static_cast<void*>(synth_channel), 0, sizeof(ms_synth_channel_t));
    synth_channel->ms_engine = static_cast<ms_synth_engine_t*>(engine->obj);
    synth_channel->speak_request = nullptr;
    synth_channel->stop_response = nullptr;
    synth_channel->time_to_complete = 0;
    synth_channel->paused = FALSE;
    synth_channel->audio_buffer = nullptr;
    synth_channel->audio_stream.reset();
    synth_channel->cancelled = FALSE;
    synth_channel->resource = new SynthResource();

    capabilities = mpf_source_stream_capabilities_create(pool);
    // use 16kHz
    mpf_codec_capabilities_add(&capabilities->codecs, MPF_SAMPLE_RATE_16000, "LPCM");

    /* create media termination */
    termination =
    mrcp_engine_audio_termination_create(synth_channel, /* object to associate */
                                         &audio_stream_vtable, /* virtual methods table of audio stream */
                                         capabilities, /* stream capabilities */
                                         pool); /* pool to allocate memory from */

    /* create engine channel base */
    synth_channel->channel =
    mrcp_engine_channel_create(engine, /* engine */
                               &channel_vtable, /* virtual methods table of engine channel */
                               synth_channel, /* object to associate */
                               termination,   /* associated media termination */
                               pool);         /* pool to allocate memory from */

    synth_channel->audio_buffer = mpf_buffer_create(pool);
    return synth_channel->channel;
}

/** Destroy engine channel */
static apt_bool_t ms_synth_channel_destroy(mrcp_engine_channel_t* channel)
{
    /* nothing to destroy */
    return TRUE;
}

/** Open engine channel (asynchronous response MUST be sent)*/
static apt_bool_t ms_synth_channel_open(mrcp_engine_channel_t* channel)
{
    return ms_synth_msg_signal(MS_SYNTH_MSG_OPEN_CHANNEL, channel, nullptr);
}

/** Close engine channel (asynchronous response MUST be sent)*/
static apt_bool_t ms_synth_channel_close(mrcp_engine_channel_t* channel)
{
    return ms_synth_msg_signal(MS_SYNTH_MSG_CLOSE_CHANNEL, channel, nullptr);
}

/** Process MRCP channel request (asynchronous response MUST be sent)*/
static apt_bool_t ms_synth_channel_request_process(mrcp_engine_channel_t* channel,
                                                   mrcp_message_t* request)
{
    return ms_synth_msg_signal(MS_SYNTH_MSG_REQUEST_PROCESS, channel, request);
}

static bool ConvertTextToAudio(ms_synth_channel_t* channel, const char* src_text, bool ssml)
{
    apt_log(APT_LOG_MARK, APT_PRIO_INFO, "[MS TTS] creating synthesizer...");

    // auto synthesizer = SpeechSynthesizer::FromConfig(speechConfig, nullptr);
    auto synthesizer = SynthesizerPool::borrowSynthesizer();
    synthesizer->SynthesisStarted.Connect([](const SpeechSynthesisEventArgs& e) noexcept {
        apt_log(APT_LOG_MARK, APT_PRIO_INFO, "[MS TTS] synthesis started.");
    });
    synthesizer->Synthesizing.Connect([](const SpeechSynthesisEventArgs& e) noexcept {
        apt_log(APT_LOG_MARK, APT_PRIO_INFO, "[MS TTS] synthesizing (audio data being received).");
    });
    synthesizer->SynthesisCanceled.Connect(
    [&synthesizer, &channel](const SpeechSynthesisEventArgs& e) {
        apt_log(
        APT_LOG_MARK, APT_PRIO_WARNING, "[MS TTS] synthesis canceled %s.",
        SpeechSynthesisCancellationDetails::FromResult(e.Result)->ErrorDetails.c_str());
        const auto cancellation =
        SpeechSynthesisCancellationDetails::FromResult(e.Result);
        if(CancellationReason::Error == cancellation->Reason)
        {
            apt_log(APT_LOG_MARK, APT_PRIO_ERROR,
                    "[MS TTS] CANCELED: ErrorCode=%d; ErrorDetails: [%s].",
                    static_cast<int>(cancellation->ErrorCode),
                    cancellation->ErrorDetails.c_str());
            apt_log(APT_LOG_MARK, APT_PRIO_WARNING,
                    "[MS TTS] CANCELED: Did you update the subscription info?");
        }
        // change the indicator
        channel->cancelled = TRUE;
        // destroy the synthesizer and return it to pool.
        synthesizer.reset();
        SynthesizerPool::returnSynthesizer(synthesizer);
    });
    synthesizer->SynthesisCompleted.Connect([&synthesizer](const SpeechSynthesisEventArgs& e) {
        apt_log(APT_LOG_MARK, APT_PRIO_INFO, "[MS TTS] synthesis completed, audio length: %d.",
                e.Result->GetAudioLength());
        synthesizer->SynthesisStarted.DisconnectAll();
        synthesizer->Synthesizing.DisconnectAll();
        synthesizer->SynthesisCanceled.DisconnectAll();
        synthesizer->SynthesisCompleted.DisconnectAll();
        SynthesizerPool::returnSynthesizer(synthesizer);
    });
    apt_log(APT_LOG_MARK, APT_PRIO_INFO, "[MS TTS] synthesizer created.");
    std::shared_ptr<SpeechSynthesisResult> result;
    if (ssml)
    {
        result = synthesizer->StartSpeakingSsml(src_text);
    }
    else
    {
        result = synthesizer->StartSpeakingText(src_text);
    }
    channel->audio_stream = AudioDataStream::FromResult(result);
    return TRUE;
}

/** Process SPEAK request */
static apt_bool_t ms_synth_channel_speak(mrcp_engine_channel_t* channel,
                                         mrcp_message_t* request,
                                         mrcp_message_t* response)
{


    apt_log(APT_LOG_MARK, APT_PRIO_INFO, "TTS service start to process speak request");
    apt_str_t* body;

    auto* synth_channel = static_cast<ms_synth_channel_t*>(channel->method_obj);
    const mpf_codec_descriptor_t* descriptor =
    mrcp_engine_source_stream_codec_get(channel);

    if(!descriptor)
    {
        apt_log(SYNTH_LOG_MARK, APT_PRIO_WARNING, "Failed to Get Codec Descriptor " APT_SIDRES_FMT,
                MRCP_MESSAGE_SIDRES(request));
        response->start_line.status_code = MRCP_STATUS_CODE_METHOD_FAILED;

        return FALSE;
    }

    synth_channel->speak_request = request;
    bool if_request_ssml = false;
    const auto generic_header = mrcp_generic_header_get(synth_channel->speak_request);
    if(mrcp_generic_header_property_check(synth_channel->speak_request,
                                          GENERIC_HEADER_CONTENT_TYPE) == TRUE)
    {
        if(strcmp(generic_header->content_type.buf,
                  "application/synthesis+ssml") == 0 ||
           strcmp(generic_header->content_type.buf, "application/ssml+xml") == 0)
        {
            apt_log(SYNTH_LOG_MARK, APT_PRIO_INFO, "request type is ssml");
            if_request_ssml = true;
        }
        else
        {
            apt_log(SYNTH_LOG_MARK, APT_PRIO_INFO, "request type is plain text.");
        }
    }
    body = &synth_channel->speak_request->body;
    if(!body->length)
    {
        synth_channel->speak_request = nullptr;
        mrcp_engine_channel_message_send(synth_channel->channel, response);

        return FALSE;
    }

    synth_channel->time_to_complete = 0;

    response->start_line.request_state = MRCP_REQUEST_STATE_INPROGRESS;
    /* send asynchronous response */
    mrcp_engine_channel_message_send(channel, response);

    ConvertTextToAudio(synth_channel, body->buf, if_request_ssml);
    mpf_buffer_event_write(synth_channel->audio_buffer, MEDIA_FRAME_TYPE_EVENT);
    return TRUE;
}

/** Process STOP request */
static apt_bool_t ms_synth_channel_stop(mrcp_engine_channel_t* channel,
                                        mrcp_message_t* request,
                                        mrcp_message_t* response)
{
    ms_synth_channel_t* synth_channel =
    static_cast<ms_synth_channel_t*>(channel->method_obj);
    /* store the request, make sure there is no more activity and only then send the response */
    synth_channel->stop_response = response;
    return TRUE;
}

/** Process PAUSE request */
static apt_bool_t ms_synth_channel_pause(mrcp_engine_channel_t* channel,
                                         mrcp_message_t* request,
                                         mrcp_message_t* response)
{
    ms_synth_channel_t* synth_channel =
    static_cast<ms_synth_channel_t*>(channel->method_obj);
    synth_channel->paused = TRUE;
    /* send asynchronous response */
    mrcp_engine_channel_message_send(channel, response);
    return TRUE;
}

/** Process RESUME request */
static apt_bool_t ms_synth_channel_resume(mrcp_engine_channel_t* channel,
                                          mrcp_message_t* request,
                                          mrcp_message_t* response)
{
    ms_synth_channel_t* synth_channel =
    static_cast<ms_synth_channel_t*>(channel->method_obj);
    synth_channel->paused = FALSE;
    /* send asynchronous response */
    mrcp_engine_channel_message_send(channel, response);
    return TRUE;
}

/** Process SET-PARAMS request */
static apt_bool_t ms_synth_channel_set_params(mrcp_engine_channel_t* channel,
                                              mrcp_message_t* request,
                                              mrcp_message_t* response)
{
    return TRUE;
}

/** Process GET-PARAMS request */
static apt_bool_t ms_synth_channel_get_params(mrcp_engine_channel_t* channel,
                                              mrcp_message_t* request,
                                              mrcp_message_t* response)
{
    mrcp_synth_header_t* req_synth_header;
    /* get synthesizer header */
    req_synth_header = (mrcp_synth_header_t*)mrcp_resource_header_get(request);
    if(req_synth_header)
    {
        mrcp_synth_header_t* res_synth_header =
        (mrcp_synth_header_t*)mrcp_resource_header_prepare(response);
        /* check voice age header */
        if(mrcp_resource_header_property_check(request, SYNTHESIZER_HEADER_VOICE_AGE) == TRUE)
        {
            res_synth_header->voice_param.age = 25;
            mrcp_resource_header_property_add(response, SYNTHESIZER_HEADER_VOICE_AGE);
        }
        /* check voice name header */
        if(mrcp_resource_header_property_check(request, SYNTHESIZER_HEADER_VOICE_NAME) == TRUE)
        {
            apt_string_set(&res_synth_header->voice_param.name, "David");
            mrcp_resource_header_property_add(response, SYNTHESIZER_HEADER_VOICE_NAME);
        }
    }

    /* send asynchronous response */
    mrcp_engine_channel_message_send(channel, response);
    return TRUE;
}

/** Dispatch MRCP request */
static apt_bool_t ms_synth_channel_request_dispatch(mrcp_engine_channel_t* channel,
                                                    mrcp_message_t* request)
{
    apt_bool_t processed = FALSE;
    mrcp_message_t* response = mrcp_response_create(request, request->pool);
    switch(request->start_line.method_id)
    {
    case SYNTHESIZER_SET_PARAMS:
        processed = ms_synth_channel_set_params(channel, request, response);
        break;
    case SYNTHESIZER_GET_PARAMS:
        processed = ms_synth_channel_get_params(channel, request, response);
        break;
    case SYNTHESIZER_SPEAK:
        processed = ms_synth_channel_speak(channel, request, response);
        break;
    case SYNTHESIZER_STOP:
        processed = ms_synth_channel_stop(channel, request, response);
        break;
    case SYNTHESIZER_PAUSE:
        processed = ms_synth_channel_pause(channel, request, response);
        break;
    case SYNTHESIZER_RESUME:
        processed = ms_synth_channel_resume(channel, request, response);
        break;
    case SYNTHESIZER_BARGE_IN_OCCURRED:
        processed = ms_synth_channel_stop(channel, request, response);
        break;
    case SYNTHESIZER_CONTROL:
        break;
    case SYNTHESIZER_DEFINE_LEXICON:
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
static apt_bool_t ms_synth_stream_destroy(mpf_audio_stream_t* stream)
{
    return TRUE;
}

/** Callback is called from MPF engine context to perform any action before open */
static apt_bool_t ms_synth_stream_open(mpf_audio_stream_t* stream, mpf_codec_t* codec)
{
    return TRUE;
}

/** Callback is called from MPF engine context to perform any action after close */
static apt_bool_t ms_synth_stream_close(mpf_audio_stream_t* stream)
{
    return TRUE;
}


/** Callback is called from MPF engine context to read/get new frame */
static apt_bool_t ms_synth_stream_read(mpf_audio_stream_t* stream, mpf_frame_t* frame)
{
    ms_synth_channel_t* synth_channel = static_cast<ms_synth_channel_t*>(stream->obj);
    /* check if STOP was requested */
    if(synth_channel->stop_response)
    {
        /* send asynchronous response to STOP request */
        mrcp_engine_channel_message_send(synth_channel->channel, synth_channel->stop_response);
        synth_channel->stop_response = nullptr;
        synth_channel->speak_request = nullptr;
        synth_channel->paused = FALSE;
        return TRUE;
    }

    /* check if there is active SPEAK request and it isn't in paused state */
    if(synth_channel->speak_request && synth_channel->paused == FALSE)
    {
        /* normal processing */
        apt_bool_t completed = FALSE;

        if(nullptr != synth_channel->audio_stream)
        {
            const auto size = frame->codec_frame.size;
            if(synth_channel->audio_stream->GetStatus() == StreamStatus::AllData)
            {
                if(synth_channel->audio_stream->ReadData(static_cast<uint8_t*>(
                                                         frame->codec_frame.buffer),
                                                         frame->codec_frame.size) > 0)
                {
                    frame->type |= MEDIA_FRAME_TYPE_AUDIO;
                }
                else
                {
                    completed = TRUE;
                }
            }
            else if(synth_channel->audio_stream->GetStatus() == StreamStatus::PartialData &&
                    synth_channel->audio_stream->CanReadData(size))
            {
                synth_channel->audio_stream->ReadData(static_cast<uint8_t*>(
                                                      frame->codec_frame.buffer),
                                                      frame->codec_frame.size);
                frame->type |= MEDIA_FRAME_TYPE_AUDIO;
            }
        }

        if(completed)
        {
            /* raise SPEAK-COMPLETE event */
            mrcp_message_t* message =
            mrcp_event_create(synth_channel->speak_request, SYNTHESIZER_SPEAK_COMPLETE,
                              synth_channel->speak_request->pool);
            if(message)
            {
                /* get/allocate synthesizer header */
                mrcp_synth_header_t* synth_header =
                (mrcp_synth_header_t*)mrcp_resource_header_prepare(message);
                if(synth_header)
                {
                    /* set completion cause */
                    synth_header->completion_cause = SYNTHESIZER_COMPLETION_CAUSE_NORMAL;
                    mrcp_resource_header_property_add(message, SYNTHESIZER_HEADER_COMPLETION_CAUSE);
                }
                /* set request state */
                message->start_line.request_state = MRCP_REQUEST_STATE_COMPLETE;

                synth_channel->speak_request = nullptr;
                /* send async event */
                apt_log(APT_LOG_MARK, APT_PRIO_INFO, "[tts] send stream completed.");
                mrcp_engine_channel_message_send(synth_channel->channel, message);
            }
        }

        if(synth_channel->cancelled)
        {
            synth_channel->cancelled = FALSE;
            /* raise SPEAK-COMPLETE event */
            mrcp_message_t* message =
            mrcp_event_create(synth_channel->speak_request, SYNTHESIZER_SPEAK_COMPLETE,
                              synth_channel->speak_request->pool);
            if(message)
            {
                /* get/allocate synthesizer header */
                auto* synth_header =
                static_cast<mrcp_synth_header_t*>(mrcp_resource_header_prepare(message));
                if(synth_header)
                {
                    /* set completion cause */
                    synth_header->completion_cause = SYNTHESIZER_COMPLETION_CAUSE_ERROR;
                    mrcp_resource_header_property_add(message, SYNTHESIZER_HEADER_COMPLETION_CAUSE);
                }
                /* set request state */
                message->start_line.request_state = MRCP_REQUEST_STATE_COMPLETE;

                synth_channel->speak_request = nullptr;
                /* send async event */
                mrcp_engine_channel_message_send(synth_channel->channel, message);
            }
        }

        return TRUE;
    }
    return TRUE;
}

static apt_bool_t ms_synth_msg_signal(ms_synth_msg_type_e type,
                                      mrcp_engine_channel_t* channel,
                                      mrcp_message_t* request)
{
    apt_bool_t status = FALSE;
    ms_synth_channel_t* ms_channel = static_cast<ms_synth_channel_t*>(channel->method_obj);
    ms_synth_engine_t* ms_engine = ms_channel->ms_engine;
    apt_task_t* task = apt_consumer_task_base_get(ms_engine->task);
    apt_task_msg_t* msg = apt_task_msg_get(task);
    if(msg)
    {
        ms_synth_msg_t* ms_msg;
        msg->type = TASK_MSG_USER;
        ms_msg = reinterpret_cast<ms_synth_msg_t*>(msg->data);

        ms_msg->type = type;
        ms_msg->channel = channel;
        ms_msg->request = request;
        status = apt_task_msg_signal(task, msg);
    }
    return status;
}

static apt_bool_t ms_synth_msg_process(apt_task_t* task, apt_task_msg_t* msg)
{
    ms_synth_msg_t* ms_msg = (ms_synth_msg_t*)msg->data;
    switch(ms_msg->type)
    {
    case MS_SYNTH_MSG_OPEN_CHANNEL:
        /* open channel and send async response */
        mrcp_engine_channel_open_respond(ms_msg->channel, TRUE);
        break;
    case MS_SYNTH_MSG_CLOSE_CHANNEL:
        /* close channel, make sure there is no activity and send async response */
        mrcp_engine_channel_close_respond(ms_msg->channel);
        break;
    case MS_SYNTH_MSG_REQUEST_PROCESS:
        ms_synth_channel_request_dispatch(ms_msg->channel, ms_msg->request);
        break;
    default:
        break;
    }
    return TRUE;
}
