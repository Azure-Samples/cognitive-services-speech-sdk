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
        Task(TaskId id, std::packaged_task<void()>&& task, std::promise<bool>&& executed)
            : m_task(std::move(task)), m_executed(std::move(executed)), m_id(id), m_state(State::New)
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
            m_executed.set_value(false);
        }

        void MarkFailed(const std::exception_ptr& e)
        {
            m_state = State::Canceled;
            m_executed.set_exception(std::make_exception_ptr(e));
        }

    protected:
        void ResetTask()
        {
            m_task.reset();
            m_state = State::New;
        }

    private:
        std::packaged_task<void()> m_task;
        std::promise<bool> m_executed;
        TaskId m_id;
        State m_state;
    };

    using TaskPtr = std::shared_ptr<Task>;

    // Represents a timer task.
    class DelayTask : public Task
    {
    public:
        DelayTask(TaskId id, std::packaged_task<void()>&& task, std::promise<bool>&& executed,
            std::chrono::milliseconds delay)
            : Task(id, std::move(task), std::move(executed)), m_delay(delay)
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

    // A queue of tasks with an associated thread.
    class Thread : public std::enable_shared_from_this<Thread>
    {
    public:
        void Start();
        void Stop(bool detached = false);

        void Queue(TaskPtr task);
        void Queue(DelayTaskPtr task);

        bool Cancel(TaskId id);
        void CancelAllTasks();

        std::thread::id Id() const
        {
            return m_thread.get_id();
        }

    private:
        void MarkFailed(const std::exception_ptr& e);
        void RemoveAllTasks();
        void AddDelayTaskAtProperPlace(DelayTaskPtr task);

        template<class T>
        bool CancelTask(std::deque<T>& container, TaskId id)
        {
            auto found = std::find_if(container.begin(), container.end(), [id](const T& t) { return t->Id() == id; });
            if (found != container.end())
            {
                (*found)->MarkCanceled();
                container.erase(found);
                return true;
            }
            return false;
        }

        template<class T>
        void MarkAllTasksFailed(std::deque<T>& container, const std::exception_ptr& e)
        {
            for (const auto& t : container)
                t->MarkFailed(e);
        }

        template<class T>
        void MarkAllTasksCancelled(std::deque<T>& container)
        {
            for (const auto& t : container)
                t->MarkCanceled();
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

        std::deque<TaskPtr> m_tasks;
        std::deque<DelayTaskPtr> m_timerTasks;
        std::atomic<bool> m_failed;
    };

    using ThreadPtr = std::shared_ptr<Thread>;

    std::atomic_int m_nextTaskId { 1 };
    std::map<Affinity, ThreadPtr> m_threads;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
