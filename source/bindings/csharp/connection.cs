//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Threading;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

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
            ThrowIfNull(recognizer, "null recognizer");
            IntPtr handle = IntPtr.Zero;
            ThrowIfFail(Internal.Connection.connection_from_recognizer(recognizer.recoHandle, out handle));
            return new Connection(handle);
        }

        ~Connection()
        {
            inFinalizer = true;

            if (connectionHandle != null)
            {
                LogErrorIfFail(Internal.Connection.connection_connected_set_callback(connectionHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Connection.connection_disconnected_set_callback(connectionHandle, null, IntPtr.Zero));
            }

            if (gch.IsAllocated)
            {
                gch.Free();
            }
            // keep delegates alive and set null here
            connectedCallbackDelegate = null;
            disconnectedCallbackDelegate = null;
        }

        internal Connection(IntPtr connectionPtr)
        {
            ThrowIfNull(connectionPtr, "Invalid connectionPtr.");
            connectionHandle = new InteropSafeHandle(connectionPtr, Internal.Connection.connection_handle_release);

            connectedCallbackDelegate = FireEvent_Connected;
            disconnectedCallbackDelegate = FireEvent_Disconnected;

            gch = GCHandle.Alloc(this, GCHandleType.Weak);
            ThrowIfFail(Internal.Connection.connection_connected_set_callback(connectionHandle, connectedCallbackDelegate, GCHandle.ToIntPtr(gch)));
            ThrowIfFail(Internal.Connection.connection_disconnected_set_callback(connectionHandle, disconnectedCallbackDelegate, GCHandle.ToIntPtr(gch)));
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
            ThrowIfFail(connectionHandle != null, SpxError.InvalidHandle.ToInt32());
            ThrowIfFail(Internal.Connection.connection_open(connectionHandle, forContinuousRecognition));
        }

        /// <summary>
        /// Closes the connection the service.
        /// Users can optionally call Close() to manually shutdown the connection of the associated Recognizer. The call
        /// might fail, depending on the process state of the Recognizer. But the failure does not affect the state of the 
        /// associated Recognizer.
        /// </summary>
        public void Close()
        {
            ThrowIfFail(connectionHandle != null, SpxError.InvalidHandle.ToInt32());
            ThrowIfFail(Internal.Connection.connection_close(connectionHandle));
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

        private GCHandle gch;
        private InteropSafeHandle connectionHandle;
        private ConnectionCallbackFunctionDelegate connectedCallbackDelegate;
        private ConnectionCallbackFunctionDelegate disconnectedCallbackDelegate;
        private volatile bool inFinalizer = false;

        /// <summary>
        /// Defines a private methods which raise a C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        ///

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_Connected(IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var connection = InteropSafeHandle.GetObjectFromWeakHandle<Connection>(pvContext);
                if (connection == null || connection.inFinalizer)
                    return;
                var resultEventArg = new ConnectionEventArgs(hevent);
                connection.Connected?.Invoke(connection, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(Internal.SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_Disconnected(IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var connection = InteropSafeHandle.GetObjectFromWeakHandle<Connection>(pvContext);
                if (connection == null || connection.inFinalizer)
                    return;
                var resultEventArg = new ConnectionEventArgs(hevent);
                connection.Disconnected?.Invoke(connection, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(Internal.SpxError.InvalidHandle);
            }
        }
    }
}
