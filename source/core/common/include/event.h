//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// Based on code from here:
// https://www.codeproject.com/Articles/1256352/CppEvent-How-to-Implement-Events-using-Standard-Cp

#pragma once

#include <atomic>
#include <mutex>
#include <functional>
#include <list>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    template<typename ...Args>
    class event
    {
    public:
        event() = default;

        size_t add(const std::function<void(Args...)>& callback)
        {
            std::lock_guard<std::mutex> lock(m_lock);
            event_handler handler(m_nextId++, callback);
            m_handlers.push_back(handler);
            return handler.id;
        }

        template<typename C>
        size_t add(std::shared_ptr<C> instance, void (C::* callback)(Args...))
        {
            std::weak_ptr<C> weak(instance);
            std::function<void(Args...)> boundCallback = [weak, callback](Args... args)
            {
                auto ptr = weak.lock();
                if (ptr != nullptr)
                {
                    (ptr.get()->*callback)(args...);
                }
            };

            return add(boundCallback);
        }

        void remove(const size_t id)
        {
            std::lock_guard<std::mutex> lock(m_lock);
            m_handlers.remove_if(
                [id](const auto& handler) { return handler.id == id; });
        }

        void clear()
        {
            std::lock_guard<std::mutex> lock(m_lock);
            m_handlers.clear();
        }

        void raise(Args... params)
        {
            std::list<event_handler> allHandlers;
            {
                std::lock_guard<std::mutex> lock(m_lock);
                allHandlers = m_handlers; // copy list
            }

            for (const auto& handler : allHandlers)
            {
                handler.callback(params...);
            }
        }

        inline size_t operator +=(const std::function<void(Args...)>& handler) { return add(handler); }
        inline void operator ()(Args... params) { raise(params...); }

        event(const event& other) = delete;
        event(event&& other) = delete;
        event& operator =(const event& other) = delete;
        event& operator =(event&& other) = delete;

    private:
        struct event_handler
        {
            event_handler(size_t id, std::function<void(Args...)> callback)
                : id(id), callback(callback)
            {}

            size_t id;
            std::function<void(Args...)> callback;

            inline bool operator==(const event_handler& other) const
            {
                return id == other.id;
            }
        };

        size_t m_nextId = 0;
        std::mutex m_lock;
        std::list<event_handler> m_handlers;
    };

} } } } // Microsoft::CognitiveServices::Speech::Impl
