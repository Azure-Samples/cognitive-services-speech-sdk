#include "stdafx.h"
#include <mfapi.h>
#include <mfidl.h>

#undef WAVE_FORMAT_PCM // already defined in mmeapi.h
#include "audio_sample_handler.h"

///////////////////////////////////////////////////////////////////////////////

#define BITS_PER_SECOND        16
#define CHANNELS               1
#define SAMPLES_PER_SECOND     16000
#define BLOCK_ALIGN            2
#define AVG_BYTES_PER_SECOND   (BITS_PER_SECOND>>3)*CHANNELS*SAMPLES_PER_SECOND

///////////////////////////////////////////////////////////////////////////////

namespace CARBON_IMPL_NAMESPACE() {


AudioSampleHandler::AudioSampleHandler()
    : AudioSampleHandler({ WAVE_FORMAT_PCM, CHANNELS, SAMPLES_PER_SECOND, AVG_BYTES_PER_SECOND, BLOCK_ALIGN, BITS_PER_SECOND })
{
}

AudioSampleHandler::AudioSampleHandler(const WAVEFORMATEX& format)
    : AudioSampleHandler(format, MFAudioFormat_PCM)
{
}

AudioSampleHandler::AudioSampleHandler(const WAVEFORMATEX& format, GUID subType)
    : SampleHandler(MFMediaType_Audio, subType), m_format{format}
{
}

}; // CARBON_IMPL_NAMESPACE()
