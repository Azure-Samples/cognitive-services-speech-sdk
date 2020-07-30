//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// base_gstreamer.cpp: Implementation definitions for BaseGstreamer C++ class
//

#include "stdafx.h"
#include "base_gstreamer.h"
#include "gstreamer_modules.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

BaseGstreamer::BaseGstreamer(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback, BufferType buffer):
    m_readCallback{ readCallback },
    m_buffer{ buffer }
{
    spx_gst_init();

    ThrowAfterClean(readCallback == nullptr, SPXERR_UNINITIALIZED, "Read stream callback is not initialized");

    m_codecPipeline = gst_pipeline_new("pipeline");
    ThrowAfterClean(m_codecPipeline == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_pipeline_new**. Gstreamer pipeline cannot be created.");

    m_bufferSource = gst_element_factory_make("appsrc", "audio_source");
    ThrowAfterClean(m_bufferSource == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer appsrc cannot be created");

    m_bufferSink = gst_element_factory_make("appsink", "app_sink");
    ThrowAfterClean(m_bufferSink == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer appsink cannot be created");

    m_this = this;

    ThrowAfterClean(g_signal_connect(m_bufferSource, "need-data", G_CALLBACK(StartFeed), this) <= 0,
        SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **g_signal_connect**. Gstreamer appsrc need-data callback registration failed");
    g_object_set(m_bufferSink, "emit-signals", TRUE, NULL);
    ThrowAfterClean(g_signal_connect(m_bufferSink, "new-sample", G_CALLBACK(NewSamples), this) <= 0,
        SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **g_signal_connect**. Gstreamer appsink new-sample callback registration failed");
}

BaseGstreamer::~BaseGstreamer()
{

}

void BaseGstreamer::UnrefObject(gpointer *elem)
{
    if (elem != nullptr && *elem != nullptr)
    {
        if (gst_element_get_parent(*elem) == nullptr)
        {
            gst_object_unref(*elem);
            *elem = nullptr;
        }
    }
}

void BaseGstreamer::ThrowAfterClean(bool cond, uint32_t errCode, const char* pszFormat)
{
    if (cond)
    {
        SPX_TRACE_ERROR_IF(true, "%s", pszFormat);
        Stop();
        UnrefObject((gpointer *)&m_codecPipeline);
        UnrefObject((gpointer *)&m_bufferSource);
        UnrefObject((gpointer *)&m_bufferSink);
        UnrefObject((gpointer *)&m_bus);
        SPX_IFTRUE_THROW_HR(true, errCode);
    }
}

GstElement *BaseGstreamer::GetBaseElement(ElementType id)
{
    GstElement *element = nullptr;
    switch (id)
    {
    case ElementType::Pipeline:
        element = m_codecPipeline;
        break;
    case ElementType::Source:
        element = m_bufferSource;
        break;
    case ElementType::Sink:
        element = m_bufferSink;
        break;
    default:
        break;
    }
    return element;
}

void BaseGstreamer::Stop()
{
    UnrefObject((gpointer *)&m_bus);
    if (m_codecPipeline != nullptr)
    {
        gst_element_set_state(m_codecPipeline, GST_STATE_NULL);
        UnrefObject((gpointer *)&m_codecPipeline);
    }
}

bool BaseGstreamer::GetStatus()
{
    std::unique_lock<std::mutex> lock(m_mtx);

    GstMessage *msg = nullptr;
    GError *err = nullptr;
    gchar *debug_info = nullptr;
    bool terminate = false;

    if (!m_endOfStream)
    {
        try
        {
            msg = gst_bus_timed_pop_filtered(m_bus, 0, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

            if (msg != nullptr)
            {
                switch (GST_MESSAGE_TYPE(msg))
                {
                case GST_MESSAGE_ERROR:
                    m_bErrorInsideGstreamer = true;
                    gst_message_parse_error(msg, &err, &debug_info);
                    if (msg->src != nullptr)
                    {
                        m_gstErrorString = "Source: " + std::string(GST_OBJECT_NAME(msg->src)) + "\r\n";
                    }

                    if (err != nullptr && err->message != nullptr)
                    {
                        m_gstErrorString += "Message: " + std::string(err->message) + "\r\n";
                        g_clear_error(&err);
                    }

                    if (debug_info != nullptr)
                    {
                        m_gstErrorString += "DebugInfo: " + std::string(debug_info) + "\r\n";
                        g_free(debug_info);
                    }
                    break;

                case GST_MESSAGE_EOS:
                    Stop();
                    m_endOfStream = true;
                    break;

                default:
                    m_gstErrorString = "Unexpected message received from Gstreamer";
                    m_bErrorInsideGstreamer = true;
                    break;
                }
                terminate = true;
            }
        }
        catch (const std::exception& e)
        {
            m_bErrorInsideGstreamer = true;
            m_gstErrorString = e.what();
        }
        catch (...)
        {
            m_gstErrorString = "Error: unexpected exception";
            m_bErrorInsideGstreamer = true;
        }

        if (msg != nullptr)
        {
            gst_message_unref(msg);
        }

        if (m_bErrorInsideGstreamer)
        {
            SPX_TRACE_ERROR_IF(true, "%s", m_gstErrorString.c_str());
            ThrowRuntimeError(m_gstErrorString);
        }
    }
    else
    {
        terminate = true;
    }


    return terminate;
}

void BaseGstreamer::StartReader()
{
    GstStateChangeReturn pipelineState = gst_element_set_state(GST_ELEMENT(m_codecPipeline), GST_STATE_PLAYING);

    ThrowAfterClean(pipelineState == GST_STATE_CHANGE_FAILURE,
        SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **gst_element_set_state**. Gstreamer pipeline GST_STATE_PLAYING failed");

    m_bus = gst_element_get_bus(m_codecPipeline);
    ThrowAfterClean(m_bus == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **gst_element_get_bus**. Gstreamer gst_element_get_bus failed");
}

GstFlowReturn BaseGstreamer::NewSamples(GstElement *sink, BaseGstreamer *data)
{
    GstFlowReturn ret = GST_FLOW_ERROR;
    if (data != nullptr && data->m_this != nullptr)
    {
        GstSample *sample = nullptr;;
        BaseGstreamer *obj = (BaseGstreamer *)data->m_this;

        try
        {
            GstMapInfo map;
            if (data != nullptr)
            {
                g_signal_emit_by_name(sink, "pull-sample", &sample);
                if (sample != nullptr)
                {
                    GstBuffer *buffer = gst_sample_get_buffer(sample);
                    if (buffer != nullptr)
                    {
                        if (gst_buffer_map(buffer, &map, GST_MAP_READ))
                        {
                            obj->m_buffer->Write(map.data, static_cast<uint32_t>(map.size));
                            ret = GST_FLOW_OK;
                            gst_buffer_unmap(buffer, &map);
                        }
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            std::unique_lock<std::mutex> lock(obj->m_mtx);
            obj->m_gstErrorString = e.what();
            obj->m_bErrorInsideGstreamer = true;
        }
        catch (...)
        {
            std::unique_lock<std::mutex> lock(obj->m_mtx);
            obj->m_gstErrorString = "Error: unexpected exception";
            obj->m_bErrorInsideGstreamer = true;
        }

        if (sample != nullptr)
        {
            gst_sample_unref(sample);
        }
    }

    return ret;
}

void BaseGstreamer::PushData(BaseGstreamer *data)
{
    if (data != nullptr && data->m_this != nullptr)
    {
        GstBuffer *buffer = nullptr;
        GstFlowReturn ret;
        GstMapInfo map;
        gint16 *raw = nullptr;
        BaseGstreamer *obj = (BaseGstreamer *)data->m_this;
        try
        {
            buffer = gst_buffer_new_and_alloc((gsize)CHUNK_SIZE);

            if (buffer != nullptr)
            {
                if (gst_buffer_map(buffer, &map, GST_MAP_WRITE))
                {
                    raw = (gint16 *)map.data;
                    if (raw != nullptr)
                    {
                        auto bytesActuallyRead = obj->m_readCallback((uint8_t*)raw, CHUNK_SIZE);
                        map.size = bytesActuallyRead;

                        if (bytesActuallyRead > 0)
                        {
                            g_signal_emit_by_name(data->m_bufferSource, "push-buffer", buffer, &ret);
                        }
                        else
                        {
                            g_signal_emit_by_name(data->m_bufferSource, "end-of-stream", &ret);
                        }

                        if (ret != GST_FLOW_OK)
                        {
                            // this is not a catastrophic error
                            SPX_TRACE_ERROR_IF(true, "Error pushing buffer to gstreamer");
                        }
                    }
                    gst_buffer_unmap(buffer, &map);
                }
            }
        }
        catch (const std::exception& e)
        {
            std::unique_lock<std::mutex> lock(obj->m_mtx);
            obj->m_gstErrorString = e.what();
            obj->m_bErrorInsideGstreamer = true;
        }
        catch (...)
        {
            std::unique_lock<std::mutex> lock(obj->m_mtx);
            obj->m_gstErrorString = "Error: unexpected exception";
            obj->m_bErrorInsideGstreamer = true;
        }

        gst_buffer_unref(buffer);
    }
}

void BaseGstreamer::StartFeed(GstElement *source, guint size, BaseGstreamer *data)
{
    (void)source;
    (void)size;
    while (1)
    {
        auto& buffer = data->m_buffer;
        auto readPos = buffer->GetReadPos();
        auto writePos = buffer->GetWritePos();
        auto avail = buffer->GetSize() - (writePos - readPos);
        if (avail > (2 * CHUNK_SIZE))
        {
            PushData(data);
            break;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }
}

uint32_t BaseGstreamer::Read(uint8_t* buffer, uint32_t bytesToRead)
{
    size_t bytesRead{};
    m_buffer->Read(buffer, bytesToRead, &bytesRead);
    return static_cast<uint32_t>(bytesRead);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
