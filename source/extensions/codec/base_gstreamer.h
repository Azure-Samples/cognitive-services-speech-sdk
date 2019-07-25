//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// base_gstreamer.h: Implementation declarations for BaseGstreamer C++ class
//

#pragma once
#include <gst/gst.h>
#include "ispxinterfaces.h"
#include "ring_buffer.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

enum class ElementType
{
    Pipeline = 0,
    Source = 1,
    Sink = 2
};

enum class PipelineCurrentState
{
    GST_EOS = 0,
    GST_ERROR = 1,
    GST_PLAYING = 2
};


class BaseGstreamer
{
public:
    BaseGstreamer(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback);
    virtual ~BaseGstreamer();

    void StartReader();
    bool GetStatus();
    void Stop();
    uint32_t Read(uint8_t* buffer, uint32_t bytesToRead);

protected:
    void ThrowAfterClean(bool cond, uint32_t errCode, const char* pszFormat);
    void UnrefObject(gpointer *elem);
    GstElement *GetBaseElement(ElementType id);

private:
    static GstFlowReturn NewSamples(GstElement *sink, BaseGstreamer *data);
    static void StartFeed(GstElement *source, guint size, BaseGstreamer *data);
    static void PushData(BaseGstreamer *data);

    GstElement *m_codecPipeline = nullptr;
    GstElement *m_bufferSource = nullptr;
    GstElement *m_bufferSink = nullptr;
    GstBus *m_bus = nullptr;

    ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type m_readCallback;
    static const uint32_t CHUNK_SIZE = 512;
    static const uint32_t BUFFER_32KB = 32*1024;
    void *m_this;
    std::shared_ptr<RingBuffer> m_ringBuffer = nullptr;
    std::string m_gstErrorString;
    bool m_bErrorInsideGstreamer = false;
    std::mutex m_mtx;
    bool m_endOfStream = false;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
