//
// Copyright (c) Microsoft. All rights reserved.
// See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "thread_service.h"

namespace Microsoft { namespace CognitiveServices { namespace Speech { namespace Impl {

    using namespace std;
    using namespace std::chrono;

    void CSpxThreadService::Init()
    {
        SPX_DBG_TRACE_ERROR_IF(!m_threads.empty(), "Init should be called only once at the beginning.");
        SPX_IFTRUE_THROW_HR(!m_threads.empty(), SPXERR_INVALID_STATE);
        m_threads =
        {
            {Affinity::Background, make_shared<Thread>()},
            {Affinity::User, make_shared<Thread>()}
        };

        for (auto& t : m_threads)
            t.second->Start();
    }

    void CSpxThreadService::Term()
    {
        if (m_threads.empty())
            return;

        // Currently, stopping the user thread as detached.
        m_threads[Affinity::User]->Stop(true);

        for (auto& t : m_threads)
            t.second->Stop();
    }

    void CSpxThreadService::CheckInitialized()
    {
        SPX_DBG_TRACE_ERROR_IF(m_threads.empty(), "Thread service should be initialized.");
        SPX_IFTRUE_THROW_HR(m_threads.empty(), SPXERR_INVALID_STATE);
    }

    CSpxThreadService::TaskId CSpxThreadService::ExecuteAsync(packaged_task<void()>&& task, Affinity affinity, promise<bool>&& executed)
    {
        CheckInitialized();

        auto taskId = m_nextTaskId++;
        auto innerTask = make_shared<Task>(taskId, move(task), move(executed));
        m_threads[affinity]->Queue(innerTask);
        return taskId;
    }

    CSpxThreadService::TaskId CSpxThreadService::ExecuteAsync(packaged_task<void()>&& task, milliseconds delay, Affinity affinity, promise<bool>&& executed)
    {
        CheckInitialized();

        auto taskId = m_nextTaskId++;
        auto innerTask = make_shared<DelayTask>(taskId, move(task), move(executed), delay);
        m_threads[affinity]->Queue(innerTask);
        return taskId;
    }

    void CSpxThreadService::ExecuteSync(std::packaged_task<void()>&& task, Affinity affinity)
    {
        for (const auto& t : m_threads)
        {
            if (t.second->Id() == this_thread::get_id())
            {
                SPX_DBG_TRACE_ERROR("Task cannot be executed synchronously on the thread"
                    " from the thread service in order to avoid potential deadlocks.");
                SPX_THROW_HR(SPXERR_ABORT);
            }
        }

        auto future = task.get_future();
        std::promise<bool> executed;
        auto executedFuture = executed.get_future();
        ExecuteAsync(std::move(task), affinity, std::move(executed));

        if (executedFuture.get())
        {
            future.get();
        }
    }

    bool CSpxThreadService::Cancel(CSpxThreadService::TaskId id)
    {
        CheckInitialized();

        for (auto& t : m_threads)
        {
            if (t.second->Cancel(id))
                return true;
        }
        return false;
    }

    void CSpxThreadService::CancelAllTasks()
    {
        CheckInitialized();

        for (auto& t : m_threads)
            t.second->CancelAllTasks();
    }

    void CSpxThreadService::Task::Run()
    {
        try
        {
            m_state = State::Running;
            m_task();
            m_state = State::Finished;
            m_executed.set_value(true);
        }
        catch (exception& e)
        {
            UNUSED(e);
            m_state = State::Failed;
            SPX_DBG_TRACE_ERROR("Exception happened during task execution: %s", e.what());
            m_executed.set_exception(current_exception());
        }
        catch (...)
        {
            m_state = State::Failed;
            SPX_DBG_TRACE_ERROR("Unknown exception happened during task execution");
            m_executed.set_exception(current_exception());
        }
    }

    void CSpxThreadService::Thread::Start()
    {
        if (m_started)
        {
            SPX_DBG_TRACE_ERROR("Thread has already been started");
            SPX_THROW_HR(SPXERR_INVALID_STATE);
        }

        m_started = true;
        m_thread = thread(CSpxThreadService::Thread::WorkerLoop, shared_from_this());
    }

    void CSpxThreadService::Thread::Stop(bool detached)
    {
        if (m_shouldStop || !m_started)
            return;

        if (detached)
        {
            m_thread.detach();
        }
        else if (m_thread.get_id() == this_thread::get_id())
        {
            SPX_DBG_TRACE_ERROR("Thread cannot be stopped from its own task.");
            SPX_THROW_HR(SPXERR_ABORT);
        }

        m_shouldStop = true;
        m_cv.notify_all();

        if (m_thread.joinable())
        {
            m_thread.join();
        }

        CancelAllTasks();
    }

    void CSpxThreadService::Thread::Queue(TaskPtr task)
    {
        unique_lock<mutex> lock(m_mutex);
        if (m_failed)
        {
            SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        }

        // Make sure we do not schedule new tasks
        // if the thread is being stopped.
        if (m_shouldStop)
        {
            task->MarkCanceled();
            return;
        }

        m_tasks.push_back(task);
        m_cv.notify_all();
    }

    void CSpxThreadService::Thread::Queue(DelayTaskPtr task)
    {
        unique_lock<mutex> lock(m_mutex);
        if (m_failed)
            SPX_THROW_HR(SPXERR_RUNTIME_ERROR);

        // Make sure we do not schedule new tasks
        // if the thread is being stopped.
        if (m_shouldStop)
        {
            task->MarkCanceled();
            return;
        }

        AddDelayTaskAtProperPlace(task);
        m_cv.notify_all();
    }

    bool CSpxThreadService::Thread::Cancel(TaskId id)
    {
        unique_lock<mutex> lock(m_mutex);
        return CancelTask(m_tasks, id) ||
               CancelTask(m_timerTasks, id);
    }

    void CSpxThreadService::Thread::CancelAllTasks()
    {
        unique_lock<mutex> lock(m_mutex);

        // Mark all tasks as canceled.
        MarkAllTasksCancelled(m_tasks);
        MarkAllTasksCancelled(m_timerTasks);

        // Remove all.
        RemoveAllTasks();
    }

    void CSpxThreadService::Thread::MarkFailed(const exception_ptr& e)
    {
        unique_lock<mutex> lock(m_mutex);
        m_failed = true;
        MarkAllTasksFailed(m_tasks, e);
        MarkAllTasksFailed(m_timerTasks, e);
        RemoveAllTasks();
    }

    void CSpxThreadService::Thread::RemoveAllTasks()
    {
        m_tasks.clear();
        m_timerTasks.clear();
    }

    void CSpxThreadService::Thread::AddDelayTaskAtProperPlace(DelayTaskPtr task)
    {
        if (task->CurrentState() == Task::State::Failed)
        {
            // We cannot reschedule a failed task.
            return;
        }

        task->UpdateWhen();

        auto place = upper_bound(m_timerTasks.begin(), m_timerTasks.end(), task,
            [](const DelayTaskPtr& a, const DelayTaskPtr& b) { return a->When() < b->When(); });
        if (place == m_timerTasks.end())
        {
            m_timerTasks.push_back(task);
        }
        else
        {
            m_timerTasks.insert(place, task);
        }
    }

    void CSpxThreadService::Thread::WorkerLoop(shared_ptr<Thread> self)
    {
        try
        {
            const int MaxSlice = 10;
            milliseconds delay = milliseconds(1000);
            unique_lock<mutex> lock(self->m_mutex);

            while (!self->m_shouldStop)
            {

                // Execute all tasks that have been scheduled,
                // but not more than slice tasks to avoid timer tasks starvation.
                int sliceCounter = 0;
                while (!self->m_tasks.empty() && sliceCounter++ < MaxSlice)
                {
                    auto task = self->m_tasks.front();
                    self->m_tasks.pop_front();

                    lock.unlock();
                    task->Run();

                    if (self->m_shouldStop)
                        return;

                    lock.lock();
                }

                // Execute timer tasks if required,
                // but not more than slice tasks to avoid immediate tasks starvation.
                sliceCounter = 0;
                while (!self->m_timerTasks.empty() &&
                    self->m_timerTasks.front()->When() < system_clock::now() &&
                    sliceCounter++ < MaxSlice)
                {
                    auto task = self->m_timerTasks.front();
                    self->m_timerTasks.pop_front();

                    lock.unlock();
                    task->Run();

                    if (self->m_shouldStop)
                        return;

                    lock.lock();
                }

                delay = milliseconds(200);
                if (!self->m_timerTasks.empty())
                {
                    delay = std::min(duration_cast<milliseconds>(
                        self->m_timerTasks.front()->When() - system_clock::now()), delay);
                }

                // Continue if there is some delay task that should be executed.
                if (delay.count() <= 0)
                    continue;

                // Sleeping for the delay till there is some task or we are asked to stop.
                if(self->m_tasks.empty() && !self->m_shouldStop)
                    self->m_cv.wait_for(lock, delay);
            }
        }
        // If we are in the exception handler, something really bad happened.
        // Not the task, but some of our structures misbehaved and we are in unknown state.
        // We cannot really recover, because this can lead to hangs or live lock.
        // so we abort all outstanding promises and set the whole thread to invalid state,
        // so that the next access to the thread object will result in an error.
        catch (const exception& e)
        {
            UNUSED(e);
            SPX_DBG_TRACE_ERROR("Exception caused termination of the thread service: %s", e.what());
            self->MarkFailed(current_exception());
        }
        catch (...)
        {
            SPX_DBG_TRACE_ERROR("Unknown exception happened during task execution");
            self->MarkFailed(current_exception());
        }

        self->m_shouldStop = true;
        self->CancelAllTasks();
    }

}}}} // Microsoft::CognitiveServices::Speech::Impl
