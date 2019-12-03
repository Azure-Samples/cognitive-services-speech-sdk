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

    ~Timer() = default;

    void Start(uint32_t intervalInMs, std::function<void()> task)
    {
        if (!m_stopped)
        {
            return; // timer is currently running, need expire it first
        }

        m_thread = std::thread([this, intervalInMs, task]()
        {
            while (!m_tryToStop)
            {
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    task();
                    m_lastRefreshTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                    m_cv.notify_all();
                }

                while (!m_tryToStop)
                {
                    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                    auto timeSinceLastRefresh = currentTime - m_lastRefreshTime;
                    if (timeSinceLastRefresh >= intervalInMs)
                    {
                        break;
                    }

                    auto checkIntervalInMs = std::min(static_cast<int64_t>(m_checkIntervalInMs), static_cast<int64_t>(intervalInMs - timeSinceLastRefresh));
                    std::this_thread::sleep_for(std::chrono::milliseconds(checkIntervalInMs));
                }
            }

            m_stopped = true;
        });

        m_stopped = false;
    }

    void WaitUntilValid(uint32_t expirationTimeInMS)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
#ifdef _DEBUG
        while (!m_cv.wait_for(lock, std::chrono::milliseconds(100), [&] { return IsValid(expirationTimeInMS); }))
        {
            SPX_DBG_TRACE_VERBOSE("%s: waiting ...", __FUNCTION__);
        }
#else
        m_cv.wait(lock, [&] { return IsValid(expirationTimeInMS); });
#endif
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

    bool IsValid(uint32_t expirationTimeInMS) const
    {
        auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        auto timeSinceLastRefresh = currentTime - m_lastRefreshTime;
        return timeSinceLastRefresh < expirationTimeInMS;
    }

private:

    const uint32_t m_checkIntervalInMs = 20; // Check the status every 20ms in case the expire() method need to wait too long

    std::thread m_thread;
    std::atomic<bool> m_stopped;
    std::atomic<bool> m_tryToStop;
    std::atomic<int64_t> m_lastRefreshTime;  // milliseconds since epoch.

    std::mutex m_mutex;
    std::condition_variable m_cv;
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
    Timer m_accessTokenRenewer;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
