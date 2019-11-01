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
    class event_handler
    {
    public:
        typedef std::function<void(Args...)> handler_func_type;

        event_handler() = default;

        event_handler(handler_func_type handler)
            : m_id(0), m_function(handler)
        {
            m_id = ++m_nextId;
        }

        event_handler(const event_handler& other)
            : m_id(other.m_id), m_function(other.m_function)
        {
        }

        event_handler(event_handler&& other)
            : m_id(other.m_id), m_function(std::move(other.m_function))
        {
        }

        event_handler& operator =(const event_handler& other)
        {
            m_id = other.m_id;
            m_function = other.m_function;
            return *this;
        }

        event_handler& operator =(event_handler&& other)
        {
            m_id = other.m_id;
            m_function = std::move(other.m_function);
            return *this;
        }

        size_t id() const
        {
            return m_id;
        }

        void operator()(Args... args) const
        {
            if (m_function)
            {
                m_function(args...);
            }
        }

        bool operator ==(const event_handler& other) const
        {
            return m_id == other.id;
        }

    private:
        size_t m_id;
        handler_func_type m_function;
        static std::atomic<size_t> m_nextId;
    };

    // init the static members
    template<typename ...Args> std::atomic<size_t> event_handler<Args...>::m_nextId(0);



    template<typename ...Args>
    class event
    {
    public:
        event() = default;

        size_t add(const typename event_handler<Args...>::handler_func_type& handler)
        {
            return add(event_handler<Args...>(handler));
        }

        size_t add(const event_handler<Args...>& handler)
        {
            std::lock_guard<std::mutex> lock(m_lock);
            m_handlers.push_back(handler);
            return handler.id();
        }

        template<typename C>
        size_t addUnsafe(C* instance, void (C::*callback)(Args...))
        {
            std::function<void(Args...)> boundCallback = [instance, callback](Args... args)
            {
                (instance->*callback)(args...);
            };

            return add(event_handler<Args...>(boundCallback));
        }

        template<typename C>
        size_t add(std::shared_ptr<C> instance, void (C::*callback)(Args...))
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

            return add(event_handler<Args...>(boundCallback));
        }

        void remove(size_t id)
        {
            std::lock_guard<std::mutex> lock(m_lock);
            m_handlers.remove_if(
                [id](const event_handler<Args...>& handler) { return id == handler.id(); });
        }

        void remove(const event_handler<Args...>& handler)
        {
            std::lock_guard<std::mutex> lock(m_lock);
            m_handlers.remove(handler);
        }

        void raise(Args... params)
        {
            std::list<event_handler<Args...>> allHandlers;
            {
                std::lock_guard<std::mutex> lock(m_lock);
                allHandlers = m_handlers; // copy list
            }

            for (const event_handler<Args...>& handler : allHandlers)
            {
                handler(params...);
            }
        }

        inline size_t operator +=(const event_handler<Args...>& handler) { return add(handler); }
        inline size_t operator +=(const typename event_handler<Args...>::handler_func_type& handler) { return add(handler); }
        inline void operator -=(const event_handler<Args...>& handler) { remove(handler); }
        inline void operator ()(Args... params) { raise(params...); }

        event(const event& other) = delete;
        event(event&& other) = delete;
        event& operator =(const event& other) = delete;
        event& operator =(event&& other) = delete;

    private:
        std::mutex m_lock;
        std::list<event_handler<Args...>> m_handlers;
    };


} } } } // Microsoft::CognitiveServices::Speech::Impl
