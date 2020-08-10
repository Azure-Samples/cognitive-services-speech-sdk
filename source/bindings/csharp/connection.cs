//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Internal;
using ConversationTranslator = Microsoft.CognitiveServices.Speech.Transcription.ConversationTranslator;
using DialogServiceConnector = Microsoft.CognitiveServices.Speech.Dialog.DialogServiceConnector;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Connection is a proxy class for managing connection to the speech service of the specified Recognizer.
    /// By default, a Recognizer autonomously manages connection to service when needed.
    /// The Connection class provides additional methods for users to explicitly open or close a connection and
    /// to subscribe to connection status changes.
    /// The use of Connection is optional. It is intended for scenarios where fine tuning of application
    /// behavior based on connection status is needed. Users can optionally call Open() to manually
    /// initiate a service connection before starting recognition on the Recognizer associated with this Connection.
    /// After starting a recognition, calling Open() or Close() might fail. This will not impact
    /// the Recognizer or the ongoing recognition. Connection might drop for various reasons, the Recognizer will
    /// always try to reinstitute the connection as required to guarantee ongoing operations. In all these cases
    /// Connected/Disconnected events will indicate the change of the connection status.
    /// Added in version 1.2.0.
    /// </summary>
    public sealed class Connection : IDisposable
    {
        /// <summary>
        /// Gets the Connection instance from the specified recognizer.
        /// </summary>
        /// <param name="recognizer">The recognizer associated with the connection.</param>
        /// <returns>The Connection instance of the recognizer.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public static Connection FromRecognizer(Recognizer recognizer)
        {
            ThrowIfNull(recognizer, "null recognizer");
            IntPtr handle = IntPtr.Zero;
            ThrowIfFail(Internal.Connection.connection_from_recognizer(recognizer.recoHandle, out handle));
            return new Connection(handle);
        }

        /// <summary>
        /// Gets the Connection instance from the conversation translator.
        /// </summary>
        /// <param name="convTrans">The conversation translator associated with the connection.</param>
        /// <returns>The Connection instance of the conversation translator.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public static Connection FromConversationTranslator(ConversationTranslator convTrans)
        {
            ThrowIfNull(convTrans, "null conversation translator");
            IntPtr handle = IntPtr.Zero;
            ThrowIfFail(Internal.Connection.connection_from_conversation_translator(convTrans._nativeHandle, out handle));
            return new Connection(handle);
        }


        /// <summary>
        /// Gets the Connection instance from the specified dialog service connector, used for observing and managing
        /// connection and disconnection from the speech service.
        /// </summary>
        /// <param name="dialogServiceConnector">The dialog service connector associated with the connection.</param>
        /// <returns>The Connection instance of the dialog service connector.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public static Connection FromDialogServiceConnector(DialogServiceConnector dialogServiceConnector)
        {
            ThrowIfNull(dialogServiceConnector, "null dialog service connector");
            IntPtr handle = IntPtr.Zero;
            ThrowIfFail(Internal.Connection.connection_from_dialog_service_connector(dialogServiceConnector.dialogServiceConnectorHandle, out handle));
            return new Connection(handle);
        }

        ~Connection()
        {
            isDisposing = true;
            Dispose(false);
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public void Dispose()
        {
            try
            {
                isDisposing = true;
                lock (connectionLock)
                {
                    if (activeAsyncConnectionCounter != 0)
                    {
                        throw new InvalidOperationException("Cannot dispose a connection while async operation is running. Await async connections to avoid unexpected disposals.");
                    }
                }
            }
            finally
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }
        }

        /// <summary>
        /// This method performs cleanup of resources.
        /// The Boolean parameter <paramref name="disposing"/> indicates whether the method is called from <see cref="IDisposable.Dispose"/> (if <paramref name="disposing"/> is true) or from the finalizer (if <paramref name="disposing"/> is false).
        /// Derived classes should override this method to dispose resource if needed.
        /// </summary>
        /// <param name="disposing">Flag to request disposal.</param>
        private void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (connectionHandle != null)
            {
                LogErrorIfFail(Internal.Connection.connection_connected_set_callback(connectionHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Connection.connection_disconnected_set_callback(connectionHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Connection.connection_message_received_set_callback(connectionHandle, null, IntPtr.Zero));
            }

            // Dispose of managed resources
            if (disposing)
            {
                connectionHandle?.Dispose();
            }

            if (gch.IsAllocated)
            {
                gch.Free();
            }
            disposed = true;

            // keep delegates alive and set null here
            connectedCallbackDelegate = null;
            disconnectedCallbackDelegate = null;
            messageReceivedCallbackDelegate = null;
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        internal Connection(IntPtr connectionPtr)
        {
            ThrowIfNull(connectionPtr, "Invalid connectionPtr.");
            connectionHandle = new InteropSafeHandle(connectionPtr, Internal.Connection.connection_handle_release);

            connectedCallbackDelegate = FireEvent_Connected;
            disconnectedCallbackDelegate = FireEvent_Disconnected;
            messageReceivedCallbackDelegate = FireEvent_MessageReceived;

            gch = GCHandle.Alloc(this, GCHandleType.Weak);
        }

        /// <summary>
        /// Starts to set up connection to the service.
        /// Users can optionally call Open() to manually set up a connection in advance before starting recognition on the
        /// Recognizer associated with this Connection. After starting recognition, calling Open() might fail, depending on
        /// the process state of the Recognizer. But the failure does not affect the state of the associated Recognizer.
        /// Note: On return, the connection might not be ready yet. Please subscribe to the Connected event to
        /// be notified when the connection is established.
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
        /// Sends a message to the speech service.
        /// Added in version 1.7.0.
        /// </summary>
        /// <param name="path">The path of the message.</param>
        /// <param name="payload">The payload of the message. This is a json string.</param>
        /// <returns>A task representing the asynchronous operation that sends the message.</returns>
        public Task SendMessageAsync(string path, string payload)
        {
            return Task.Run(() =>
            {
                DoAsyncConnectionAction(() => SendMessage(path, payload));
            });
        }

        /// <summary>
        /// Sends a binary message to the speech service.
        /// Added in version 1.10.0.
        /// </summary>
        /// <param name="path">The path of the message.</param>
        /// <param name="payload">The binary payload of the message.</param>
        /// <param name="size">The size of the binary payload.</param>
        /// <returns>A task representing the asynchronous operation that sends the message.</returns>
        public Task SendMessageAsync(string path, byte[] payload, uint size)
        {
            return Task.Run(() =>
            {
                DoAsyncConnectionAction(() => SendMessageData(path, payload, size));
            });
        }

        /// <summary>
        /// Appends a parameter in a message to service.
        /// Added in version 1.7.0.
        /// </summary>
        /// <param name="path">The path of the network message.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="propertyValue">Value of the property. This is a json string.</param>
        public void SetMessageProperty(string path, string propertyName, string propertyValue)
        {
           ThrowIfFail(connectionHandle != null, SpxError.InvalidHandle.ToInt32());
           IntPtr propertyNamePtr = Utf8StringMarshaler.MarshalManagedToNative(propertyName);
           IntPtr propertyValuePtr = Utf8StringMarshaler.MarshalManagedToNative(propertyValue);
           ThrowIfFail(Internal.Connection.connection_set_message_property(connectionHandle, path, propertyNamePtr, propertyValuePtr));
        }

        private event EventHandler<ConnectionEventArgs> _Connected;
        private event EventHandler<ConnectionEventArgs> _Disconnected;
        private event EventHandler<ConnectionMessageEventArgs> _MessageReceived;

        /// <summary>
        /// The Connected event to indicate that the recognizer is connected to service.
        /// In order to receive the Connected event after subscribing to it, the Connection object itself needs to be alive.
        /// If the Connection object owning this event is out of its life time, all subscribed events won't be delivered.
        /// </summary>
        public event EventHandler<ConnectionEventArgs> Connected
        {
            add
            {
                if (this._Connected == null)
                {
                    ThrowIfFail(Internal.Connection.connection_connected_set_callback(connectionHandle, connectedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Connected += value;
            }
            remove
            {
                this._Connected -= value;
                if (this._Connected == null)
                {
                    if (connectionHandle != null)
                    {
                        LogErrorIfFail(Internal.Connection.connection_connected_set_callback(connectionHandle, null, IntPtr.Zero));
                    }
                }
            }
        }

        /// <summary>
        /// The Disconnected event to indicate that the recognizer is disconnected from service.
        /// In order to receive the Disconnected event after subscribing to it, the Connection object itself needs to be alive.
        /// If the Connection object owning this event is out of its life time, all subscribed events won't be delivered.
        /// </summary>
        public event EventHandler<ConnectionEventArgs> Disconnected
        {
            add
            {
                if (this._Disconnected == null)
                {
                    ThrowIfFail(Internal.Connection.connection_disconnected_set_callback(connectionHandle, disconnectedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Disconnected += value;
            }
            remove
            {
                this._Disconnected -= value;
                if (this._Disconnected == null)
                {
                    if (connectionHandle != null)
                    {
                        LogErrorIfFail(Internal.Connection.connection_disconnected_set_callback(connectionHandle, null, IntPtr.Zero));
                    }
                }
            }
        }

        /// <summary>
        /// The MessageReceived event indicates that the service has sent a network message to the client.
        /// </summary>
        public event EventHandler<ConnectionMessageEventArgs> MessageReceived
        {
            add
            {
                if (this._MessageReceived == null)
                {
                    ThrowIfFail(Internal.Connection.connection_message_received_set_callback(connectionHandle, messageReceivedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._MessageReceived += value;
            }
            remove
            {
                this._MessageReceived -= value;
                if (this._MessageReceived == null)
                {
                    if (connectionHandle != null)
                    {
                        LogErrorIfFail(Internal.Connection.connection_message_received_set_callback(connectionHandle, null, IntPtr.Zero));
                    }
                }
            }
        }

        private GCHandle gch;
        volatile bool disposed = false;
        volatile bool isDisposing = false;
        private InteropSafeHandle connectionHandle;
        private ConnectionCallbackFunctionDelegate connectedCallbackDelegate;
        private ConnectionCallbackFunctionDelegate disconnectedCallbackDelegate;
        private ConnectionCallbackFunctionDelegate messageReceivedCallbackDelegate;
        object connectionLock = new object();
        private int activeAsyncConnectionCounter = 0;

        /// <summary>
        /// Defines a private methods which raise a C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        ///

        [MonoPInvokeCallback(typeof(ConnectionCallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_Connected(IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var connection = InteropSafeHandle.GetObjectFromWeakHandle<Connection>(pvContext);
                if (connection == null || connection.isDisposing)
                    return;
                var resultEventArg = new ConnectionEventArgs(hevent);
                connection._Connected?.Invoke(connection, resultEventArg);
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }

        [MonoPInvokeCallback(typeof(ConnectionCallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_Disconnected(IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var connection = InteropSafeHandle.GetObjectFromWeakHandle<Connection>(pvContext);
                if (connection == null || connection.isDisposing)
                    return;
                var resultEventArg = new ConnectionEventArgs(hevent);
                connection._Disconnected?.Invoke(connection, resultEventArg);
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }

        [MonoPInvokeCallback(typeof(ConnectionCallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_MessageReceived(IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var connection = InteropSafeHandle.GetObjectFromWeakHandle<Connection>(pvContext);
                if (connection == null || connection.isDisposing)
                    return;
                var eventArgs = new ConnectionMessageEventArgs(hevent);
                connection._MessageReceived?.Invoke(connection, eventArgs);
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }

        void DoAsyncConnectionAction(Action connectionImplAction)
        {
            lock(connectionLock)
            {
                activeAsyncConnectionCounter++;
            }
            if (disposed || isDisposing)
            {
                throw new ObjectDisposedException(this.GetType().Name);
            }

            try
            {
                connectionImplAction();
            }
            finally
            {
                lock (connectionLock)
                {
                    activeAsyncConnectionCounter--;
                }
            }

        }

        internal void SendMessage(string path, string payload)
        {
            ThrowIfFail(connectionHandle != null, SpxError.InvalidHandle.ToInt32());
            IntPtr payloadPtr = Utf8StringMarshaler.MarshalManagedToNative(payload);
            try
            {
                ThrowIfFail(Internal.Connection.connection_send_message(connectionHandle, path, payloadPtr));
            }
            finally
            {
                Marshal.FreeHGlobal(payloadPtr);
            }
        }

        internal void SendMessageData(string path, byte[] payload, UInt32 size)
        {
            ThrowIfFail(connectionHandle != null, SpxError.InvalidHandle.ToInt32());
            ThrowIfFail(Internal.Connection.connection_send_message_data(connectionHandle, path, payload, size));
        }
    }
}