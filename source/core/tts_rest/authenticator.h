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

        m_thread = std::thread([this, intervalInMs, task]()
        {
            task();

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

            m_stopped = true;
        });

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

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }


private:

    const int m_checkIntervalInMs = 200; // Check the status every 200ms in case the expire() method need to wait too long

    std::thread m_thread;
    std::atomic<bool> m_stopped;
    std::atomic<bool> m_tryToStop;
};


class CSpxRestTtsAuthenticator : public ISpxObjectInit
{
public:

    CSpxRestTtsAuthenticator(const std::string& issueTokenUri, const std::string& subscriptionKey,
        const std::string& proxyHost, int proxyPort, const std::string& proxyUsername, const std::string& proxyPassword);
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
    std::string HttpPost(const std::string& issueTokenUri, const std::string& subscriptionKey,
        const std::string& proxyHost, int proxyPort, const std::string& proxyUsername, const std::string& proxyPassword);

private:

    std::string m_issueTokenUri;
    std::string m_subscriptionKey;
    std::string m_proxyHost;
    int m_proxyPort;
    std::string m_proxyUsername;
    std::string m_proxyPassword;
    std::string m_accessToken;
    std::atomic<bool> m_accessTokenInitialized{ false };
    Timer m_accessTokenRenewer;
    std::mutex m_mutex;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
