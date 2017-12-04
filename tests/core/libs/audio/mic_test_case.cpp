#include "catch.hpp"
#include "microphone.h"
#include <atomic>
#include <numeric>
#include <functional>

using namespace CARBON_IMPL_NAMESPACE();
using namespace std;

class AudioTestSink final : public ISpxAudioProcessor
{
    using DataBuffer = pair<ISpxAudioProcessor::AudioData_Type, uint32_t>;

public:
    AudioTestSink(bool running = true)
        : m_running{ running }, 
        m_setFormatCallCounter{ 0 },
        m_processAudioCallCounter{ 0 },
        m_format {0, 0, 0, 0, 0, 0, 0 }
    {}

    virtual void SetFormat(WAVEFORMATEX* format) override {
        unique_lock<mutex> lock(m_mutex);
        if (!m_running) {
            return;
        }
        m_setFormatCallCounter++;
        memcpy(&m_format, format, sizeof(WAVEFORMATEX));
    }
    
    virtual void ProcessAudio(AudioData_Type data, uint32_t size) override {
        unique_lock<mutex> lock(m_mutex);
        if (!m_running) {
            return;
        }
        m_data.push_back(make_pair(data, size));
        if (m_processAudioCallCounter++ == 0) {
            m_cv.notify_one();
        }
    }

    void WaitAndStop(int seconds) {
       unique_lock<mutex> lock(m_mutex);
       if (m_running) {
           m_cv.wait_for(lock, chrono::seconds(seconds));
           m_running = false;
       }
    }

    int GetNumCallsToSetFormat() const {
        unique_lock<mutex> lock(m_mutex);
        REQUIRE(!m_running);
        return m_setFormatCallCounter;
    }

    int GetNumCallsToProcessAudio() const {
        unique_lock<mutex> lock(m_mutex);
        REQUIRE(!m_running);
        return m_setFormatCallCounter;
    }

    const WAVEFORMATEX GetFormat() const {
        unique_lock<mutex> lock(m_mutex);
        REQUIRE(!m_running);
        return m_format;
    }

    const size_t GetTotalAudioBytes() const {
        unique_lock<mutex> lock(m_mutex);
        REQUIRE(!m_running);
        return accumulate(m_data.begin(), m_data.end(),
            size_t(0), // start with first element
            [](size_t total, const DataBuffer& buffer ) {
            return total + buffer.second;
        });
    }

    void Reset() {
        REQUIRE(!m_running);
        m_running = true;
        m_format = { 0, 0, 0, 0, 0, 0, 0 };
        m_data.clear();
        m_setFormatCallCounter = 0;
        m_processAudioCallCounter = 0;
    }

private:
    bool m_running { true };
    mutable mutex m_mutex;
    condition_variable m_cv;
    WAVEFORMATEX m_format;
    vector<DataBuffer> m_data;
    int m_setFormatCallCounter;
    int m_processAudioCallCounter;
};

void CheckFormatIsValid(const WAVEFORMATEX& format) {
    REQUIRE(format.wFormatTag != 0);
    REQUIRE(format.nChannels != 0);
    REQUIRE(format.nSamplesPerSec != 0);
    REQUIRE(format.wBitsPerSample != 0);
}

void RunMicAndCheckStateTransitions(const shared_ptr<ISpxAudioPump>& mic, const shared_ptr<AudioTestSink>& sink) {
    REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
    mic->StartPump(sink);
    REQUIRE(mic->GetState() == ISpxAudioPump::State::Processing);
    sink->WaitAndStop(3);
    REQUIRE(mic->GetState() == ISpxAudioPump::State::Processing);
    mic->StopPump();
    REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
}

void CheckThatSinkReceivedAudioData(const shared_ptr<AudioTestSink>& sink) {
    REQUIRE(sink->GetNumCallsToSetFormat() == 1);
    REQUIRE(sink->GetNumCallsToProcessAudio() != 0);
    CheckFormatIsValid(sink->GetFormat());
    REQUIRE(sink->GetTotalAudioBytes() != 0);
}


TEST_CASE("Mic is properly functioning", "[audio][mic]") {

    const auto& mic = Microphone::Create();

    SECTION("freshly created mic is properly initialized") {
        REQUIRE(mic != nullptr);
        REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
    }

    SECTION("freshly created mic provides a valid audio format") {
        WAVEFORMATEX format;
        REQUIRE(mic->GetFormat(&format, sizeof(format)) != 0);
        CheckFormatIsValid(format);
    }

    SECTION("start/stop calls are reflected by corresponding state changes") {
        RunMicAndCheckStateTransitions(mic, make_shared<AudioTestSink>(false));
    }

    SECTION("mic is pumping audio data into the processor") {
        const auto& sink = make_shared<AudioTestSink>();
        RunMicAndCheckStateTransitions(mic, sink);
        CheckThatSinkReceivedAudioData(sink);
    }

    SECTION("mic can be used (switched on and off) multiple times in a row") {
        const auto& sink = make_shared<AudioTestSink>();
        RunMicAndCheckStateTransitions(mic, sink);
        CheckThatSinkReceivedAudioData(sink);
        sink->Reset();
        RunMicAndCheckStateTransitions(mic, sink);
        CheckThatSinkReceivedAudioData(sink);
        sink->Reset();
        RunMicAndCheckStateTransitions(mic, sink);
        CheckThatSinkReceivedAudioData(sink);
    }

    SECTION("start/stop methods can be safely invoked multiples times in a row") {
        const auto& sink = make_shared<AudioTestSink>(false);
        REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
        for (int j = 0; j < 3; ++j) {
            for (int i = 0; i < 3; ++i) {
                mic->StartPump(sink);
                REQUIRE(mic->GetState() == ISpxAudioPump::State::Processing);
            }
            for (int i = 0; i < 3; ++i) {
                mic->StopPump();
                REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
            }
        }
    }

    SECTION("mic can be used from multiple threads") {
        const auto& sink = make_shared<AudioTestSink>();
        mutex m;
        condition_variable all_runnning, barrier;
        int numThreads = 5;

        chrono::seconds timeout(30);

        auto run = [&]() {
            std::unique_lock<std::mutex> lock(m);
            if (--numThreads == 0) {
                all_runnning.notify_all();
            }
            barrier.wait_for(lock, timeout);
            mic->StartPump(sink);
            barrier.wait_for(lock, timeout);
            mic->StopPump();
        };

        auto stopPump = [&]() {
            mic->StopPump();
        };

        REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);

        for (int j = 0; j < 3; ++j) {
            vector<std::thread> v;
            numThreads = 5;

            for (int i = numThreads; i >0 ; --i) {
                v.emplace_back(run);
            }

            {
                std::unique_lock<std::mutex> lock(m);
                if (numThreads > 0) {
                    CHECK(all_runnning.wait_for(lock, timeout) == cv_status::no_timeout);
                }
                
                barrier.notify_all();
            }

            sink->WaitAndStop(3);
            REQUIRE(mic->GetState() == ISpxAudioPump::State::Processing);

            {
                std::unique_lock<std::mutex> lock(m);
                barrier.notify_all();
            }

            for (auto& t : v) {
                t.join();
            }
            
            REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);

            CheckThatSinkReceivedAudioData(sink);
            sink->Reset();
        }
    }

    SECTION("check a few corner cases") {
        REQUIRE(mic->GetFormat(nullptr, 0) != 0);
        REQUIRE(mic->GetFormat(nullptr, uint32_t(-1)) != 0);
        vector<char> x(sizeof(WAVEFORMATEX)/sizeof(char));
        REQUIRE(mic->GetFormat(reinterpret_cast<WAVEFORMATEX*>(x.data()), 1) != 0);
        CHECK_THROWS(mic->StartPump(nullptr)); 
        REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
    }
}