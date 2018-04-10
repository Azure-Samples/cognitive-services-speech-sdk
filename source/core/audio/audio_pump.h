//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_pump.h: Implementation declarations for CSpxAudioPump C++ class
//

#pragma once
#include "spxcore_common.h"
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxAudioPump : public ISpxAudioPump, public ISpxAudioPumpReaderInit
{
public:

    CSpxAudioPump();
    ~CSpxAudioPump();

    // --- ISpxAudioPumpReaderInit

    void SetAudioReader(std::shared_ptr<ISpxAudioReader>& reader) override;

    // --- ISpxAudioPump

    uint16_t GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat) override;
    void SetFormat(const WAVEFORMATEX* pformat, uint16_t cbFormat) override;

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
    void WaitForPumpStart(std::unique_lock<std::mutex>& lock);
    void WaitForPumpIdle(std::unique_lock<std::mutex>& lock);

    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::shared_ptr<ISpxAudioReader> m_audioReader;

    enum State m_state;
    enum State m_stateRequested;
    const int m_waitMsStartPumpRequestTimeout = 5000;
    const int m_waitMsStopPumpRequestTimeout = 5000;

    std::thread m_thread;
};


} // CARBON_IMPL_NAMESPACE
