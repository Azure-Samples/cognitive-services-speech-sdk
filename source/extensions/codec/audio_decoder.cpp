//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_decoder.cpp: Implementation definitions for AudioDecoder C++ class
//

#include "stdafx.h"
#include "audio_decoder.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

AudioDecoder::AudioDecoder(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback, CodecsTypeInternal codecType) :
    BaseGstreamer(readCallback)
{

    std::vector<std::string> pluginNames;
    std::vector<GstElement*> ::iterator it;

    switch (codecType)
    {
    case CodecsTypeInternal::MP3:
        pluginNames = { "mpegaudioparse", "mpg123audiodec", "audioconvert" , "audioresample" };
        break;
    case CodecsTypeInternal::ALAW:
        pluginNames = { "wavparse", "alawdec", "audioresample" };
        break;
    case CodecsTypeInternal::MULAW:
        pluginNames = { "wavparse", "mulawdec", "audioresample" };
        break;
    case CodecsTypeInternal::FLAC:
        pluginNames = { "flacparse", "flacdec", "audioconvert", "audioresample" };
        break;
    case CodecsTypeInternal::AMRNB:
        //pluginNames = { "amrparse", "amrnbdec", "audioconvert", "audioresample" };
        SPX_IFTRUE_THROW_HR(true, SPXERR_CONTAINER_FORMAT_NOT_SUPPORTED_ERROR);
        break;
    case CodecsTypeInternal::AMRWB:
        //pluginNames = { "amrparse", "amrwbdec", "audioconvert", "audioresample" };
        SPX_IFTRUE_THROW_HR(true, SPXERR_CONTAINER_FORMAT_NOT_SUPPORTED_ERROR);
        break;
    default:
        SPX_IFTRUE_THROW_HR(true, SPXERR_CONTAINER_FORMAT_NOT_SUPPORTED_ERROR);
        break;
    }
    
    // m_gstElementQueue is a queue of GstElement* which holds the gstreamer pipeline from Source to Sink element
    // In the following code we parse the plugin names from pluginNames one by one and using gst_element_factory_make
    // we create GstElement and put it into m_gstElementQueue. We use the C++ iterator to parse from the begining of the
    // queue to the end of the queue.
    m_gstElementQueue.push_back(GetBaseElement(ElementType::Source));
    
    for (std::vector <std::string> ::iterator i = pluginNames.begin(); i < pluginNames.end(); i++)
    {
        GstElement *temp = gst_element_factory_make(i->c_str(), i->c_str());
        if (temp == nullptr)
        {
            std::string errorString = "Failed **gst_element_factory_make**. Plugin ";
            errorString += *i + " cannot be created";
            ThrowAfterCleanLocal(true, SPXERR_GSTREAMER_INTERNAL_ERROR, errorString.c_str());
        }
        m_gstElementQueue.push_back(temp);
    }
    m_gstElementQueue.push_back(GetBaseElement(ElementType::Sink));
    m_isSinkInQueue = true;

    // m_gstElementQueue is a queue of GstElement* which holds the gstreamer pipeline from source to sink element
    // using C++ iterator we parse each element from source to sink and put that inside gstreamer pipeline

    for( it = m_gstElementQueue.begin(); it != m_gstElementQueue.end(); ++it )
    {
        gst_bin_add_many(GST_BIN(GetBaseElement(ElementType::Pipeline)),
            *it, NULL);
    }

    // We link all the element except the resampler and the sink. The resampler has to resample at 16Khz, 16bit and 1 channel
    // so we parse the queue from the begining till the end except the last 2 element, thats why there is a -2 in the loop below
    unsigned int i = 0;
    for (it = m_gstElementQueue.begin(); i < m_gstElementQueue.size() - 2 ; i++)
    {
        GstElement *front = *it;
        ++it;
        gboolean linked = gst_element_link_many(front, *it, NULL);
        if (!linked)
        {
            std::string errorString = "Failed **gst_element_link_many**. Linking " + std::string(front->object.name) + " with " + std::string((*it)->object.name) + " failed";
            ThrowAfterCleanLocal(true, SPXERR_GSTREAMER_INTERNAL_ERROR, errorString.c_str());
        }
    }

    GstCaps *caps = gst_caps_new_simple("audio/x-raw",
        "format", G_TYPE_STRING, "S16LE",
        "rate", G_TYPE_INT, 16000,
        "channels", G_TYPE_INT, 1,
        NULL);

    ThrowAfterCleanLocal(caps == nullptr, SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **gst_caps_new_simple**. Gstreamer caps cannot be created");

    // Here we link the last two element to complete the pipeline
    GstElement *front = *it;
    ++it;
    // front is the resampler and "it" is sink. They both are linked with the caps contrat
    ThrowAfterCleanLocal(!gst_element_link_filtered(front, *it, caps), SPXERR_GSTREAMER_INTERNAL_ERROR,
        "Failed **gst_element_link_filtered**. Gstreamer linking 'audioresample ! appsink' failed");

    gst_caps_unref(caps);
}

void AudioDecoder::ThrowAfterCleanLocal(bool cond, uint32_t errCode, const char* pszFormat)
{
    if (cond)
    {
        std::vector <GstElement*> ::iterator it;

        if (m_isSinkInQueue)
        {
            m_gstElementQueue.pop_back();
        }

        m_gstElementQueue.erase(m_gstElementQueue.begin());

        for (it = m_gstElementQueue.begin(); it != m_gstElementQueue.end(); ++it)
        {
            UnrefObject((gpointer *)&(*it));
        }

        m_gstElementQueue.erase(m_gstElementQueue.begin(), m_gstElementQueue.end());

        ThrowAfterClean(cond, errCode, pszFormat);
    }
}

AudioDecoder::~AudioDecoder()
{
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
