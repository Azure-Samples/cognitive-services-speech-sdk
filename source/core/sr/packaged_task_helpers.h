//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// packaged_task_helpers.h: Implementation definitions/declarations for CSpxPackagedTaskHelper C++ class
//

#pragma once
#include "spxcore_common.h"
#include <thread>
#include <list>
#include <mutex>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxPackagedTaskHelper
{
public:

    CSpxPackagedTaskHelper() = default;

    ~CSpxPackagedTaskHelper()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        RunAsyncWaitAndClear();
    }

    void RunAsync(std::packaged_task<void()>&& task)
    {
        std::unique_lock<std::mutex> lock(m_runAsyncMutex);

        auto taskFuture = task.get_future();
        std::thread taskThread(std::move(task));
        taskThread.detach();

        m_runAsyncFutures.emplace_back(std::move(taskFuture));
    }

    void RunAsyncWaitAndClear()
    {
        std::unique_lock<std::mutex> lock(m_runAsyncMutex);
        while (!m_runAsyncFutures.empty())
        {
            std::future<void>& future = m_runAsyncFutures.front();

            lock.unlock();
            future.wait_for(std::chrono::milliseconds(100));

            lock.lock();
            m_runAsyncFutures.pop_front();
        }
    }

private:

    std::mutex m_runAsyncMutex;
    std::list<std::future<void>> m_runAsyncFutures;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
