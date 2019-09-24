//
// Copyright (c) Microsoft.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// gstreamer_modules.cpp
//

#include "gstreamer_modules.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void spx_gst_init() {
#if defined(TARGET_OS_IPHONE)
    GST_PLUGIN_STATIC_REGISTER(coreelements);
    GST_PLUGIN_STATIC_REGISTER(app);
    GST_PLUGIN_STATIC_REGISTER(audioconvert);
    GST_PLUGIN_STATIC_REGISTER(mpg123);
    GST_PLUGIN_STATIC_REGISTER(audioresample);
    GST_PLUGIN_STATIC_REGISTER(audioparsers);
    GST_PLUGIN_STATIC_REGISTER(ogg);
    GST_PLUGIN_STATIC_REGISTER(opusparse);
    GST_PLUGIN_STATIC_REGISTER(opus);
    GST_PLUGIN_STATIC_REGISTER(wavparse);
    GST_PLUGIN_STATIC_REGISTER(alaw);
    GST_PLUGIN_STATIC_REGISTER(mulaw);
    GST_PLUGIN_STATIC_REGISTER(flac);
#endif
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
