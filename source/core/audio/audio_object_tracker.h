//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once
#include <memory>
#include <map>
#include <mutex>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template<typename T, typename HANDLE>
class AudioObjectTracker
{
public:
    HANDLE Track(std::shared_ptr<T> ptr)
    {
        std::lock_guard<std::mutex> lk{ m_mutex };
        auto handle = reinterpret_cast<HANDLE>(ptr.get());
        m_items.emplace(handle, std::move(ptr));
        return handle;
    }

    void Release(HANDLE handle)
    {
        std::lock_guard<std::mutex> lk{ m_mutex };
        m_items.erase(handle);
    }

    std::shared_ptr<T> Get(HANDLE handle)
    {
        std::lock_guard<std::mutex> lk{ m_mutex };
        auto it = m_items.find(handle);
        if (it == m_items.end())
        {
            return nullptr;
        }
        return it->second;
    }

private:
    mutable std::mutex m_mutex;
    std::map<HANDLE, std::shared_ptr<T>> m_items;
};

} } } }
