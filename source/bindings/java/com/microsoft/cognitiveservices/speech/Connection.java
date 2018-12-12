//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.Recognizer;

/**
 * Defines the Connection class which manages connection of a recognizer.
 * Added in version 1.2.0.
 */
public final class Connection
{
     /**
      * Creates an instance of Connection object from the specified recognizer.
      * @param recognizer The recognizer associated with the connection.
      * @return The Connection object being created.
      */
    public static Connection fromRecognizer(Recognizer recognizer)
    {
        com.microsoft.cognitiveservices.speech.internal.Connection connectionImpl = com.microsoft.cognitiveservices.speech.internal.Connection.FromRecognizer(recognizer.getRecognizerImpl());
        return new Connection(connectionImpl);
    }

    /**
     * Defines event handler for connected event.
     */
    public final EventHandlerImpl<ConnectionEventArgs> connected = new EventHandlerImpl<ConnectionEventArgs>();

    /**
     * Defines event handler for disconnected event.
     */
    public final EventHandlerImpl<ConnectionEventArgs> disconnected = new EventHandlerImpl<ConnectionEventArgs>();

    /*! \cond PROTECTED */

    /**
     * This method performs cleanup of resources.
     * The Boolean parameter disposing indicates whether the method is called from Dispose (if disposing is true) or from the finalizer (if disposing is false).
     * Derived classes should override this method to dispose resource if needed.
     * @param disposing Flag to request disposal.
     */
    protected void dispose(boolean disposing) {
        if (disposed) {
            return;
        }

        if (disposing) {
            // disconnect
            connectionImpl.getConnected().DisconnectAll();
            connectionImpl.getDisconnected().DisconnectAll();

            connectedHandler.delete();
            disconnectedHandler.delete();
            connectionImpl.delete();
        }

        disposed = true;
    }

    /*! \endcond */

    private com.microsoft.cognitiveservices.speech.internal.Connection connectionImpl;
    private ConnectionEventHandlerImpl connectedHandler;
    private ConnectionEventHandlerImpl disconnectedHandler;
    private boolean disposed = false;

    private Connection(com.microsoft.cognitiveservices.speech.internal.Connection connectionImpl)
    {
        Contracts.throwIfNull(connectionImpl, "RecognizerInternalImplementation");
        this.connectionImpl = connectionImpl;
        initialize();
    }

    private void initialize()
    {
        this.connectedHandler = new ConnectionEventHandlerImpl(this, true);
        this.disconnectedHandler = new ConnectionEventHandlerImpl(this,false);
        connectionImpl.getConnected().AddEventListener(connectedHandler);
        connectionImpl.getDisconnected().AddEventListener(disconnectedHandler);
    }

    /**
     * Define a private class which raises an event when a corresponding callback is invoked from the native layer.
     */
    private class ConnectionEventHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.ConnectionEventListener {

        public ConnectionEventHandlerImpl(Connection connection, Boolean isConnectedEvent) {
            Contracts.throwIfNull(connection, "connection");

            this.connection = connection;
            this.isConnectedEvent= isConnectedEvent;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.ConnectionEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            if (connection.disposed) {
                return;
            }

            ConnectionEventArgs arg = new ConnectionEventArgs(eventArgs);
            EventHandlerImpl<ConnectionEventArgs>  handler = this.isConnectedEvent ?
                    this.connection.connected : this.connection.disconnected;

            if (handler != null) {
                handler.fireEvent(this.connection, arg);
            }
        }

        private Connection connection;
        private Boolean isConnectedEvent;
    }
}


