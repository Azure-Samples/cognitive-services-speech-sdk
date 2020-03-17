//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// opus_decoder.cpp: Implementation definitions for OpusDecoder C++ class
//

#include "stdafx.h"
#include "opus_decoder.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

OpusDecoder::OpusDecoder(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback, uint16_t bitsPerSample, uint16_t numChannels, uint32_t sampleRate) :
    BaseGstreamer(readCallback)
{
    m_oggDemux = gst_element_factory_make("oggdemux", "oggdemux");
    ThrowAfterCleanLocal(m_oggDemux == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer oggdemux cannot be created");

    m_opusParser = gst_element_factory_make("opusparse", "opusparse");
    ThrowAfterCleanLocal(m_opusParser == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer opusparse cannot be created");

    m_opusDecoder = gst_element_factory_make("opusdec", "opusdec");
    ThrowAfterCleanLocal(m_opusDecoder == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer opusdec cannot be created");

    m_audioConvert = gst_element_factory_make("audioconvert", "audioconvert");
    ThrowAfterCleanLocal(m_audioConvert == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer audioconvert cannot be created");

    m_audioResampler = gst_element_factory_make("audioresample", "audioresample");
    ThrowAfterCleanLocal(m_audioResampler == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer audioresample cannot be created");

    gst_bin_add_many(GST_BIN(GetBaseElement(ElementType::Pipeline)),
        GetBaseElement(ElementType::Source),
        m_oggDemux,
        m_opusParser,
        m_opusDecoder,
        m_audioConvert,
        m_audioResampler,
        GetBaseElement(ElementType::Sink), NULL);

    ThrowAfterCleanLocal(!gst_element_link_many(GetBaseElement(ElementType::Source),
        m_oggDemux, NULL), SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **gst_element_link_many**. Gstreamer linking 'appsrc ! oggdemux' failed");

    ThrowAfterCleanLocal(!gst_element_link_many(m_opusParser,
        m_opusDecoder,
        m_audioConvert,
        m_audioResampler, NULL), SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **gst_element_link_many**. Gstreamer linking 'opusparse ! opusdec ! audioconvert ! audioresample' failed");

    UNUSED(bitsPerSample);
    // The following setting is for signed 16 bit little endian
    std::string numBitsPerSampleString = "S16LE";

    GstCaps *caps = gst_caps_new_simple("audio/x-raw",
        "format", G_TYPE_STRING, numBitsPerSampleString.c_str(),
        "rate", G_TYPE_INT, sampleRate,
        "channels", G_TYPE_INT, numChannels,
        NULL);

    ThrowAfterCleanLocal(caps == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_caps_new_simple**. Gstreamer caps cannot be created");

    ThrowAfterCleanLocal(!gst_element_link_filtered(m_audioResampler, GetBaseElement(ElementType::Sink), caps),
        SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_link_filtered**. Gstreamer linking 'audioresample ! appsink' failed");

    gst_caps_unref(caps);

    ThrowAfterCleanLocal(g_signal_connect(m_oggDemux, "pad-added", G_CALLBACK(PadAddedHandler), this) <= 0,
        SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **g_signal_connect**. Gstreamer oggdemux pad-added callback registration failed");
}

void OpusDecoder::PadAddedHandler(GstElement *src, GstPad *new_pad, OpusDecoder *data)
{
    GstPadLinkReturn ret = GST_PAD_LINK_OK;
    GstCaps *new_pad_caps = nullptr;
    GstStructure *new_pad_struct = nullptr;
    const gchar *new_pad_type = nullptr;
    GstPad *sink_pad = gst_element_get_static_pad(data->m_opusParser, "sink");

    UNUSED(src);

    SPX_DBG_TRACE_VERBOSE("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));
    if (sink_pad != nullptr && !gst_pad_is_linked(sink_pad))
    {
        new_pad_caps = gst_pad_get_current_caps(new_pad);
        new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
        new_pad_type = gst_structure_get_name(new_pad_struct);
        if (g_str_has_prefix(new_pad_type, "audio/x-opus"))
        {
            ret = gst_pad_link(new_pad, sink_pad);
        }
    }
    gst_caps_unref(new_pad_caps);
    gst_object_unref(sink_pad);
    SPX_TRACE_ERROR_IF(GST_PAD_LINK_FAILED(ret), "Failed **gst_pad_link**. Gstreamer oggdemux and opusparse linking failed");
}

void OpusDecoder::ThrowAfterCleanLocal(bool cond, uint32_t errCode, const char* pszFormat)
{
    if (cond)
    {
        UnrefObject((gpointer *)&m_oggDemux);
        UnrefObject((gpointer *)&m_opusParser);
        UnrefObject((gpointer *)&m_audioConvert);
        UnrefObject((gpointer *)&m_opusDecoder);
        UnrefObject((gpointer *)&m_audioResampler);
        ThrowAfterClean(cond, errCode, pszFormat);
    }
}


OpusDecoder::~OpusDecoder()
{

}

} } } } // Microsoft::CognitiveServices::Speech::Impl
