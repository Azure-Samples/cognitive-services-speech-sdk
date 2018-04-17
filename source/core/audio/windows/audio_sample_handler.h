#pragma once
#include "sample_handler.h"
#include "ispxinterfaces.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class AudioSampleHandler : public SampleHandler
{
public:
    AudioSampleHandler();
    AudioSampleHandler(const WAVEFORMATEX& format);
    AudioSampleHandler(const WAVEFORMATEX& format, GUID subType);
    virtual ~AudioSampleHandler() = default;

    virtual const WAVEFORMATEX& GetFormat() override { return m_format; };

private:
    WAVEFORMATEX m_format;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
