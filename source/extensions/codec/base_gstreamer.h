//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// base_gstreamer.h: Implementation declarations for BaseGstreamer C++ class
//

#pragma once
#include <gst/gst.h>
#include "ispxinterfaces.h"

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

enum class CodecsTypeInternal
{
    OGG_OPUS = 0x101,
    MP3 = 0x102,
    FLAC = 0x103,
    ALAW = 0x104,
    MULAW = 0x105,
    AMRNB = 0x106,
    AMRWB = 0x107
};

class BaseGstreamer
{
public:
    using BufferType = std::shared_ptr<ISpxReadWriteBuffer>;
    static constexpr uint32_t CHUNK_SIZE = 512;
    static constexpr uint32_t BUFFER_32KB = 32*1024;

    BaseGstreamer(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback, BufferType buffer);
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
    void *m_this;
    BufferType m_buffer{};
    std::string m_gstErrorString;
    bool m_bErrorInsideGstreamer = false;
    std::mutex m_mtx;
    bool m_endOfStream = false;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl