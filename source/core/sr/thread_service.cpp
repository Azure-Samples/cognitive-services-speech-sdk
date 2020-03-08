//
// Copyright (c) Microsoft. All rights reserved.
// See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "thread_service.h"
#include "try_catch_helpers.h"

const int timeoutInMilliseconds = 100;
const int timeoutRetryLimit = 10;

namespace Microsoft { namespace CognitiveServices { namespace Speech { namespace Impl {

    using namespace std;
    using namespace std::chrono;

    void CSpxThreadService::Init()
    {
        SPX_TRACE_ERROR_IF(!m_threads.empty(), "Init should be called only once at the beginning.");
        SPX_IFTRUE_THROW_HR(!m_threads.empty(), SPXERR_INVALID_STATE);
        m_threads =
        {
            {Affinity::Background, make_shared<Thread>()},
            {Affinity::User, make_shared<Thread>()}
        };

        for (auto& t : m_threads)
            t.second->Start();
    }

    CSpxThreadService::~CSpxThreadService()
    {
        string error;
        SPXAPI_TRY()
        {
            Term();
        }
        SPXAPI_CATCH_ONLY_NO_FORCED_UNWIND()
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
        SPX_TRACE_ERROR_IF(m_threads.empty(), "Thread service should be initialized.");
        SPX_IFTRUE_THROW_HR(m_threads.empty(), SPXERR_INVALID_STATE);
    }

    CSpxThreadService::TaskId CSpxThreadService::ExecuteAsync(packaged_task<void()>&& task, Affinity affinity, promise<bool>&& executed)
    {
        CheckInitialized();

        auto taskId = m_nextTaskId++;
        auto innerTask = make_shared<Task>(taskId, move(task));
        m_threads[affinity]->Queue(innerTask, move(executed));
        return taskId;
    }

    CSpxThreadService::TaskId CSpxThreadService::ExecuteAsync(packaged_task<void()>&& task, milliseconds delay, Affinity affinity, promise<bool>&& executed)
    {
        CheckInitialized();

        auto taskId = m_nextTaskId++;
        auto innerTask = make_shared<DelayTask>(taskId, move(task), delay);
        m_threads[affinity]->Queue(innerTask, move(executed));
        return taskId;
    }

    bool CSpxThreadService::IsOnServiceThread()
    {
        CheckInitialized();

        // see if current thread is managed by thread pool
        auto id = this_thread::get_id();

        for (auto& t : m_threads) {
            if (id == t.second->Id())
                return true;
        }

        return false;
    }

    void CSpxThreadService::ExecuteSync(std::packaged_task<void()>&& task, Affinity affinity)
    {
        // are we trying to schedule synchronous work on our own thread from the same thread?
        for (const auto& t : m_threads)
        {
            if (t.first == affinity && t.second->Id() == this_thread::get_id())
            {
                SPX_TRACE_ERROR("Task cannot be executed synchronously on the thread"
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
        m_state = State::Running;
        m_task();
        m_state = State::Finished;
    }

    void CSpxThreadService::Thread::Start()
    {
        if (m_started)
        {
            SPX_TRACE_ERROR("Thread has already been started");
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
            m_shouldStop = true;
            // If thread is itself, no need to do thread join with timeout 
            if (m_thread.get_id() != this_thread::get_id())
            {
                // Wait thread to complete operation (join) with the timeoutRetryLimit
                int counter = 0;
                while (counter < timeoutRetryLimit)
                {
                    if (m_thread.joinable())
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(timeoutInMilliseconds));
                    }
                    else
                    {
                        break;
                    }
                    counter++;
                }
            }
            m_thread.detach();
        }
        else if (m_thread.get_id() == this_thread::get_id())
        {
            SPX_TRACE_ERROR("Thread cannot be stopped from its own task.");
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

    void CSpxThreadService::Thread::Queue(TaskPtr task, promise<bool>&& executed)
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

        m_tasks.push_back({ task, move(executed) });
        m_cv.notify_all();
    }

    void CSpxThreadService::Thread::Queue(DelayTaskPtr task, promise<bool>&& executed)
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

        AddDelayTaskAtProperPlace(task, move(executed));
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

    void CSpxThreadService::Thread::AddDelayTaskAtProperPlace(DelayTaskPtr task, promise<bool>&& executed)
    {
        if (task->CurrentState() == Task::State::Failed)
        {
            // We cannot reschedule a failed task.
            return;
        }

        task->UpdateWhen();
        DelayTaskAndPromise value{ task, move(executed) };
        auto place = upper_bound(m_timerTasks.begin(), m_timerTasks.end(), value,
            [](const DelayTaskAndPromise& a, const DelayTaskAndPromise& b) { return (a.first)->When() < (b.first)->When(); });

        if (place == m_timerTasks.end())
        {
            m_timerTasks.push_back({ task, move(value.second) });
        }
        else
        {
            m_timerTasks.insert(place, { task, move(value.second) });
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
                    self->RunTask(lock, self->m_tasks);

                    if (self->m_shouldStop)
                        return;

                    lock.lock();
                }

                // Execute timer tasks if required,
                // but not more than slice tasks to avoid immediate tasks starvation.
                sliceCounter = 0;
                while (!self->m_timerTasks.empty() &&
                    self->m_timerTasks.front().first->When() < system_clock::now() &&
                    sliceCounter++ < MaxSlice)
                {
                    self->RunTask(lock, self->m_timerTasks);

                    if (self->m_shouldStop)
                        return;

                    lock.lock();
                }

                delay = milliseconds(200);
                if (!self->m_timerTasks.empty())
                {
                    delay = std::min(duration_cast<milliseconds>(
                        self->m_timerTasks.front().first->When() - system_clock::now()), delay);
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
        // We cannot really recover, because this can lead to live locks.
        // We abort all outstanding promises and set the whole thread to invalid state,
        // so that the next access to the thread object will result in an error.
        catch (const exception& e)
        {
            SPX_TRACE_ERROR("Exception caused termination of the thread service: %s", e.what());
            self->MarkFailed(current_exception());
        }
#ifdef SHOULD_HANDLE_FORCED_UNWIND
        // Currently Python forcibly kills the thread by throwing __forced_unwind,
        // taking care we propagate this exception further.
        catch (abi::__forced_unwind&)
        {
            SPX_TRACE_ERROR("Caught forced unwind in a thread service thread, rethrowing");
            self->MarkFailed(make_exception_ptr(runtime_error("Forced unwind")));
            self->CancelAllTasks();
            throw;
        }
#endif
        catch (...)
        {
            SPX_TRACE_ERROR("Unknown exception happened during task execution");
            self->MarkFailed(current_exception());
        }

        self->CancelAllTasks();
    }

}}}} // Microsoft::CognitiveServices::Speech::Impl
