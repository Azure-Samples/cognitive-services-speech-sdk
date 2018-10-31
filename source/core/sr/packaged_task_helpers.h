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

    std::shared_future<std::shared_ptr<ISpxRecognitionResult>> RunAsync(std::packaged_task<std::shared_ptr<ISpxRecognitionResult>()>&& task)
    {
        std::unique_lock<std::mutex> lock(m_runAsyncMutex);

        auto taskFuture = std::shared_future<std::shared_ptr<ISpxRecognitionResult>>(task.get_future());
        std::thread taskThread(std::move(task));

        // taskThread.detach();
        m_threads.emplace_back(std::move(taskThread));

        m_runAsyncFutures2.emplace_back(taskFuture);

        return taskFuture;
    }

    std::shared_future<void> RunAsync(std::packaged_task<void()>&& task)
    {
        std::unique_lock<std::mutex> lock(m_runAsyncMutex);

        auto taskFuture = std::shared_future<void>(task.get_future());
        std::thread taskThread(std::move(task));

        // Note: no need to detach since we .join() later
        m_threads.emplace_back(std::move(taskThread));

        m_runAsyncFutures.emplace_back(taskFuture);

        return taskFuture;
    }

    void RunAsyncWaitAndClear()
    {
        std::unique_lock<std::mutex> lock(m_runAsyncMutex);
        while (!m_runAsyncFutures.empty())
        {
            std::shared_future<void>& future = m_runAsyncFutures.front();

            lock.unlock();
            SPX_DBG_TRACE_INFO("Task<void> checking future %p", &future);

            std::future_status status = future.wait_for(std::chrono::milliseconds(5000));
            UNUSED(status); // Release builds

            SPX_DBG_TRACE_ERROR_IF(status == std::future_status::timeout, "************************** Task<void> %p did not complete. This may corrupt your memory", &future);

            lock.lock();
            m_runAsyncFutures.pop_front();
        }

        while (!m_runAsyncFutures2.empty())
        {
            std::shared_future<std::shared_ptr<ISpxRecognitionResult>>& future = m_runAsyncFutures2.front();

            lock.unlock();
            SPX_DBG_TRACE_INFO("Task<ISpxRecognitionResult> checking future %p", &future);

            std::future_status status = future.wait_for(std::chrono::milliseconds(5000));
            UNUSED(status); // Release builds

            SPX_DBG_TRACE_ERROR_IF(status == std::future_status::timeout, "************************** Task<ISpxRecognitionResult> %p did not complete. This may corrupt your memory", &future);

            lock.lock();
            m_runAsyncFutures2.pop_front();
        }


        while (!m_threads.empty())
        {
            std::thread &thread = m_threads.front();

            lock.unlock();

            SPX_TRACE_ERROR("Task<> helper - checking thread %p", thread.native_handle());
            thread.join();

            lock.lock();
            m_threads.pop_front();
        }
    }

private:

    std::list<std::thread> m_threads;
    std::mutex m_runAsyncMutex;
    std::list<std::shared_future<std::shared_ptr<ISpxRecognitionResult>>> m_runAsyncFutures2;
    std::list<std::shared_future<void>> m_runAsyncFutures;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
