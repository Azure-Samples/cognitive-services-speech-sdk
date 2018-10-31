//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <future>
#include <list>
#include <memory>
#include "audio_stream_session.h"
#include "spxcore_common.h"
#include "asyncop.h"
#include "create_object_helpers.h"
#include "guid_utils.h"
#include "string_utils.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "site_helpers.h"
#include "service_helpers.h"
#include "property_id_2_name_map.h"
#include "try_catch_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxAudioSessionEventThreadService::CSpxAudioSessionEventThreadService() :
    m_fireEventQueueThreadTerminate(false)
{
    m_firedEventQueueThread = std::thread(&CSpxAudioSessionEventThreadService::WorkerThread, this);
    m_firedEventQueueThread.detach();
}

CSpxAudioSessionEventThreadService::~CSpxAudioSessionEventThreadService()
{
    SPX_DBG_ASSERT(m_fireEventQueueThreadTerminate == true);
}

void CSpxAudioSessionEventThreadService::Shutdown()
{
    // note: we take a keepalive here since
    // we cannot be sure the caller keeps the instance
    // alive until the thread is done.
    if (!m_fireEventQueueThreadTerminate)
    {
        m_fireEventQueueThreadTerminate = true;

        m_keepAlive = this->shared_from_this();
        m_firedEventsQueueCv.notify_all();
    }
}

void CSpxAudioSessionEventThreadService::WorkerThread()
{
    while (!m_fireEventQueueThreadTerminate)
    {
        try
        {
            std::unique_lock<std::mutex> lock(m_firedEventsQueueMutex);

            m_firedEventsQueueCv.wait(lock, [&] { return m_fireEventQueueThreadTerminate || !m_firedEventsQueue.empty(); });
            if (m_fireEventQueueThreadTerminate)
            {
                m_keepAlive = nullptr;
                return;
            }

            if (m_firedEventsQueue.empty())
            {
                continue;
            }

            FireEventDataRecord ev = m_firedEventsQueue.front();
            m_firedEventsQueue.pop();

            lock.unlock();

            FireEvent(std::move(ev));
        }
        catch (SPXHR hrx)
        {
            UNUSED(hrx); // Release builds
            SPX_DBG_TRACE_WARNING("EventDelivery failed with %s", stringify(hrx).c_str());
        }
        catch (const ExceptionWithCallStack& ex)
        {
            UNUSED(ex); // Release builds

            auto msg = stringify(ex.GetErrorCode());
            msg += " ";
            msg += ex.GetCallStack();
            SPX_DBG_TRACE_WARNING("EventDelivery failed with - ExceptionWithCallStack - %s", msg.c_str());
        }
        catch (const std::runtime_error& ex)
        {
            UNUSED(ex); // Release builds

            SPX_DBG_TRACE_WARNING("EventDelivery failed with - runtime_error - %s", ex.what());
        }
        catch (...)
        {
            SPX_DBG_TRACE_WARNING("EventDelivery failed with - ... - %s", stringify(SPXERR_UNHANDLED_EXCEPTION).c_str());
        }
    }

    // in case, keep alive is active, reset it here.
    if (m_keepAlive)
    {
        m_keepAlive = nullptr;
    }
}

void CSpxAudioSessionEventThreadService::FireEvent(FireEventDataRecord&& ev)
{
    if (m_fireEventQueueThreadTerminate)
        return;

    for (auto weakRecognizer : ev.weakRecognizers)
    {
        // don't do anything if we are supposed to stop
        if (m_fireEventQueueThreadTerminate) break;

        try
        {
            auto recognizer = weakRecognizer.lock();
            if (recognizer)
            {
                auto ptr = SpxQueryInterface<ISpxRecognizerEvents>(recognizer);
                if (ptr)
                {
                    // don't do anything if we are supposed to stop
                    if (m_fireEventQueueThreadTerminate) break;

                    switch (ev.sessionType)
                    {
                    case CSpxAudioSessionEventThreadService::EventType::SessionStart:
                        ptr->FireSessionStarted(ev.sessionId);
                        break;

                    case CSpxAudioSessionEventThreadService::EventType::SessionStop:
                        ptr->FireSessionStopped(ev.sessionId);
                        break;

                    case CSpxAudioSessionEventThreadService::EventType::SpeechStart:
                        ptr->FireSpeechStartDetected(ev.sessionId, ev.offset);
                        break;

                    case CSpxAudioSessionEventThreadService::EventType::SpeechEnd:
                        ptr->FireSpeechEndDetected(ev.sessionId, ev.offset);
                        break;

                    case CSpxAudioSessionEventThreadService::EventType::RecoResultEvent:
                        ptr->FireResultEvent(ev.sessionId, ev.result);
                        break;

                    default:
                        SPX_DBG_TRACE_WARNING("EventDelivery unknown event type %d", (int)ev.sessionType);
                        SPX_THROW_HR(SPXERR_INVALID_STATE);
                    }
                }
            }
        }
        catch (SPXHR hrx)
        {
            UNUSED(hrx); // Release builds

            SPX_DBG_TRACE_WARNING("EventDelivery failed with %s", stringify(hrx).c_str());
        }
        catch (const ExceptionWithCallStack& ex)
        {
            UNUSED(ex); // Release builds

            auto msg = stringify(ex.GetErrorCode());
            msg += " ";
            msg += ex.GetCallStack();
            SPX_DBG_TRACE_WARNING("EventDelivery failed with %s", msg.c_str());
        }
        catch (const std::runtime_error& ex)
        {
            UNUSED(ex); // Release builds

            SPX_DBG_TRACE_WARNING("EventDelivery failed with %s", ex.what());
        }
        catch (...)
        {
            SPX_DBG_TRACE_WARNING("EventDelivery failed with %s", stringify(SPXERR_UNHANDLED_EXCEPTION).c_str());
        }
    }
}

void CSpxAudioSessionEventThreadService::FireEvent(EventType sessionType, std::list<std::weak_ptr<ISpxRecognizer>> weakRecognizers, std::shared_ptr<ISpxRecognitionResult> result, const std::wstring& sessionId, uint64_t offset)
{
    // don't do anything if we are supposed to stop
    if (m_fireEventQueueThreadTerminate)
        return;

    try
    {
        FireEventDataRecord ev { weakRecognizers, sessionType, result, sessionId, offset };

        std::unique_lock<std::mutex> lock(m_firedEventsQueueMutex);
        m_firedEventsQueue.emplace(std::move(ev));
        m_firedEventsQueueCv.notify_all();
    }
    catch (...)
    {
        SPX_DBG_TRACE_WARNING("CSpxAudioSessionEventThreadService::FireEvent enqueue failed with %s", stringify(SPXERR_UNHANDLED_EXCEPTION).c_str());
    }
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
