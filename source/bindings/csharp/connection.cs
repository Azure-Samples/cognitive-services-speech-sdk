//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines the Connection class which manages connection of a recognizer.
    /// Added in version 1.2.0.
    /// </summary>
    public sealed class Connection : IDisposable
    {
        /// <summary>
        /// Gets an instance of Connection object for the specified recognizer.
        /// </summary>
        /// <param name="recognizer">The recognizer associated with the connection.</param>
        /// <returns>The Connection object being created.</returns>
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
        /// Defines event handler for connected event.
        /// </summary>
        public event EventHandler<ConnectionEventArgs> Connected;

        /// <summary>
        /// Defines event handler for disconnected event.
        /// </summary>
        public event EventHandler<ConnectionEventArgs> Disconnected;

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
        }

        /// <summary>
        /// This method performs cleanup of resources.
        /// The Boolean parameter <paramref name="disposing"/> indicates whether the method is called from <see cref="IDisposable.Dispose"/> (if <paramref name="disposing"/> is true) or from the finalizer (if <paramref name="disposing"/> is false).
        /// </summary>
        /// <param name="disposing">Flag to request disposal.</param>
        private void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                this.connectionImpl.Connected.DisconnectAll();
                this.connectionImpl.Disconnected.DisconnectAll();
                connectedHandler?.Dispose();
                disconnectedHandler?.Dispose();
                this.connectionImpl?.Dispose();
            }

            disposed = true;
        }

        private bool disposed = false;
        private readonly Internal.Connection connectionImpl;

        internal ConnectionEventHandlerImpl connectedHandler;
        internal ConnectionEventHandlerImpl disconnectedHandler;

        /// <summary>
        /// Define a private class which raise a C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        internal class ConnectionEventHandlerImpl : Internal.ConnectionEventListener
        {
            public ConnectionEventHandlerImpl(Connection connection, ConnectionEventType eventType)
            {
                this.connection = connection;
                this.eventType = eventType;
            }

            public override void Execute(Internal.ConnectionEventArgs eventArgs)
            {
                if (this.connection.disposed)
                {
                    return;
                }

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
