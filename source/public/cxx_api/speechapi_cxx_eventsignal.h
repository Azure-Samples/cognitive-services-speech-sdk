//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_eventsignal.h: Public API declarations for EventSignal<T> C++ template class
//

#pragma once
#include <functional>
#include <list>
#include <string>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {


template <class T>
class EventSignal
{
public:

    using NotifyCallback_Type = std::function<void(EventSignal<T>&)>;

    EventSignal() :
        m_connectedCallback(nullptr),
        m_disconnectedCallback(nullptr)
    {
    };

    EventSignal(NotifyCallback_Type connected, NotifyCallback_Type disconnected) :
        m_connectedCallback(connected),
        m_disconnectedCallback(disconnected)
    {
    };

    virtual ~EventSignal(){};

    using CallbackFunction = std::function<void(T eventArgs)>;

    EventSignal<T>& operator+=(CallbackFunction callback)
    {
        Connect(callback);
        return *this;
    };

    EventSignal<T>& operator-=(CallbackFunction callback)
    {
        Disconnect(callback);
        return *this;
    };

    void operator()(T t)
    {
        Signal(t);
    }

    void Connect(CallbackFunction callback)
    {
        m_callbacks.push_back(callback);
        if (m_callbacks.size() == 1 && m_connectedCallback != nullptr)
        {
            m_connectedCallback(*this);
        }
    };

    void Disconnect(CallbackFunction callback)
    {
        auto prevSize = m_callbacks.size();

        m_callbacks.remove_if([&](CallbackFunction item) {
            return callback.target_type() == item.target_type(); 
        });

        if (m_callbacks.size() == 0 && prevSize > 0 && m_disconnectedCallback != nullptr)
        {
            m_disconnectedCallback(*this);
        }
    };

    void DisconnectAll()
    {
        auto prevSize = m_callbacks.size();

        m_callbacks.clear();

        if (m_callbacks.size() == 0 && prevSize > 0 && m_disconnectedCallback != nullptr)
        {
            m_disconnectedCallback(*this);
        }
    };

    void Signal(T t)
    {
        for (auto c3 : m_callbacks)
        {
            c3(t);
        }
    };

    bool IsConnected() const 
    {
        return m_callbacks.size() > 0;
    };

    
private:

    EventSignal(const EventSignal&) = delete;
    EventSignal(const EventSignal&&) = delete;

    EventSignal& operator=(const EventSignal&) = delete;

    std::list<CallbackFunction> m_callbacks;

    NotifyCallback_Type m_connectedCallback;
    NotifyCallback_Type m_disconnectedCallback;
};


}; // CARBON_NAMESPACE_ROOT
