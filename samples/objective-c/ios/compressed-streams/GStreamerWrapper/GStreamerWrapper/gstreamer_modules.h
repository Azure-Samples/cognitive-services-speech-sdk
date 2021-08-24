//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// gstreamer_modules.h
//

#pragma once

#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif

#include <gst/gst.h>

#if defined(TARGET_OS_IPHONE)
extern "C"
{
#define GST_G_IO_MODULE_DECLARE(name) \
    extern void G_PASTE(g_io_module_, G_PASTE(name, _load_static)) (void)

#define GST_G_IO_MODULE_LOAD(name) \
    G_PASTE(g_io_module_, G_PASTE(name, _load_static)) ()

    GST_PLUGIN_STATIC_DECLARE(coreelements);
    GST_PLUGIN_STATIC_DECLARE(app);
    GST_PLUGIN_STATIC_DECLARE(audioconvert);
    GST_PLUGIN_STATIC_DECLARE(mpg123);
    GST_PLUGIN_STATIC_DECLARE(audioresample);
    GST_PLUGIN_STATIC_DECLARE(audioparsers);
    GST_PLUGIN_STATIC_DECLARE(ogg);
    GST_PLUGIN_STATIC_DECLARE(opusparse);
    GST_PLUGIN_STATIC_DECLARE(opus);
    GST_PLUGIN_STATIC_DECLARE(wavparse);
    GST_PLUGIN_STATIC_DECLARE(alaw);
    GST_PLUGIN_STATIC_DECLARE(mulaw);
    GST_PLUGIN_STATIC_DECLARE(flac);
    GST_PLUGIN_STATIC_DECLARE(playback);

    //TODO: Need to enable the following on 1.16.0. Blocked on mac device

/*
    GST_PLUGIN_STATIC_DECLARE(adder);
    GST_PLUGIN_STATIC_DECLARE(audiomixer);
    GST_PLUGIN_STATIC_DECLARE(audiorate);
    GST_PLUGIN_STATIC_DECLARE(audiotestsrc);
    GST_PLUGIN_STATIC_DECLARE(compositor);
    GST_PLUGIN_STATIC_DECLARE(gio);
    GST_PLUGIN_STATIC_DECLARE(overlaycomposition);
    GST_PLUGIN_STATIC_DECLARE(pango);
    GST_PLUGIN_STATIC_DECLARE(rawparse);
    GST_PLUGIN_STATIC_DECLARE(typefindfunctions);
    GST_PLUGIN_STATIC_DECLARE(videoconvert);
    GST_PLUGIN_STATIC_DECLARE(videorate);
    GST_PLUGIN_STATIC_DECLARE(videoscale);
    GST_PLUGIN_STATIC_DECLARE(videotestsrc);
    GST_PLUGIN_STATIC_DECLARE(volume);
    GST_PLUGIN_STATIC_DECLARE(autodetect);
    GST_PLUGIN_STATIC_DECLARE(videofilter);
    GST_PLUGIN_STATIC_DECLARE(subparse);
    GST_PLUGIN_STATIC_DECLARE(theora);
    GST_PLUGIN_STATIC_DECLARE(vorbis);
    GST_PLUGIN_STATIC_DECLARE(ivorbisdec);
    GST_PLUGIN_STATIC_DECLARE(apetag);
    GST_PLUGIN_STATIC_DECLARE(auparse);
    GST_PLUGIN_STATIC_DECLARE(avi);
    GST_PLUGIN_STATIC_DECLARE(dv);
    GST_PLUGIN_STATIC_DECLARE(flv);
    GST_PLUGIN_STATIC_DECLARE(flxdec);
    GST_PLUGIN_STATIC_DECLARE(icydemux);
    GST_PLUGIN_STATIC_DECLARE(id3demux);
    GST_PLUGIN_STATIC_DECLARE(isomp4);
    GST_PLUGIN_STATIC_DECLARE(jpeg);
    GST_PLUGIN_STATIC_DECLARE(lame);
    GST_PLUGIN_STATIC_DECLARE(matroska);
    GST_PLUGIN_STATIC_DECLARE(multipart);
    GST_PLUGIN_STATIC_DECLARE(png);
    GST_PLUGIN_STATIC_DECLARE(speex);
    GST_PLUGIN_STATIC_DECLARE(taglib);
    GST_PLUGIN_STATIC_DECLARE(vpx);
    GST_PLUGIN_STATIC_DECLARE(wavenc);
    GST_PLUGIN_STATIC_DECLARE(wavpack);
    GST_PLUGIN_STATIC_DECLARE(y4menc);
    GST_PLUGIN_STATIC_DECLARE(adpcmdec);
    GST_PLUGIN_STATIC_DECLARE(adpcmenc);
    GST_PLUGIN_STATIC_DECLARE(bz2);
    GST_PLUGIN_STATIC_DECLARE(dashdemux);
    GST_PLUGIN_STATIC_DECLARE(dvbsuboverlay);
    GST_PLUGIN_STATIC_DECLARE(dvdspu);
    GST_PLUGIN_STATIC_DECLARE(hls);
    GST_PLUGIN_STATIC_DECLARE(id3tag);
    GST_PLUGIN_STATIC_DECLARE(kate);
    GST_PLUGIN_STATIC_DECLARE(midi);
    GST_PLUGIN_STATIC_DECLARE(mxf);
    GST_PLUGIN_STATIC_DECLARE(openh264);
    GST_PLUGIN_STATIC_DECLARE(pcapparse);
    GST_PLUGIN_STATIC_DECLARE(pnm);
    GST_PLUGIN_STATIC_DECLARE(rfbsrc);
    GST_PLUGIN_STATIC_DECLARE(siren);
    GST_PLUGIN_STATIC_DECLARE(smoothstreaming);
    GST_PLUGIN_STATIC_DECLARE(subenc);
    GST_PLUGIN_STATIC_DECLARE(videoparsersbad);
    GST_PLUGIN_STATIC_DECLARE(y4mdec);
    GST_PLUGIN_STATIC_DECLARE(jpegformat);
    GST_PLUGIN_STATIC_DECLARE(gdp);
    GST_PLUGIN_STATIC_DECLARE(rsvg);
    GST_PLUGIN_STATIC_DECLARE(openjpeg);
    GST_PLUGIN_STATIC_DECLARE(spandsp);
    GST_PLUGIN_STATIC_DECLARE(sbc);
    GST_PLUGIN_STATIC_DECLARE(assrender);
    GST_PLUGIN_STATIC_DECLARE(asfmux);
    GST_PLUGIN_STATIC_DECLARE(dtsdec);
    GST_PLUGIN_STATIC_DECLARE(mpegpsdemux);
    GST_PLUGIN_STATIC_DECLARE(mpegpsmux);
    GST_PLUGIN_STATIC_DECLARE(mpegtsdemux);
    GST_PLUGIN_STATIC_DECLARE(mpegtsmux);
    GST_PLUGIN_STATIC_DECLARE(voaacenc);
    GST_PLUGIN_STATIC_DECLARE(a52dec);
    GST_PLUGIN_STATIC_DECLARE(amrnb);
    GST_PLUGIN_STATIC_DECLARE(amrwbdec);
    GST_PLUGIN_STATIC_DECLARE(asf);
    GST_PLUGIN_STATIC_DECLARE(dvdsub);
    GST_PLUGIN_STATIC_DECLARE(dvdlpcmdec);
    GST_PLUGIN_STATIC_DECLARE(xingmux);
    GST_PLUGIN_STATIC_DECLARE(realmedia);
    GST_PLUGIN_STATIC_DECLARE(x264);
    GST_PLUGIN_STATIC_DECLARE(libav);
    */
}
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

__attribute__((visibility ("default"))) void spx_gst_init_base();
__attribute__((visibility ("default"))) void spx_gst_init_extra();

} } } } // Microsoft::CognitiveServices::Speech::Impl
