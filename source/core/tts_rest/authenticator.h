//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// authenticator.h: Implementation declarations for CSpxRestTtsAuthenticator C++ class
//

#pragma once
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "interface_helpers.h"
#include "spxcore_common.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class Timer
{
public:

    Timer() : m_stopped(true), m_tryToStop(false) {}

    ~Timer() {}

    void Start(int intervalInMs, std::function<void()> task)
    {
        if (!m_stopped)
        {
            return; // timer is currently running, need expire it first
        }

        std::thread([this, intervalInMs, task]() {
            int checkTimes = intervalInMs / m_checkIntervalInMs;
            int remainingMs = intervalInMs % m_checkIntervalInMs;

            while (!m_tryToStop)
            {
                for (int i = 0; i < checkTimes; ++i)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(m_checkIntervalInMs));
                    if (m_tryToStop)
                    {
                        break;
                    }
                }

                if (!m_tryToStop)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(remainingMs));
                }

                if (!m_tryToStop)
                {
                    task();
                }
            }

            // stop task ...
            {
                std::lock_guard<std::mutex> locker(m_mutex);
                m_stopped = true;
                m_stoppedCondition.notify_one();
            }
        }).detach();

        m_stopped = false;
    }

    void Expire()
    {
        if (m_stopped)
        {
            return;
        }

        if (m_tryToStop)
        {
            return;
        }

        m_tryToStop = true;

        {
            std::unique_lock<std::mutex> locker(m_mutex);
            m_stoppedCondition.wait(locker, [this] { return m_stopped ==  true; });
            if (m_stopped)
            {
                m_tryToStop = false;
            }
        }
    }


private:

    const int m_checkIntervalInMs = 200; // Check the status every 200ms in case the expire() method need to wait too long

    std::atomic<bool> m_stopped;
    std::atomic<bool> m_tryToStop;
    std::mutex m_mutex;
    std::condition_variable m_stoppedCondition;
};


class CSpxRestTtsAuthenticator : public ISpxObjectInit
{
public:

    CSpxRestTtsAuthenticator(const std::string& issueTokenUri, const std::string& subscriptionKey);
    virtual ~CSpxRestTtsAuthenticator();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit
    void Init() override;
    void Term() override;

    std::string GetAccessToken();


private:

    void RenewAccessToken();
    std::string HttpPost(const std::string& issueTokenUri, const std::string& subscriptionKey);

private:

    std::string m_issueTokenUri;
    std::string m_subscriptionKey;
    std::string m_accessToken;
    Timer m_accessTokenRenewer;
    std::mutex m_mutex;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
