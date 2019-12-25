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

    Timer() : m_stopped(true), m_tryToStop(false), m_nextRefreshTime(0)
    {
    }

    ~Timer()
    {
        Expire();
    }

    void Start(uint32_t successRefreshIntervalInMs, uint32_t failureRefreshIntervalInMs, std::function<bool()> task)
    {
        SPX_TRACE_ERROR_IF(!m_stopped, "timer is currently running, need expire it first.");
        SPX_IFTRUE_THROW_HR(!m_stopped, SPXERR_INVALID_STATE);

        m_thread = std::thread([this, successRefreshIntervalInMs, failureRefreshIntervalInMs, task]()
        {
            while (!m_tryToStop)
            {
                {
                    auto refreshDelta = task() ? successRefreshIntervalInMs : failureRefreshIntervalInMs;
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_nextRefreshTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count() + refreshDelta;
                    m_cv.notify_all();
                }

                while (!m_tryToStop)
                {
                    const auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                    if (currentTime >= m_nextRefreshTime)
                    {
                        break;
                    }

                    auto checkIntervalInMs = std::min(static_cast<int64_t>(m_checkIntervalInMs), static_cast<int64_t>(m_nextRefreshTime - currentTime));
                    std::this_thread::sleep_for(std::chrono::milliseconds(checkIntervalInMs));
                }
            }

            m_stopped = true;
        });

        m_stopped = false;
    }

    /**
     * \brief wait until the result is valid
     * \param thresholdTimeInMS threshold time, i.e. if current time - m_nextRefreshTime < threshold, the result is valid
     * \param timeoutInMs waiting timeout, in milliseconds
     * \return bool, if result is valid after waiting
     */
    bool WaitUntilValid(uint32_t thresholdTimeInMS, uint32_t timeoutInMs)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
#ifdef _DEBUG
        int remainingTime = timeoutInMs;
        while (!m_cv.wait_for(lock, std::chrono::milliseconds(100), [&] { return m_stopped || IsValid(thresholdTimeInMS); }))
        {
            SPX_DBG_TRACE_VERBOSE("%s: waiting ...", __FUNCTION__);
            remainingTime -= 100;
            if (remainingTime <= 0)
            {
                break;
            }
        }
        return IsValid(thresholdTimeInMS);
#else
        m_cv.wait_for(lock, std::chrono::milliseconds(timeoutInMs), [&] { return m_stopped || IsValid(thresholdTimeInMS); });
        return IsValid(thresholdTimeInMS);
#endif
    }

    void ForceRenew()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_nextRefreshTime = 0;
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

    bool IsValid(uint32_t thresholdTimeInMS) const
    {
        const auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        return currentTime - m_nextRefreshTime < thresholdTimeInMS;
    }

private:

    const uint32_t m_checkIntervalInMs = 20; // Check the status every 20ms in case the expire() method need to wait too long

    std::thread m_thread;
    std::atomic<bool> m_stopped;
    std::atomic<bool> m_tryToStop;
    std::atomic<int64_t> m_nextRefreshTime;  // milliseconds since epoch.

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

    std::string GetAccessToken(uint32_t timeoutInMs=5000);


private:

    bool RenewAccessToken();
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
