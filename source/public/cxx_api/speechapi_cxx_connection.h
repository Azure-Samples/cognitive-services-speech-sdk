//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_recognizer.h>
#include <speechapi_cxx_eventsignal.h>
#include <speechapi_cxx_connection_eventargs.h>
#include <speechapi_c.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Connection is a proxy class for managing connection to the speech service of the specified Recognizer.
/// By default, a Recognizer autonomously manages connection to service when needed. 
/// The Connection class provides additional methods for users to explicitly open or close a connection and 
/// to subscribe to connection status changes.
/// The use of Connection is optional, and mainly for scenarios where fine tuning of application
/// behavior based on connection status is needed. Users can optionally call Open() to manually set up a connection 
/// in advance before starting recognition on the Recognizer associated with this Connection. After starting recognition,
/// calling Open() or Close() might fail, depending on the process state of the Recognizer. But this does not affect 
/// the state of the associated Recognizer. And if the Recognizer needs to connect or disconnect to service, it will 
/// setup or shutdown the connection independently. In this case the Connection will be notified by change of connection 
/// status via Connected/Disconnected events.
/// Added in version 1.2.0.
/// </summary>
class Connection : public std::enable_shared_from_this<Connection>
{

public:
    /// <summary>
    /// Gets the Connection instance from the specified recognizer. 
    /// </summary>
    /// <param name="recognizer">The recognizer associated with the connection.</param>
    /// <returns>The Connection instance of the recognizer.</returns>
    static std::shared_ptr<Connection> FromRecognizer(std::shared_ptr<Recognizer> recognizer)
    {
        SPX_INIT_HR(hr);
        SPX_IFTRUE_THROW_HR(recognizer == nullptr, SPXERR_INVALID_ARG);

        SPXCONNECTIONHANDLE handle = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(hr = ::connection_from_recognizer(recognizer->m_hreco, &handle));

        return std::make_shared<Connection>(handle);
    }

    /// <summary>
    /// Starts to set up connection to the service.
    /// Users can optionally call Open() to manually set up a connection in advance before starting recognition on the 
    /// Recognizer associated with this Connection. After starting recognition, calling Open() might fail, depending on 
    /// the process state of the Recognizer. But the failure does not affect the state of the associated Recognizer.
    /// Note: On return, the connection might not be ready yet. Please subscribe to the Connected event to
    /// be notfied when the connection is established.
    /// </summary>
    /// <param name="forContinuousRecognition">Indicates whether the connection is used for continuous recognition or single-shot recognition.</param>
    void Open(bool forContinuousRecognition)
    {
        SPX_IFTRUE_THROW_HR(m_connectionHandle == SPXHANDLE_INVALID, SPXERR_INVALID_HANDLE);
        SPX_THROW_ON_FAIL(::connection_open(m_connectionHandle, forContinuousRecognition));
    }

    /// <summary>
    /// Closes the connection the service.
    /// Users can optionally call Close() to manually shutdown the connection of the associated Recognizer. The call
    /// might fail, depending on the process state of the Recognizer. But the failure does not affect the state of the 
    /// associated Recognizer.
    /// </summary>
    void Close()
    {
        SPX_IFTRUE_THROW_HR(m_connectionHandle == SPXHANDLE_INVALID, SPXERR_INVALID_HANDLE);
        SPX_THROW_ON_FAIL(::connection_close(m_connectionHandle));
    }

    /// <summary>
    /// The Connected event to indicate that the recognizer is connected to service.
    /// </summary>
    EventSignal<const ConnectionEventArgs&> Connected;

    /// <summary>
    /// The Diconnected event to indicate that the recognizer is disconnected from service.
    /// </summary>
    EventSignal<const ConnectionEventArgs&> Disconnected;

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    /// <param name="handle">The connection handle.</param>
    explicit Connection(SPXCONNECTIONHANDLE handle) :
        Connected(GetConnectionEventConnectionsChangedCallback(), GetConnectionEventConnectionsChangedCallback(), false),
        Disconnected(GetConnectionEventConnectionsChangedCallback(), GetConnectionEventConnectionsChangedCallback(), false),
        m_connectionHandle(handle)
    {
        SPX_DBG_TRACE_FUNCTION();
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~Connection()
    {
        SPX_DBG_TRACE_FUNCTION();

        try
        {
            Disconnected.DisconnectAll();
            Connected.DisconnectAll();
        }
        catch (const std::exception& ex)
        {
            SPX_DBG_TRACE_ERROR("Exception caught in ~Connection(): %s", ex.what());
            (void)ex;
        }
        catch (...)
        {
            SPX_DBG_TRACE_ERROR("Unknown exception happened during ~Connection().");
        }

        if (m_connectionHandle != SPXHANDLE_INVALID)
        {
            ::connection_handle_release(m_connectionHandle);
            m_connectionHandle = SPXHANDLE_INVALID;
        }
    }

private:
    DISABLE_COPY_AND_MOVE(Connection);

    SPXCONNECTIONHANDLE m_connectionHandle;

    static void FireConnectionEvent(bool firingConnectedEvent, SPXEVENTHANDLE event, void* context)
    {
        std::exception_ptr p;
        try
        {
            std::unique_ptr<ConnectionEventArgs> connectionEvent{ new ConnectionEventArgs(event) };

            auto connection = static_cast<Connection*>(context);
            auto keepAlive = connection->shared_from_this();
            if (firingConnectedEvent)
            {
                connection->Connected.Signal(*connectionEvent.get());
            }
            else
            {
                connection->Disconnected.Signal(*connectionEvent.get());
            }
        }
#ifdef SHOULD_HANDLE_FORCED_UNWIND
        // Currently Python forcibly kills the thread by throwing __forced_unwind,
        // taking care we propagate this exception further.
        catch (abi::__forced_unwind&)
        {
            SPX_DBG_TRACE_ERROR("__forced_unwind exception caught in FireConnectionEvent.");
            throw;
        }
#endif
        catch (...)
        {
            p = std::current_exception();
            SPX_DBG_TRACE_ERROR("Caught exception in FireConnectionEvent: %s. Will rethrow later.", firingConnectedEvent ? "Connected" : "Disconnected");
        }

        // ConnectionEventArgs doesn't hold hevent, and thus can't release it properly ... release it here
        SPX_DBG_ASSERT(recognizer_event_handle_is_valid(event));
        recognizer_event_handle_release(event);

        if (p != nullptr)
        {
            std::rethrow_exception(p);
        }
    }

    static void FireEvent_Connected(SPXEVENTHANDLE event, void* context)
    {
        FireConnectionEvent(true, event, context);
    }

    static void FireEvent_Disconnected(SPXEVENTHANDLE event, void* context)
    {
        FireConnectionEvent(false, event, context);
    }

    void ConnectionEventConnectionsChanged(const EventSignal<const ConnectionEventArgs&>& connectionEvent)
    {
        if (m_connectionHandle != SPXHANDLE_INVALID)
        {
            SPX_DBG_TRACE_VERBOSE("%s: m_connectionHandle=0x%8x", __FUNCTION__, m_connectionHandle);
            SPX_DBG_TRACE_VERBOSE_IF(!::connection_handle_is_valid(m_connectionHandle), "%s: m_connectionHandle is INVALID!!!", __FUNCTION__);

            if (&connectionEvent == &Connected)
            {
                SPX_THROW_ON_FAIL(connection_connected_set_callback(m_connectionHandle, Connected.IsConnected() ? FireEvent_Connected : nullptr, this));
            }
            else if (&connectionEvent == &Disconnected)
            {
                SPX_THROW_ON_FAIL(connection_disconnected_set_callback(m_connectionHandle, Disconnected.IsConnected() ? FireEvent_Disconnected : nullptr, this));
            }
        }
    }

    inline std::function<void(const EventSignal<const ConnectionEventArgs&>&)> GetConnectionEventConnectionsChangedCallback()
    {
        return [=](const EventSignal<const ConnectionEventArgs&>& connectionEvent) { this->ConnectionEventConnectionsChanged(connectionEvent); };
    }
};

} } } // Microsoft::CognitiveServices::Speech
