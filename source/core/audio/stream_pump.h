//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_pump.h: Implementation declarations for CSpxStreamPump C++ class
//

#pragma once
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


    class CSpxStreamPump : public ISpxAudioPump, public ISpxStreamPumpReaderInit
    {
    public:

        CSpxStreamPump();
        ~CSpxStreamPump();

        SPX_INTERFACE_MAP_BEGIN()
            SPX_INTERFACE_MAP_ENTRY(ISpxStreamPumpReaderInit)
            SPX_INTERFACE_MAP_ENTRY(ISpxAudioPump)
        SPX_INTERFACE_MAP_END()

        // --- ISpxStreamPumpReaderInit

        void SetAudioStream(AudioInputStream* reader) override;

        // --- ISpxStreamPump

        uint16_t GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat) override;
        void SetFormat(const WAVEFORMATEX* pformat, uint16_t cbFormat) override;

        void StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor) override;
        void PausePump() override;
        void StopPump() override;

        State GetState() override;


    private:

        CSpxStreamPump(const CSpxStreamPump&) = delete;
        CSpxStreamPump(const CSpxStreamPump&&) = delete;

        CSpxStreamPump& operator=(const CSpxStreamPump&) = delete;
        CSpxStreamPump& operator=(const CSpxStreamPump&&) = delete;

        void PumpThread(std::shared_ptr<CSpxStreamPump> keepAlive, std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor);

        std::mutex m_mutex;
        std::condition_variable m_cv;

        AudioInputStream* m_streamReader;

        enum State m_state;
        enum State m_stateRequested;

        std::thread m_thread;
    };


} } } } // Microsoft::CognitiveServices::Speech::Impl
