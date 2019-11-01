//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// handle_table.h: Implementation declarations/definitions for Handle Table C++  classes
//

#pragma once
#include <atomic>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <platform.h>
#include <spxdebug.h>
#include <mutex>
#include "debug_utils.h"

#ifdef _MSC_VER
#include <shared_mutex>
#endif // _MSC_VER


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

// Class to count the number of handles a CSpxHandleTable instance is tracking
class CSpxHandleCounter
{
public:
    CSpxHandleCounter(std::string name)
    {
        this->name = name;
        size = 0;
    }

    size_t Increment()
    {
        return ++size;
    }

    size_t Decrement()
    {
        return --size;
    }

    size_t Count()
    {
        return size;
    }

    std::string Name()
    {
        return this->name;
    }

private:
    std::atomic_size_t size;
    std::string name;
};

template <class T, class Handle>
class CSpxHandleTable
{
public:

    CSpxHandleTable(std::shared_ptr<CSpxHandleCounter> counter)
    {
        m_counter = counter;
    }

    ~CSpxHandleTable()
    {
    }

    Handle TrackHandle(std::shared_ptr<T> t)
    {
        Handle handle = SPXHANDLE_INVALID;
        WriteLock_Type writeLock(m_mutex);

        T* ptr = t.get();
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s ptr=0x%8p", __FUNCTION__, (void*)ptr);

        if (ptr != nullptr)
        {
            handle = reinterpret_cast<Handle>(ptr);
            SPX_DBG_TRACE_VERBOSE_IF(1, "%s type=%s handle=0x%8p, ptr=0x%8p, total=%zu", __FUNCTION__, PAL::GetTypeName<T>().c_str(), (void*)handle, (void*)ptr, m_ptrMap.size() + 1);

            m_counter->Increment();
            m_handleMap.emplace(handle, t);
            m_ptrMap.emplace(ptr, handle);
        }

        return handle;
    }

    bool IsTracked(Handle handle)
    {
        ReadLock_Type readLock(m_mutex);
        return m_handleMap.find(handle) != m_handleMap.end();
    }

    bool IsTracked(T* ptr)
    {
        ReadLock_Type readLock(m_mutex);
        return m_ptrMap.find(ptr) != m_ptrMap.end();
    }

    std::shared_ptr<T> operator[](Handle handle)
    {
        ReadLock_Type readLock(m_mutex);
        auto item = m_handleMap.find(handle);
        SPX_THROW_HR_IF(SPXERR_INVALID_ARG, item == m_handleMap.end());
        return item->second;
    }

    Handle operator[](T* ptr)
    {
        ReadLock_Type readLock(m_mutex);
        auto item = m_ptrMap.find(ptr);
        SPX_IFTRUE_THROW_HR(item == m_ptrMap.end(), SPXERR_INVALID_ARG);
        return item->second;
    }

    void StopTracking(Handle handle)
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s handle=0x%8p", __FUNCTION__, (void*)handle);
        if (IsTracked(handle))
        {
            WriteLock_Type writeLock(m_mutex);
            auto iterHandleMap = m_handleMap.find(handle);
            if (iterHandleMap != m_handleMap.end())
            {
                auto sharedPtr = iterHandleMap->second;
                auto iterPtrMap = m_ptrMap.find(sharedPtr.get());

                SPX_DBG_TRACE_VERBOSE_IF(1, "%s type=%s handle=0x%8p, ptr=0x%8p, total=%zu", __FUNCTION__, PAL::GetTypeName<T>().c_str(), (void*)handle, (void*)sharedPtr.get(), m_ptrMap.size() -1 );

                m_handleMap.erase(iterHandleMap);
                m_ptrMap.erase(iterPtrMap);
                m_counter->Decrement();

                // If the "sharedPtr" ends up being the very last reference to the "T" object
                // the scope exit will cause T's dtor to be called, which in turn could, potentially
                // result in a call back to this same handle table, but from another thread. That would
                // cause a deadlock (this thread waiting for that thread, but this thread owns the mutex
                // and that thread will never be able to obtain it)... Unless ... We unlock the write lock
                // and then have the shared_ptr release it's reference .. So ... That's what we'll do.
                writeLock.unlock();
                sharedPtr.reset();
            }
        }
    }

    void StopTracking(T* ptr)
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s ptr=0x%8x", __FUNCTION__, ptr);
        if (IsTracked(ptr))
        {
            WriteLock_Type writeLock(m_mutex);
            auto iterPtrMap = m_ptrMap.find(ptr);
            if (iterPtrMap != m_ptrMap.end())
            {
                auto handle = iterPtrMap->second;
                auto iterHandleMap = m_handleMap.find(handle);
                auto sharedPtr = iterHandleMap->second;

                SPX_DBG_TRACE_VERBOSE_IF(1, "%s type=%s handle=0x%8x, ptr=0x%8p, total=%zu", __FUNCTION__, PAL::GetTypeName<T>().c_str(), handle, (void*)sharedPtr.get(), m_ptrMap.size() -1 );

                m_ptrMap.erase(iterPtrMap);
                m_handleMap.erase(iterHandleMap);
                m_counter->Decrement();

                // If the "sharedPtr" ends up being the very last reference to the "T" object
                // the scope exit will cause T's dtor to be called, which in turn could, potentially
                // result in a call back to this same handle table, but from another thread. That would
                // cause a deadlock (this thread waiting for that thread, but this thread owns the mutex
                // and that thread will never be able to obtain it)... Unless ... We unlock the write lock
                // and then have the shared_ptr release it's reference .. So ... That's what we'll do.
                writeLock.unlock();
                sharedPtr.reset();
            }
        }
    }

    void Term()
    {
        SPX_DBG_TRACE_VERBOSE_IF(m_ptrMap.size() == 0, "%s: ZERO handles 'leaked'", __FUNCTION__);
        SPX_DBG_TRACE_WARNING_IF(m_ptrMap.size() >= 1, "%s: non-zero handles 'leaked'", __FUNCTION__);

#if _DEBUG
        for (const auto& entry : m_handleMap)
        {
            SPX_DBG_TRACE_WARNING("LEAKED HANDLE: 0x%8p,     LEAKED POINTER: 0x%8p", (void *)entry.first, (void *)entry.second.get());
        }
#endif

        WriteLock_Type lock(m_mutex);
        m_handleMap.clear();
        m_ptrMap.clear();
    }

private:

    #ifdef _MSC_VER
    using ReadWriteMutex_Type = std::shared_mutex;
    using WriteLock_Type = std::unique_lock<std::shared_mutex>;
    using ReadLock_Type = std::shared_lock<std::shared_mutex>;
    #else
    using ReadWriteMutex_Type = std::mutex;
    using WriteLock_Type = std::unique_lock<std::mutex>;
    using ReadLock_Type = std::unique_lock<std::mutex>;
    #endif

    ReadWriteMutex_Type m_mutex;
    std::unordered_multimap<Handle, std::shared_ptr<T>> m_handleMap;
    std::unordered_multimap<T*, Handle> m_ptrMap;
    std::shared_ptr<CSpxHandleCounter> m_counter;
};

class CSpxSharedPtrHandleTableManager
{
public:

    template<class T, class Handle>
    static CSpxHandleTable<T, Handle>* Get()
    {
        std::unique_lock<std::mutex> lock(s_mutex);
        auto name = typeid(T).name();

        if (s_tables->find(name) == s_tables->end())
        {
            auto counter = std::make_shared<CSpxHandleCounter>(PAL::GetTypeName<T>());
            s_counters->push_front(counter);

            auto ht = std::make_shared<CSpxHandleTable<T, Handle>>(counter);
            s_tables->emplace(name, ht.get());

            auto fn = [ht]() { ht->Term(); };
            s_termFns->push_back(fn);
        }

        return (CSpxHandleTable<T, Handle>*) (*s_tables)[name];
    }

    template<class T, class Handle>
    static std::shared_ptr<T> GetPtr(Handle handle)
    {
        auto handletable = Get<T, Handle>();
        return (*handletable)[handle];
    }

    template<class T, class Handle>
    static Handle TrackHandle(std::shared_ptr<T> t)
    {
        auto handletable = Get<T, Handle>();
        return handletable->TrackHandle(t);
    }

    static void Term()
    {
        // TODO: on OSX statics are destroyed before LibUnload is invoked,
        // so Term() should be called from the s_termFns delete first.
        // Second time, when it's called from LibUnload, bail out if statics
        // are already deleted.
        // Is there a cleaner way to do the shut-down?
        if (s_termFns == nullptr)
        {
            return;
        }

        std::unique_lock<std::mutex> lock(s_mutex);

        for (auto termFn : *s_termFns.get())
        {
            termFn();
        }

        s_tables->clear();
        s_termFns->clear();
    }

    static size_t GetTotalTrackedObjectCount()
    {
        std::unique_lock<std::mutex> lock(s_mutex);
        size_t objCount = 0;

        for (const auto &counter : *s_counters) {
            objCount += counter->Count();
        }

        return objCount;
    }

    static std::string GetHandleCountByType()
    {
        std::unique_lock<std::mutex> lock(s_mutex);
        std::string ret;

        for (auto it = s_counters->begin(); it != s_counters->end(); ++it) {
            auto counter = it->get();
            ret += counter->Name() + " " + std::to_string(counter->Count()) + "\r\n";
        }

        return ret;
    }

private:

    template<typename T>
    using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

    static std::mutex s_mutex;

    // TODO: replace const char* with std::type_index?
    static std::unique_ptr<std::map<const char*, void*>> s_tables;
    static deleted_unique_ptr<std::list<std::function<void(void)>>> s_termFns;
    static std::unique_ptr<std::list<std::shared_ptr<CSpxHandleCounter>>> s_counters;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
