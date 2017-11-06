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

    using Callback = void (*) (const T& eventArgs);

    EventSignal& operator+=(Callback callback) { Connect(callback); return *this; }
    EventSignal& operator-=(Callback callback) { Disconnect(callback); return *this; }

    void operator()(const T& t) { Signal(t); }

    void Connect(Callback callback) { throw nullptr; };
    void Disconnect(Callback callback) { throw nullptr; };

    void DisconnectAll() { throw nullptr; };

    void Signal(const T& t) { throw nullptr; };

    
private:

    EventSignal(const EventSignal&) = delete;
    EventSignal(const EventSignal&&) = delete;

    EventSignal& operator=(const EventSignal&) = delete;

    std::list<Callback> m_callbacks;
};

}; // CARBON_NAMESPACE_ROOT
