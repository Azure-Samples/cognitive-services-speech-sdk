//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
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
    public sealed class Connection
    {
        /// <summary>
        /// Gets the Connection instance from the specified recognizer. 
        /// </summary>
        /// <param name="recognizer">The recognizer associated with the connection.</param>
        /// <returns>The Connection instance of the recognizer.</returns>
        public static Connection FromRecognizer(Recognizer recognizer)
        {
            var connectionImpl = Internal.Connection.FromRecognizer(recognizer.recoImpl);
            return new Connection(connectionImpl);
        }

        internal Connection(Internal.Connection connectionImpl)
        {
            if (connectionImpl == null)
            {
                throw new ArgumentException("Invalid recognizer.");
            }
            this.connectionImpl = connectionImpl;
            connectedHandler = new ConnectionEventHandlerImpl(this, ConnectionEventType.ConnectedEvent);
            disconnectedHandler = new ConnectionEventHandlerImpl(this, ConnectionEventType.DisconnectedEvent);
            this.connectionImpl.Connected.Connect(connectedHandler);
            this.connectionImpl.Disconnected.Connect(disconnectedHandler);
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
        public void Open(bool forContinuousRecognition)
        {
            this.connectionImpl.Open(forContinuousRecognition);
        }

        /// <summary>
        /// Closes the connection the service.
        /// Users can optionally call Close() to manually shutdown the connection of the associated Recognizer. The call
        /// might fail, depending on the process state of the Recognizer. But the failure does not affect the state of the 
        /// associated Recognizer.
        /// </summary>
        public void Close()
        {
            this.connectionImpl.Close();
        }

        /// <summary>
        /// The Connected event to indicate that the recognizer is connected to service.
        /// In order to receive the Connected event after subscribing to it, the Connection object itself needs to be alive.
        /// If the Connection object owning this event is out of its life time, all subscribed events won't be delivered.
        /// </summary>
        public event EventHandler<ConnectionEventArgs> Connected;

        /// <summary>
        /// The Diconnected event to indicate that the recognizer is disconnected from service.
        /// In order to receive the Disconnected event after subscribing to it, the Connection object itself needs to be alive.
        /// If the Connection object owning this event is out of its life time, all subscribed events won't be delivered.
        /// </summary>
        public event EventHandler<ConnectionEventArgs> Disconnected;

        private readonly Internal.Connection connectionImpl;

        private ConnectionEventHandlerImpl connectedHandler;
        private ConnectionEventHandlerImpl disconnectedHandler;

        /// <summary>
        /// Defines a private class which raise a C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        private class ConnectionEventHandlerImpl : Internal.ConnectionEventListener
        {
            public ConnectionEventHandlerImpl(Connection connection, ConnectionEventType eventType)
            {
                this.connection = connection;
                this.eventType = eventType;
            }

            public override void Execute(Internal.ConnectionEventArgs eventArgs)
            {
                var arg = new ConnectionEventArgs(eventArgs);

                var handler = eventType == ConnectionEventType.ConnectedEvent
                    ? this.connection.Connected
                    : this.connection.Disconnected;

                if (handler != null)
                {
                    handler(this.connection, arg);
                }
            }

            private readonly Connection connection;
            private readonly ConnectionEventType eventType;
        }
    }
}
