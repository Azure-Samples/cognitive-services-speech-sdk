//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mpeg_decoder.cpp: Implementation definitions for MpegDecoder C++ class
//

#include "stdafx.h"
#include "mpeg_decoder.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

MpegDecoder::MpegDecoder(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback) :
    BaseGstreamer(readCallback)
{
    m_mpegAudioParse = gst_element_factory_make("mpegaudioparse", "mpegaudioparse");
    ThrowAfterCleanLocal(m_mpegAudioParse == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer mpegaudioparse cannot be created");

    m_mpeg123AudioDecode = gst_element_factory_make("mpg123audiodec", "mpg123audiodec");
    ThrowAfterCleanLocal(m_mpeg123AudioDecode == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer mpg123audiodec cannot be created");

    m_audioConvert = gst_element_factory_make("audioconvert", "audioconvert");
    ThrowAfterCleanLocal(m_audioConvert == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer audioconvert cannot be created");

    m_audioResample = gst_element_factory_make("audioresample", "audioresample");
    ThrowAfterCleanLocal(m_audioResample == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR, "Failed **gst_element_factory_make**. Gstreamer audioresample cannot be created");


    gst_bin_add_many(GST_BIN(GetBaseElement(ElementType::Pipeline)),
        GetBaseElement(ElementType::Source),
        m_mpegAudioParse,
        m_mpeg123AudioDecode,
        m_audioConvert,
        m_audioResample,
        GetBaseElement(ElementType::Sink), NULL);

    ThrowAfterCleanLocal(!gst_element_link_many(GetBaseElement(ElementType::Source),
        m_mpegAudioParse,
        m_mpeg123AudioDecode,
        m_audioConvert,
        m_audioResample, NULL), SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **gst_element_link_many**. Gstreamer linking 'appsrc ! mpegaudioparse ! mpg123audiodec ! audioconvert ! audioresample' failed");

    GstCaps *caps = gst_caps_new_simple("audio/x-raw",
        "format", G_TYPE_STRING, "S16LE",
        "rate", G_TYPE_INT, 16000,
        "channels", G_TYPE_INT, 1,
        NULL);

    ThrowAfterCleanLocal(caps == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **gst_caps_new_simple**. Gstreamer caps cannot be created");

    ThrowAfterCleanLocal(!gst_element_link_filtered(m_audioResample, GetBaseElement(ElementType::Sink), caps), SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **gst_element_link_filtered**. Gstreamer linking 'audioresample ! appsink' failed");

    gst_caps_unref(caps);
}

void MpegDecoder::ThrowAfterCleanLocal(bool cond, uint32_t errCode, const char* pszFormat)
{
    if (cond)
    {
        UnrefObject((gpointer *)&m_mpegAudioParse);
        UnrefObject((gpointer *)&m_mpeg123AudioDecode);
        UnrefObject((gpointer *)&m_audioConvert);
        UnrefObject((gpointer *)&m_audioResample);
        ThrowAfterClean(cond, errCode, pszFormat);
    }
}


MpegDecoder::~MpegDecoder()
{
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
