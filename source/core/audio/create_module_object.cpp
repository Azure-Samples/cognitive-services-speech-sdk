//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.cpp: Implementation definitions for *CreateModuleObject* methods
//

#include "stdafx.h"

#include "audio_pump.h"
#include "audio_config.h"
#include "pull_audio_input_stream.h"
#include "push_audio_input_stream.h"
#include "push_audio_output_stream.h"
#include "pull_audio_output_stream.h"
#include "interactive_microphone.h"
#include "factory_helpers.h"
#include "wav_file_reader.h"
#include "wav_file_pump.h"
#include "wav_file_writer.h"
#include "default_speaker.h"
#include "null_audio_output.h"
#include "audio_data_stream.h"
#include "microphone_pump.h"
#include "single_to_many_stream_reader_adapter.h"
#include "audio_processor_write_to_audio_source_buffer.h"
#include "audio_source_buffer_data.h"
#include "audio_source_buffer_properties.h"
#include "blocking_read_write_ring_buffer.h"
#include "read_write_ring_buffer.h"
#include "audio_source_wrapper.h"
#include "microphone_audio_source_adapter.h"
#include "file_audio_source_adapter.h"
#include "stream_audio_source_adapter.h"
#include "internal_audio_codec_adapter.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


SPX_EXTERN_C void* AudioLib_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY(CSpxAudioPump, ISpxAudioPumpInit);
    SPX_FACTORY_MAP_ENTRY(CSpxAudioPump, ISpxAudioPump);
    SPX_FACTORY_MAP_ENTRY(CSpxMicrophonePump, ISpxAudioPump);
    SPX_FACTORY_MAP_ENTRY(CSpxAudioConfig, ISpxAudioConfig);
    SPX_FACTORY_MAP_ENTRY(CSpxPushAudioInputStream, ISpxAudioStreamInitFormat);
    SPX_FACTORY_MAP_ENTRY(CSpxPullAudioInputStream, ISpxAudioStreamInitFormat);
    SPX_FACTORY_MAP_ENTRY(CSpxPushAudioOutputStream, ISpxAudioStreamInitFormat);
    SPX_FACTORY_MAP_ENTRY(CSpxPullAudioOutputStream, ISpxAudioStreamInitFormat);
    SPX_FACTORY_MAP_ENTRY(CSpxPullAudioOutputStream, ISpxAudioOutput);
    SPX_FACTORY_MAP_ENTRY(CSpxInteractiveMicrophone, ISpxAudioPump);
    SPX_FACTORY_MAP_ENTRY(CSpxWavFileReader, ISpxAudioFile);
    SPX_FACTORY_MAP_ENTRY(CSpxWavFilePump, ISpxAudioFile);
    SPX_FACTORY_MAP_ENTRY(CSpxWavFileWriter, ISpxAudioFile);
    SPX_FACTORY_MAP_ENTRY(CSpxDefaultSpeaker, ISpxAudioOutput);
    SPX_FACTORY_MAP_ENTRY(CSpxNullAudioOutput, ISpxAudioOutput);
    SPX_FACTORY_MAP_ENTRY(CSpxAudioDataStream, ISpxAudioDataStream);
    SPX_FACTORY_MAP_ENTRY(CSpxSingleToManyStreamReaderAdapter, ISpxSingleToManyStreamReaderAdapter);
    SPX_FACTORY_MAP_ENTRY(CSpxReadWriteRingBuffer, ISpxReadWriteBufferInit);
    SPX_FACTORY_MAP_ENTRY(CSpxBlockingReadWriteRingBuffer, ISpxReadWriteBufferInit);
    SPX_FACTORY_MAP_ENTRY(CSpxAudioSourceBufferData, ISpxAudioSourceBufferData);
    SPX_FACTORY_MAP_ENTRY(CSpxAudioSourceBufferProperties, ISpxAudioSourceBufferProperties);
    SPX_FACTORY_MAP_ENTRY(CSpxAudioProcessorWriteToAudioSourceBuffer, ISpxAudioProcessor);
    SPX_FACTORY_MAP_ENTRY(CSpxAudioSourceWrapper, ISpxAudioSourceInit);
    SPX_FACTORY_MAP_ENTRY(CSpxMicrophoneAudioSourceAdapter, ISpxAudioSourceInit);
    SPX_FACTORY_MAP_ENTRY(CSpxFileAudioSourceAdapter, ISpxAudioSourceInit);
    SPX_FACTORY_MAP_ENTRY(CSpxStreamAudioSourceAdapter, ISpxAudioSourceInit);
    SPX_FACTORY_MAP_ENTRY(CSpxInternalAudioCodecAdapter, ISpxInternalAudioCodecAdapter);
    SPX_FACTORY_MAP_END();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
