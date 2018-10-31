//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include <queue>
#include "spxcore_common.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include "packaged_task_helpers.h"
#include "service_helpers.h"
#include "audio_buffer.h"

#include <shared_mutex>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxAudioSessionEventThreadService : public std::enable_shared_from_this<CSpxAudioSessionEventThreadService>
{
public:
    CSpxAudioSessionEventThreadService();
    ~CSpxAudioSessionEventThreadService();

    enum EventType { SessionStart, SessionStop, SpeechStart, SpeechEnd, RecoResultEvent };

    void FireEvent(EventType sessionType, std::list<std::weak_ptr<ISpxRecognizer>> recognizers, std::shared_ptr<ISpxRecognitionResult> result, const std::wstring& sessionId, uint64_t offset);

    void Shutdown();

private:
    struct FireEventDataRecord
    {
        std::list<std::weak_ptr<ISpxRecognizer>> weakRecognizers;
        EventType sessionType;
        std::shared_ptr<ISpxRecognitionResult> result;
        std::wstring sessionId;
        uint64_t offset;
    };
    
    void WorkerThread();
    void FireEvent(FireEventDataRecord&& ev);

    // Shutdown() takes a self-reference to the instance and signals
    // the worker thread to cleanup and terminate.
    // The reason for this is that Shutdown() will signal the thread to terminate,
    // however, since the worker thread is detached - and does the cleanup! - this will
    // ensure that the instance memory is alive during this period.
    // As a last step, the worker thread will reset this pointer, and thus trigger
    // the destructor.
    std::shared_ptr<CSpxAudioSessionEventThreadService> m_keepAlive;
    std::atomic<bool> m_fireEventQueueThreadTerminate;

    // The worker thread associated with this instance.
    std::thread m_firedEventQueueThread;

    // This is the queue of events to be delivered to the attached listeners.
    // The worker thread will take one at a time and call any registered client
    // sequentially.
    // The mutex is used to guard changes to the queue (adding and removing elements)
    // The condition variable is used to signal that the worker thread should cleanup
    // and terminate.
    std::mutex m_firedEventsQueueMutex;
    std::queue<FireEventDataRecord> m_firedEventsQueue;
    std::condition_variable m_firedEventsQueueCv;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
