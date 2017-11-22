//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_pump.h: Implementation declarations for CSpxAudioPump C++ class
//

#pragma once
#include <spxcore_common.h>
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxAudioPump : public ISpxAudioPump
{
public:

    CSpxAudioPump();
    ~CSpxAudioPump();

    void SetAudioReader(std::shared_ptr<ISpxAudioReader>& reader);

    uint32_t GetFormat(WAVEFORMATEX* pformat, uint32_t cbFormat) override;
    void SetFormat(const WAVEFORMATEX* pformat, uint32_t cbFormat) override;

    void StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor) override;
    void PausePump() override;
    void StopPump() override;

    State GetState() override;

    
private:

    CSpxAudioPump(const CSpxAudioPump&) = delete;
    CSpxAudioPump(const CSpxAudioPump&&) = delete;

    CSpxAudioPump& operator=(const CSpxAudioPump&) = delete;
    CSpxAudioPump& operator=(const CSpxAudioPump&&) = delete;

    void PumpThread(std::shared_ptr<CSpxAudioPump> keepAlive, std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor);
    
    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::shared_ptr<ISpxAudioReader> m_audioReader;
    enum class State m_stateRequested;
    enum class State m_state;

    std::thread m_thread;
};


}; // CARBON_IMPL_NAMESPACE()
