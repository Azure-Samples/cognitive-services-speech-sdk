//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_eventsignal.h: Public API declarations for EventSignal<T> C++ template class
//

#pragma once
#include <list>
#include <string>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {


template <class T>
class EventSignal
{
public:

    EventSignal(){};
    virtual ~EventSignal(){};

    using Callback1 = void(*) (const T& eventArgs);
    using Callback2 = void(*) (const T& eventArgs, void* pvContext);

    EventSignal& operator+=(Callback1 callback1) { Connect(callback1); return *this; }
    EventSignal& operator-=(Callback1 callback1) { Disconnect(callback1); return *this; }

    void operator()(const T& t) { Signal(t); }

    void Connect(Callback1 callback) { throw nullptr; };
    void Disconnect(Callback1 callback) { throw nullptr; };

    void Connect(Callback2 callback, void* pvContext) { throw nullptr; };
    void Disconnect(Callback2 callback, void* pvContext = nullptr) { throw nullptr; };
    void Disconnect(void* pvContext) { throw nullptr; };

    void DisconnectAll() { throw nullptr; };

    void Signal(const T& t) { throw nullptr; };

    
private:

    EventSignal(const EventSignal&) = delete;
    EventSignal(const EventSignal&&) = delete;

    EventSignal& operator=(const EventSignal&) = delete;

    std::list<Callback1> m_callbacks1;
    std::list<std::pair<void*, Callback2>> m_callbacks2;
};


}; // CARBON_NAMESPACE_ROOT
