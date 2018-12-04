//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "test_utils.h"
#include "microphone.h"
#include <atomic>
#include <cstring>
#include <numeric>
#include <functional>

#include "exception.h"
#define __SPX_THROW_HR_IMPL(hr) Microsoft::CognitiveServices::Speech::Impl::ThrowWithCallstack(hr)

#include "ispxinterfaces.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace std;

class AudioTestSink final : public ISpxAudioProcessor
{
    using DataBuffer = pair<ISpxAudioProcessor::AudioData_Type, uint32_t>;

public:
    AudioTestSink(bool running = true)
        : m_running{ running },
        m_format{ 0, 0, 0, 0, 0, 0, 0 },
        m_setFormatCallCounter{ 0 },
        m_setFormatWithNullptrCallCounter{ 0 },
        m_processAudioCallCounter{ 0 }
    {}

    virtual void SetFormat(const SPXWAVEFORMATEX* format) override
    {
        unique_lock<mutex> lock(m_mutex);
        if (format != nullptr)
        {
            m_setFormatCallCounter++;
            std::memcpy(&m_format, format, sizeof(SPXWAVEFORMATEX));
        }
        else
        {
            m_setFormatWithNullptrCallCounter++;
        }
    }

    virtual void ProcessAudio(AudioData_Type data, uint32_t size) override
    {
        unique_lock<mutex> lock(m_mutex);
        if (!m_running)
        {
            return;
        }
        m_data.push_back(make_pair(data, size));
        if (m_processAudioCallCounter++ == 0)
        {
            m_cv.notify_one();
        }
    }

    void WaitAndStop(int seconds) {
       unique_lock<mutex> lock(m_mutex);
       if (m_running)
       {
           m_cv.wait_for(lock, chrono::seconds(seconds), [&] { return m_processAudioCallCounter > 0; });
           m_running = false;
       }
    }

    int GetNumCallsToSetFormat() const
    {
        unique_lock<mutex> lock(m_mutex);
        REQUIRE(!m_running);
        return m_setFormatCallCounter;
    }

    int GetNumCallsToSetFormatWithNullptr() const
    {
        unique_lock<mutex> lock(m_mutex);
        REQUIRE(!m_running);
        return m_setFormatWithNullptrCallCounter;
    }

    int GetNumCallsToProcessAudio() const
    {
        unique_lock<mutex> lock(m_mutex);
        REQUIRE(!m_running);
        return m_processAudioCallCounter;
    }

    const SPXWAVEFORMATEX& GetFormat() const
    {
        unique_lock<mutex> lock(m_mutex);
        REQUIRE(!m_running);
        return m_format;
    }

    size_t GetTotalAudioBytes() const
    {
        unique_lock<mutex> lock(m_mutex);
        REQUIRE(!m_running);
        return accumulate(m_data.begin(), m_data.end(),
            size_t(0), // start with first element
            [](size_t total, const DataBuffer& buffer )
        {
            return total + buffer.second;
        });
    }

    void Reset()
    {
        unique_lock<mutex> lock(m_mutex);
        REQUIRE(!m_running);
        m_running = true;
        m_format = { 0, 0, 0, 0, 0, 0, 0 };
        m_data.clear();
        m_setFormatCallCounter = 0;
        m_setFormatWithNullptrCallCounter = 0;
        m_processAudioCallCounter = 0;
    }

private:
    bool m_running { true };
    mutable mutex m_mutex;
    condition_variable m_cv;
    SPXWAVEFORMATEX m_format;
    vector<DataBuffer> m_data;
    int m_setFormatCallCounter;
    int m_setFormatWithNullptrCallCounter;
    int m_processAudioCallCounter;
};

class CyclicBarrier
{

public:

    CyclicBarrier(int count) :
        m_count{ count },
        m_cycle { 0 }
    {
        m_reset = [count = m_count, this] {
            m_cycle++;
            m_count = count;
        };
    }

    void await()
    {
        unique_lock<mutex> lock(m_mutex);
        if (--m_count == 0)
        {
            m_reset();
            m_cv.notify_all();
        }
        else
        {
            m_cv.wait(lock, [cycle = m_cycle, this] { return cycle != m_cycle; });
        }
    }

    bool await(chrono::seconds timeout)
    {
        unique_lock<mutex> lock(m_mutex);
        bool result;
        if (--m_count == 0)
        {
            m_reset();
            m_cv.notify_all();
            result = true;
        }
        else
        {
            result = m_cv.wait_for(lock, timeout, [cycle = m_cycle, this] { return cycle != m_cycle; });
        }
        return result;
    }

    mutex m_mutex;
    condition_variable m_cv;
    int m_count;
    int m_cycle;
    function<void(void)> m_reset;
};

void CheckFormatIsValid(const SPXWAVEFORMATEX& format)
{
    REQUIRE(format.wFormatTag != 0);
    REQUIRE(format.nChannels != 0);
    REQUIRE(format.nSamplesPerSec != 0);
    REQUIRE(format.wBitsPerSample != 0);
}

void RunMicAndCheckStateTransitions(const shared_ptr<ISpxAudioPump>& mic, const shared_ptr<AudioTestSink>& sink)
{
    REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
    mic->StartPump(sink);
    REQUIRE(mic->GetState() == ISpxAudioPump::State::Processing);
    sink->WaitAndStop(3);
    REQUIRE(mic->GetState() == ISpxAudioPump::State::Processing);
    mic->StopPump();
    REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
}

void CheckThatSinkReceivedAudioData(const shared_ptr<AudioTestSink>& sink)
{
    REQUIRE(sink->GetNumCallsToSetFormat() == 1);
    REQUIRE(sink->GetNumCallsToSetFormatWithNullptr() == 1);
    REQUIRE(sink->GetNumCallsToProcessAudio() != 0);
    CheckFormatIsValid(sink->GetFormat());
    REQUIRE(sink->GetTotalAudioBytes() != 0);
}


TEST_CASE("Mic is properly functioning", "[!hide][audio][mic]")
{

    const auto& mic = Microphone::Create();

    SECTION("freshly created mic is properly initialized")
    {
        REQUIRE(mic != nullptr);
        REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
    }

    SECTION("freshly created mic provides a valid audio format")
    {
        SPXWAVEFORMATEX format;
        REQUIRE(mic->GetFormat(&format, sizeof(format)) != 0);
        CheckFormatIsValid(format);
    }

    SECTION("start/stop calls are reflected by corresponding state changes")
    {
        RunMicAndCheckStateTransitions(mic, make_shared<AudioTestSink>(false));
    }

    SECTION("mic is pumping audio data into the processor")
    {
        const auto& sink = make_shared<AudioTestSink>();
        RunMicAndCheckStateTransitions(mic, sink);
        CheckThatSinkReceivedAudioData(sink);
    }

    SECTION("mic can be used (switched on and off) multiple times in a row")
    {
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

    SECTION("start/stop methods can be safely invoked multiples times in a row")
    {
        const auto& sink = make_shared<AudioTestSink>(false);
        REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
        for (int j = 0; j < 3; ++j)
        {
            for (int i = 0; i < 3; ++i)
            {
                mic->StartPump(sink);
                REQUIRE(mic->GetState() == ISpxAudioPump::State::Processing);
            }
            for (int i = 0; i < 3; ++i)
            {
                mic->StopPump();
                REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
            }
        }
    }

    SECTION("mic instance can be used from multiple threads (one shared sink)")
    {
        const auto& sink = make_shared<AudioTestSink>();
        int numThreads = 5;
        CyclicBarrier barrier(numThreads + 1);

        chrono::seconds timeout(10);

        auto run = [&]() {
            barrier.await(); // 1
            mic->StartPump(sink);
            barrier.await(); // 2
            mic->StopPump();
        };

        REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);

        for (int j = 0; j < 3; ++j)
        {
            vector<thread> v;

            for (int i = numThreads; i >0 ; --i)
            {
                v.emplace_back(run);
            }

            CHECK(barrier.await(timeout)); // 1
            sink->WaitAndStop(3);
            CHECK(barrier.await(timeout)); // 2

            for (auto& t : v)
            {
                t.join();
            }

            REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);

            CheckThatSinkReceivedAudioData(sink);
            sink->Reset();
        }
    }

    SECTION("different mic instances can be used from multiple threads (different sinks)")
    {
        unsigned int numThreads = 3;
        CyclicBarrier barrier(numThreads + 1);
        mutex m;
        vector<shared_ptr<AudioTestSink>> sinks;

        chrono::seconds timeout(10);

        auto run = [&]() {
            const auto& threadLocalMic = Microphone::Create();
            const auto& sink = make_shared<AudioTestSink>();

                {
                    unique_lock<mutex> lock(m);
                    sinks.push_back(sink);
                }

                barrier.await(); // 1
                threadLocalMic->StartPump(sink);
                barrier.await(); // 2
                threadLocalMic->StopPump();
        };

        for (int j = 0; j < 3; ++j) {
            vector<thread> v;

            for (unsigned int i = numThreads; i >0; --i)
            {
                v.emplace_back(run);
            }

            CHECK(barrier.await(timeout)); // 1

            {
                unique_lock<mutex> lock(m);
                CHECK(sinks.size() == numThreads);
                for (auto& sink : sinks)
                {
                    sink->WaitAndStop(3);
                }
            }

            CHECK(barrier.await(timeout)); // 2

            for (auto& t : v)
            {
                t.join();
            }

            for (auto& sink : sinks) {
                CheckThatSinkReceivedAudioData(sink);
                sink.reset();
            }
            sinks.clear();
        }
    }

    SECTION("stress-test mic with multiple threads (no lock-step syncronization)")
    {
        int numThreads = 10;
        mutex m;
        vector<shared_ptr<AudioTestSink>> sinks;
        int numCycles = 5;

        // simple linear congruential generator
        auto lcg = [&]() {
            unique_lock<mutex> lock(m);
            static int x = 12345678;
            x = 1664525l * x + 1013904223l;
            return x;
        };

        auto run = [&]() {
            const auto& threadLocalMic = Microphone::Create();
            const auto& sink = make_shared<AudioTestSink>();
            {
                {
                    unique_lock<mutex> lock(m);
                    sinks.push_back(sink);
                }
                for (int i = 0; i < numCycles; i++)
                {
                    this_thread::sleep_for(chrono::milliseconds(lcg() % 1000));
                    threadLocalMic->StartPump(sink);
                    // NOTE: this might be a little flaky, but overall 5+ seconds
                    // should be enough time to actually receive some audio from the mic.
                    this_thread::sleep_for(chrono::milliseconds(1000 + lcg() % 1000));
                    threadLocalMic->StopPump();
                }
            }
        };

        for (int j = 0; j < 3; ++j) {
            vector<thread> v;

            for (int i = numThreads; i >0; --i)
            {
                v.emplace_back(run);
            }

            for (auto& t : v)
            {
                t.join();
            }

            for (auto& sink : sinks)
            {
                sink->WaitAndStop(0);
                REQUIRE(sink->GetNumCallsToSetFormat() == numCycles);
                REQUIRE(sink->GetNumCallsToSetFormatWithNullptr() == numCycles);
                REQUIRE(sink->GetNumCallsToProcessAudio() != 0);
                CheckFormatIsValid(sink->GetFormat());
                REQUIRE(sink->GetTotalAudioBytes() != 0);
            }
            sinks.clear();
        }
    }

    SECTION("check a few corner cases")
    {
        REQUIRE(mic->GetFormat(nullptr, 0) != 0);
        REQUIRE(mic->GetFormat(nullptr, uint16_t(-1)) != 0);
        vector<char> x(sizeof(SPXWAVEFORMATEX)/sizeof(char));
        REQUIRE(mic->GetFormat(reinterpret_cast<SPXWAVEFORMATEX*>(x.data()), 1) != 0);
        CHECK_THROWS(mic->StartPump(nullptr));
        REQUIRE(mic->GetState() == ISpxAudioPump::State::NoInput);
    }
}
