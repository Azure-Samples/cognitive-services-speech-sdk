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
#include <mutex>
#include <algorithm>
#include <speechapi_cxx_common.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


/// <summary>
/// Clients can connect to the event signal to receive events, or disconnect from the event signal to stop receiving events.
/// </summary>
/// <remarks>
/// At construction time, connect and disconnect callbacks can be provided that are called when
/// the number of connected clients changes from zero to one or one to zero, respectively.
/// </remarks>
// <typeparam name="T">
template <class T>
class EventSignal
{
public:

    /// <summary>
    /// Type for callbacks used when any client connects to the signal (the number of connected clients changes from zero to one) or
    /// the last client disconnects from the signal (the number of connected clients changes from one to zero).
    /// </summary>
    using NotifyCallback_Type = std::function<void(EventSignal<T>&)>;

    /// <summary>
    /// Constructs an event signal with empty register and disconnect callbacks.
    /// <summary>
    EventSignal() :
        m_connectedCallback(nullptr),
        m_disconnectedCallback(nullptr)
    {
    };

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="connected">Callback to invoke if the number of connected clients changes from zero to one.</param>
    /// <param name="disconnected">Callback to invoke if the number of connected clients changes from one to zero.</param>
    EventSignal(NotifyCallback_Type connected, NotifyCallback_Type disconnected) :
        m_connectedCallback(connected),
        m_disconnectedCallback(disconnected)
    {
    };

    /// <summary>
    /// Destructor.
    /// <summary>
    /// <remarks>
    /// No disconnect callback will be called.
    /// <remarks>
    virtual ~EventSignal()
    {
        {
            std::unique_lock<std::recursive_mutex> lock(m_mutex);
            m_connectedCallback = nullptr;
            m_disconnectedCallback = nullptr;
        }
        DisconnectAll();
    };

    /// <summary>
    /// Callback type that is used for signalling the event to connected clients.
    /// </summary>
    using CallbackFunction = std::function<void(T eventArgs)>;

    /// <summary>
    /// Addition assignment operator overload.
    /// Connects the provided callback <paramref name="callback"/> to the event signal, see also <see cref="Connect"/>.
    /// </summary>
    /// <param name="callback">Callback to connect.</param>
    EventSignal<T>& operator+=(CallbackFunction callback)
    {
        Connect(callback);
        return *this;
    };

    /// <summary>
    /// Subtraction assignment operator overload.
    /// Disconnects the provided callback <paramref name="callback"/> from the event signal, see also <see cref="Disconnect"/>.
    /// </summary>
    /// <param name="callback">Callback to disconnect.</param>
    EventSignal<T>& operator-=(CallbackFunction callback)
    {
        Disconnect(callback);
        return *this;
    };

    /// <summary>
    /// Function call operator.
    /// Signals the event with given arguments <paramref name="t"/> to connected clients, see also <see cref="Signal"/>.
    /// </summary>
    /// <param name="t">Event arguments to signal.</param>
    void operator()(T t)
    {
        Signal(t);
    }

    /// <summary>
    /// Connects given callback function to the event signal, to be invoked when the event is signalled.
    /// </summary>
    /// <remark>
    /// When the number of connected clients changes from zero to one, the connect callback will be called, if provided.
    /// </remark>
    /// <param name="callback">Callback to connect.</param>
    void Connect(CallbackFunction callback)
    {
        std::unique_lock<std::recursive_mutex> lock(m_mutex);
        m_callbacks.push_back(callback);

        if (m_callbacks.size() == 1 && m_connectedCallback != nullptr)
        {
            m_connectedCallback(*this);
        }
    };

    /// <summary>
    /// Disconnects given callback.
    /// <summary>
    /// <remark>
    /// When the number of connected clients changes from one to zero, the disconnect callback will be called, if provided.
    /// </remark>
    void Disconnect(CallbackFunction callback)
    {
        std::unique_lock<std::recursive_mutex> lock(m_mutex);
        auto prevSize = m_callbacks.size();

        m_callbacks.remove_if([&](CallbackFunction item) {
            return callback.target_type() == item.target_type(); 
        });

        if (m_callbacks.empty() && prevSize > 0 && m_disconnectedCallback != nullptr)
        {
            m_disconnectedCallback(*this);
        }
    };

    /// <summary>
    /// Disconnects all registered callbacks.
    /// <summary>
    void DisconnectAll()
    {
        std::unique_lock<std::recursive_mutex> lock(m_mutex);
        auto prevSize = m_callbacks.size();

        m_callbacks.clear();

        if (prevSize > 0 && m_disconnectedCallback != nullptr)
        {
            m_disconnectedCallback(*this);
        }
    };

    /// <summary>
    /// Signals the event with given arguments <paramref name="t"/> to all connected callbacks.
    /// <summary>
    /// <param name="t">Event arguments to signal.</param>
    void Signal(T t)
    {
        std::unique_lock<std::recursive_mutex> lock(m_mutex);
        auto copy = m_callbacks;
        for (auto c3 : copy)
        {
            // now, while a callback is in progress, it can disconnect itself and any other connected
            // callback. Check to see if the next one stored in the copy container is still connected.
            bool connected = (std::find_if(m_callbacks.begin(), m_callbacks.end(), 
                [&](CallbackFunction item) {
                return c3.target_type() == item.target_type();
            }) != m_callbacks.end());

            if (connected)
            {
                c3(t);
            }
        }
    };

    /// <summary>
    /// Checks if a callback is connected.
    /// <summary>
    /// <returns>true if a callback is connected</returns>
    bool IsConnected() const 
    {
        return !m_callbacks.empty();
    };

private:

    EventSignal(const EventSignal&) = delete;
    EventSignal(const EventSignal&&) = delete;

    EventSignal& operator=(const EventSignal&) = delete;

    std::list<CallbackFunction> m_callbacks;
    std::recursive_mutex m_mutex;

    NotifyCallback_Type m_connectedCallback;
    NotifyCallback_Type m_disconnectedCallback;
};


} } } // Microsoft::CognitiveServices::Speech
