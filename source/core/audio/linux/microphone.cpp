#include "stdafx.h"

#include "microphone.h"
#include "ispxinterfaces.h"

#include "audio_sys.h"


//#define FRAME_COUNT (snd_pcm_uframes_t) 232

#define BITS_PER_SAMPLE        16
#define CHANNELS               1
#define SAMPLES_PER_SECOND     16000
#define BLOCK_ALIGN            (BITS_PER_SAMPLE>>3)*CHANNELS
#define AVG_BYTES_PER_SECOND   BLOCK_ALIGN*SAMPLES_PER_SECOND

namespace CARBON_IMPL_NAMESPACE() {

using namespace std;

class MicrophonePump : public ISpxAudioPump
{
public:
    
    using Sink_Type = shared_ptr<ISpxAudioProcessor>;

    MicrophonePump();
    ~MicrophonePump();

    virtual uint16_t GetFormat(WAVEFORMATEX* format, uint16_t size) override;

    virtual void SetFormat(const WAVEFORMATEX* format, uint16_t size) override {
        UNUSED(format);
        UNUSED(size);
        // TODO: 
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }

    virtual void StartPump(Sink_Type processor) override;

    virtual void StopPump() override;

    virtual State GetState() { return m_state.load(); }

    virtual void PausePump() override {
        // TODO: remove from the interface. Not needed.
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }
private:

    void UpdateState(AUDIO_STATE state);
    int Process(uint8_t* pBuffer, size_t size);

    static void OnInputStateChange(void* pContext, AUDIO_STATE state)
    {
        static_cast<MicrophonePump*>(pContext)->UpdateState(state);
    }

    static int OnInputWrite(void* pContext, uint8_t* pBuffer, size_t size)
    {
        return static_cast<MicrophonePump*>(pContext)->Process(pBuffer, size);
    }

    atomic<State> m_state;
    const WAVEFORMATEX m_format;
    AUDIO_SYS_HANDLE m_audioHandle;
    Sink_Type m_sink;
    mutex m_mutex;
    condition_variable m_cv;
};

shared_ptr<ISpxAudioPump> Microphone::Create() 
{
    return make_shared<MicrophonePump>();
}

MicrophonePump::MicrophonePump(): 
    m_state {State::NoInput},
    m_format { WAVE_FORMAT_PCM, CHANNELS, SAMPLES_PER_SECOND, AVG_BYTES_PER_SECOND, BLOCK_ALIGN, BITS_PER_SAMPLE, 0 }
{
    m_audioHandle = audio_create();
    SPX_THROW_HR_IF(SPXERR_MIC_NOT_AVAILABLE, m_audioHandle == NULL);
    (void)audio_setcallbacks(m_audioHandle, 
        NULL, NULL, 
        &MicrophonePump::OnInputStateChange, (void*)this, 
        &MicrophonePump::OnInputWrite, (void*)this,
        NULL, NULL);

    int val;
    auto result = audio_set_options(m_audioHandle, "channels", &(val = CHANNELS));
    SPX_THROW_HR_IF(SPXERR_MIC_ERROR, result != AUDIO_RESULT_OK);
    result = audio_set_options(m_audioHandle, "bits_per_sample", &(val = BITS_PER_SAMPLE));
    SPX_THROW_HR_IF(SPXERR_MIC_ERROR, result != AUDIO_RESULT_OK);
    result = audio_set_options(m_audioHandle, "sample_rate", &(val = SAMPLES_PER_SECOND));
    SPX_THROW_HR_IF(SPXERR_MIC_ERROR, result != AUDIO_RESULT_OK);
}

MicrophonePump::~MicrophonePump()
{
    StopPump();
    audio_destroy(m_audioHandle);
}

uint16_t MicrophonePump::GetFormat(WAVEFORMATEX* format, uint16_t size) 
{
    auto totalSize = uint16_t(sizeof(WAVEFORMATEX) + m_format.cbSize);
    if (format != nullptr) {
        memcpy(format, &m_format, min(totalSize, size));
    }
    return totalSize;
}

int MicrophonePump::Process(uint8_t* pBuffer, size_t size)
{
    int result = 0;

    SPX_ASSERT_WITH_MESSAGE(!!m_sink, "@ %s", __FUNCTION__);

    if (pBuffer != nullptr) 
    {
        auto sharedBuffer = SpxAllocSharedAudioBuffer(size);
        memcpy(sharedBuffer.get(), pBuffer, size);
        m_sink->ProcessAudio(sharedBuffer, size);
    }

    return result;
}

void MicrophonePump::StartPump(shared_ptr<ISpxAudioProcessor> processor)
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(processor == nullptr, SPXERR_INVALID_ARG);
    
    State initialState{ State::NoInput };
    if (!m_state.compare_exchange_strong(initialState, State::Idle))
    {
        SPX_DBG_TRACE_VERBOSE("%s when we're already in %s state",
            __FUNCTION__, (initialState == State::Idle ? "State::Idle" : "State::Processing"));
        return; // already started.
    }

    SPX_ASSERT_WITH_MESSAGE(m_audioHandle != nullptr, "@ %s", __FUNCTION__);
    SPX_ASSERT_WITH_MESSAGE(m_sink == nullptr, "@ %s", __FUNCTION__);

    m_sink = processor;

    auto result = audio_input_start(m_audioHandle);
    if (result != AUDIO_RESULT_OK) {
        m_sink.reset();
        m_state.store(initialState);
        SPX_THROW_ON_FAIL(SPXERR_MIC_ERROR);
    }

    unique_lock<mutex> lock(m_mutex);
    m_cv.wait(lock, [&] { return GetState() != State::Idle; });
}

void MicrophonePump::StopPump()
{
    SPX_DBG_TRACE_FUNCTION();
    State initialState{ State::Processing };
    if (!m_state.compare_exchange_strong(initialState, State::Idle))
    {
        SPX_DBG_TRACE_VERBOSE("%s when we're already in %s state",
            __FUNCTION__, (initialState == State::NoInput ? "State::NoInput" : "State::Idle"));
        return; // not running (could be in the no-input initialization phase).
    }

    SPX_ASSERT_WITH_MESSAGE(m_audioHandle != nullptr, "@ %s", __FUNCTION__);
    SPX_ASSERT_WITH_MESSAGE(m_sink != nullptr, "@ %s", __FUNCTION__);

    auto result = audio_input_stop(m_audioHandle);

    if (result != AUDIO_RESULT_OK) {
        m_state.store(initialState);
        SPX_THROW_ON_FAIL(SPXERR_MIC_ERROR);
    }

    unique_lock<mutex> lock(m_mutex);
    m_cv.wait(lock, [&] { return GetState() != State::Idle; });
}

void MicrophonePump::UpdateState(AUDIO_STATE state)
{
    if (state == AUDIO_STATE_RUNNING)
    {
        auto prevState = m_state.exchange(State::Processing);
        SPX_ASSERT_WITH_MESSAGE(prevState == State::Idle, "@ %s", __FUNCTION__);
        unique_lock<mutex> lock(m_mutex);
        m_sink->SetFormat(const_cast<WAVEFORMATEX*>(&m_format));
        m_cv.notify_one();
    }
    else if (state == AUDIO_STATE_STOPPED)
    {
        auto prevState = m_state.exchange(State::NoInput);
        SPX_ASSERT_WITH_MESSAGE(prevState == State::Idle, "@ %s", __FUNCTION__);
        unique_lock<mutex> lock(m_mutex);
        // Let the sink know we're done for now... 
        // TODO: this should be a dedicated method.
        m_sink->SetFormat(nullptr);
        m_sink.reset();
        m_cv.notify_one();
    }
    else
    {
        SPX_DBG_TRACE_VERBOSE("%s: unexpected audio state: %d.", __FUNCTION__, int(state));
        SPX_THROW_ON_FAIL(SPXERR_INVALID_STATE);
    }
}

} // CARBON_IMPL_NAMESPACE