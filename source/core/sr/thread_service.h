//
// Copyright (c) Microsoft. All rights reserved.
// See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <queue>
#include <map>
#include "ispxinterfaces.h"
#include "spxerror.h"
#include "interface_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxThreadService :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxThreadService
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxThreadService)
    SPX_INTERFACE_MAP_END()

    void Init() override;
    void Term() override;
    ~CSpxThreadService();

    TaskId ExecuteAsync(std::packaged_task<void()>&& task,
        Affinity affinity = Affinity::Background,
        std::promise<bool>&& executed = std::promise<bool>()) override;

    TaskId ExecuteAsync(std::packaged_task<void()>&& task,
        std::chrono::milliseconds delay,
        Affinity affinity = Affinity::Background,
        std::promise<bool>&& executed = std::promise<bool>()) override;

    void ExecuteSync(std::packaged_task<void()>&& task,
        Affinity affinity = Affinity::Background) override;

    bool Cancel(TaskId id) override;
    void CancelAllTasks() override;

    bool IsOnServiceThread();

private:
    void CheckInitialized();

    // Represents a task of execution.
    // Not thread safe: all methods are accessed only by the execution thread.
    class Task
    {
    public:
        Task(TaskId id, std::packaged_task<void()>&& task)
            : m_task(std::move(task)), m_id(id), m_state(State::New)
        {
        }
        virtual ~Task() = default;

        enum class State
        {
            New,
            Running,
            Finished,
            Failed,
            Canceled
        };

        TaskId Id() const { return m_id; }
        State CurrentState() const { return m_state; }

        virtual void Run();

        void MarkCanceled()
        {
            m_state = State::Canceled;
        }

        void MarkFailed()
        {
            m_state = State::Canceled;
        }

    protected:
        void ResetTask()
        {
            m_task.reset();
            m_state = State::New;
        }

    private:
        std::packaged_task<void()> m_task;
        TaskId m_id;
        State m_state;
    };

    using TaskPtr = std::shared_ptr<Task>;
    using TaskAndPromise = std::pair <TaskPtr, std::promise<bool>>;
    // Represents a timer task.
    class DelayTask : public Task
    {
    public:
        DelayTask(TaskId id, std::packaged_task<void()>&& task,
            std::chrono::milliseconds delay)
            : Task(id, std::move(task)), m_delay(delay)
        {}

        void UpdateWhen()
        {
            m_when = std::chrono::system_clock::now() + m_delay;
        }

        const std::chrono::time_point<std::chrono::system_clock>& When() const
        {
            return m_when;
        }

        const std::chrono::milliseconds& Delay() const
        {
            return m_delay;
        }

        void Reset()
        {
            Task::ResetTask();
        }

    private:
        std::chrono::time_point<std::chrono::system_clock> m_when;
        std::chrono::milliseconds m_delay;
    };

    using DelayTaskPtr = std::shared_ptr<DelayTask>;
    using DelayTaskAndPromise = std::pair<DelayTaskPtr, std::promise<bool>>;
    // A queue of tasks with an associated thread.
    class Thread : public std::enable_shared_from_this<Thread>
    {
    public:
        void Start();
        void Stop(bool detached = false);

        void Queue(TaskPtr task, std::promise<bool>&& executed);
        void Queue(DelayTaskPtr task, std::promise<bool>&& executed);

        bool Cancel(TaskId id);
        void CancelAllTasks();

        std::thread::id Id() const
        {
            return m_thread.get_id();
        }

    private:
        void MarkFailed(const std::exception_ptr& e);
        void RemoveAllTasks();
        void AddDelayTaskAtProperPlace(DelayTaskPtr task, std::promise<bool>&& executed);

        template<class T>
        bool CancelTask(std::deque<T>& container, TaskId id)
        {
            auto found = std::find_if(container.begin(), container.end(), [id](const T& t) { return (t.first)->Id() == id; });
            if (found != container.end())
            {
                (*found).first->MarkCanceled();
                container.erase(found);
                return true;
            }
            return false;
        }

        template<class T>
        void MarkAllTasksFailed(std::deque<T>& container, const std::exception_ptr& e)
        {
            for (auto& t : container)
            {
                t.second.set_exception(e);
            }
        }

        template<class T>
        void MarkAllTasksCancelled(std::deque<T>& container)
        {
            for (auto& t : container)
            {
                (t.first)->MarkCanceled();
                (t.second).set_value(false);
            }
        }


        template< class T>
        void RunTask( std::unique_lock<std::mutex>& lock, std::deque<T>& container)
        {
            std::promise<bool> executed = move(container.front().second);
            std::exception_ptr p = nullptr;
            try
            {
                auto task = move(container.front().first);
                container.pop_front();

                lock.unlock();
                task->Run();
            }
            catch (const std::exception& e)
            {
                SPX_DBG_TRACE_ERROR("Exception happened during task execution: %s", e.what());
                p = std::current_exception();
            }
#ifdef SHOULD_HANDLE_FORCED_UNWIND
            // Currently Python forcibly kills the thread by throwing __forced_unwind,
            // taking care we propagate this exception further.
            catch (abi::__forced_unwind&)
            {
                SPX_DBG_TRACE_ERROR("Caught forced unwind in a thread service task, rethrowing");
                executed.set_exception(make_exception_ptr(std::runtime_error("Forced unwind")));
                throw;
            }
#endif
            catch (...)
            {
                SPX_DBG_TRACE_ERROR("Unknown exception happened during task execution");
                p = std::current_exception();
            }
            if (p == nullptr)
            {
                executed.set_value(true);
            }
            else
            {
                executed.set_exception(p);
            }
        }

        static void WorkerLoop(std::shared_ptr<Thread> self);

        // Protects all state of the thread, in particular
        // queues of tasks and delayed tasks.
        std::mutex m_mutex;

        // Conditional variable to wake up a thread.
        std::condition_variable m_cv;

        std::thread m_thread;

        // Flag indicating whether the thread should stop.
        std::atomic<bool> m_shouldStop { false };

        // Flag indicating whether Start has been called.
        std::atomic<bool> m_started { false };

        std::deque<TaskAndPromise> m_tasks;
        std::deque<DelayTaskAndPromise> m_timerTasks;
        std::atomic<bool> m_failed;
    };

    using ThreadPtr = std::shared_ptr<Thread>;

    std::atomic_int m_nextTaskId { 1 };
    std::map<Affinity, ThreadPtr> m_threads;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
