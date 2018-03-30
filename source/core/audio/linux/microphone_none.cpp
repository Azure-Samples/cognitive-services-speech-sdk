#include "stdafx.h"

#include "microphone.h"
#include "ispxinterfaces.h"


// FIXME TODO fmegen ANDROID! ROOBO ssl lib version issue
// opensll version number mismatch
extern "C" int FIPS_mode_set(int r)
{
    (void)r;
    return 0;
}


namespace CARBON_IMPL_NAMESPACE() {

using namespace std;

class MicrophonePump : public ISpxAudioPump
{
public:
    
    using Sink_Type = shared_ptr<ISpxAudioProcessor>;

    MicrophonePump() {}
    ~MicrophonePump() {}

    virtual uint16_t GetFormat(WAVEFORMATEX* format, uint16_t size) override
    {
        UNUSED(format);
        UNUSED(size);
        return sizeof(WAVEFORMATEX);
    }

    virtual void SetFormat(const WAVEFORMATEX* format, uint16_t size) override {
        UNUSED(format);
        UNUSED(size);
        // TODO: 
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }

    virtual void StartPump(Sink_Type processor) override
    {
        UNUSED(processor);
    }

    virtual void StopPump() override
    {
    }

    virtual State GetState() { return m_state.load(); }

    virtual void PausePump() override {
        // TODO: remove from the interface. Not needed.
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }
private:
    atomic<State> m_state;
};


shared_ptr<ISpxAudioPump> Microphone::Create() 
{
    shared_ptr<ISpxAudioPump> empty = std::make_shared<MicrophonePump>();
    return empty;
}


} // CARBON_IMPL_NAMESPACE