//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Internal;
using Microsoft.CognitiveServices.Speech.Translation;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Dialog
{
    /// <summary>
    /// Connects to a speech enabled dialog.
    /// Added in version 1.5.0
    /// </summary>
    public sealed class DialogConnector : IDisposable
    {
        /// <summary>
        /// Signal that indicates the start of a listening session.
        /// </summary>
        public event EventHandler<SessionEventArgs> SessionStarted;

        /// <summary>
        /// Signal that indicates the end of a listening session.
        /// </summary>
        public event EventHandler<SessionEventArgs> SessionStopped;

        /// <summary>
        /// Signal for events containing speech recognition results.
        /// </summary>
        public event EventHandler<SpeechRecognitionEventArgs> Recognized;

        /// <summary>
        /// Signal for events containing intermediate recognition results.
        /// </summary>
        public event EventHandler<SpeechRecognitionEventArgs> Recognizing;

        /// <summary>
        /// Signal for events relating to the cancellation of an interaction.
        /// </summary>
        public event EventHandler<SpeechRecognitionCanceledEventArgs> Canceled;

        /// <summary>
        /// Signal that an activity was received from the backing dialog.
        /// </summary>
        public event EventHandler<ActivityReceivedEventArgs> ActivityReceived;

        /// <summary>
        /// Creates a dialog connector using the default microphone input for a specified dialog configuration.
        /// </summary>
        /// <param name="config">Dialog config.</param>
        /// <returns>A dialog connector instance.</returns>
        public DialogConnector(DialogConfig config)
            : this(FromConfig(SpxFactory.dialog_connector_create_dialog_connector_from_config, config))
        {
        }

        /// <summary>
        /// Creates a dialog connector using the specified dialog and audio configuration.
        /// </summary>
        /// <param name="config">Dialog config.</param>
        /// <param name="audioConfig">Audio config.</param>
        /// <returns>A diallog connector instance.</returns>
        public DialogConnector(DialogConfig config, Audio.AudioConfig audioConfig)
            : this(FromConfig(SpxFactory.dialog_connector_create_dialog_connector_from_config, config, audioConfig))
        {
        }

        internal DialogConnector(InteropSafeHandle dialogPtr)
        {
            ThrowIfNull(dialogPtr);
            dialogConnectorHandle = dialogPtr;
            gch = GCHandle.Alloc(this, GCHandleType.Weak);

            sessionStartedCallbackDelegate = FireEvent_SessionStarted;
            sessionStoppedCallbackDelegate = FireEvent_SessionStopped;
            recognizedCallbackDelegate = FireEvent_Recognized;
            recognizingCallbackDelegate = FireEvent_Recognizing;
            canceledCallbackDelegate = FireEvent_Canceled;
            activityReceivedCallbackDelegate = FireEvent_ActivityReceived;

            ThrowIfFail(Internal.DialogConnector.dialog_connector_session_started_set_callback(dialogConnectorHandle, sessionStartedCallbackDelegate, GCHandle.ToIntPtr(gch)));
            ThrowIfFail(Internal.DialogConnector.dialog_connector_session_stopped_set_callback(dialogConnectorHandle, sessionStoppedCallbackDelegate, GCHandle.ToIntPtr(gch)));
            ThrowIfFail(Internal.DialogConnector.dialog_connector_recognized_set_callback(dialogConnectorHandle, recognizedCallbackDelegate, GCHandle.ToIntPtr(gch)));
            ThrowIfFail(Internal.DialogConnector.dialog_connector_recognizing_set_callback(dialogConnectorHandle, recognizingCallbackDelegate, GCHandle.ToIntPtr(gch)));
            ThrowIfFail(Internal.DialogConnector.dialog_connector_canceled_set_callback(dialogConnectorHandle, canceledCallbackDelegate, GCHandle.ToIntPtr(gch)));
            ThrowIfFail(Internal.DialogConnector.dialog_connector_activity_received_set_callback(dialogConnectorHandle, activityReceivedCallbackDelegate, GCHandle.ToIntPtr(gch)));
        }

        ~DialogConnector()
        {
            isDisposing = true;
            Dispose(false);
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// This method performs cleanup of resources.
        /// The Boolean parameter <paramref name="disposing"/> indicates whether the method is called from <see cref="IDisposable.Dispose"/> (if <paramref name="disposing"/> is true) or from the finalizer (if <paramref name="disposing"/> is false).
        /// </summary>
        /// <param name="disposing">Flag to request disposal.</param>
        internal void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (dialogConnectorHandle != null)
            {
                LogErrorIfFail(Internal.DialogConnector.dialog_connector_session_started_set_callback(dialogConnectorHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.DialogConnector.dialog_connector_session_stopped_set_callback(dialogConnectorHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.DialogConnector.dialog_connector_recognized_set_callback(dialogConnectorHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.DialogConnector.dialog_connector_recognizing_set_callback(dialogConnectorHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.DialogConnector.dialog_connector_canceled_set_callback(dialogConnectorHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.DialogConnector.dialog_connector_activity_received_set_callback(dialogConnectorHandle, null, IntPtr.Zero));
            }

            // Dispose of managed resources
            if (disposing)
            {
                dialogConnectorHandle.Dispose();
            }

            sessionStartedCallbackDelegate = null;
            sessionStoppedCallbackDelegate = null;
            recognizedCallbackDelegate = null;
            recognizingCallbackDelegate = null;
            canceledCallbackDelegate = null;
            activityReceivedCallbackDelegate = null;

            // Release any unmanaged resources not wrapped by safe handles
            if (gch.IsAllocated)
            {
                gch.Free();
            }
            disposed = true;
        }

        internal InteropSafeHandle dialogConnectorHandle;
        private IntPtr asyncStartContinuousHandle = IntPtr.Zero;
        private IntPtr asyncStopContinuousHandle = IntPtr.Zero;
        private IntPtr asyncStartKeywordHandle = IntPtr.Zero;
        private IntPtr asyncStopKeywordHandle = IntPtr.Zero;
        private CallbackFunctionDelegate sessionStartedCallbackDelegate;
        private CallbackFunctionDelegate sessionStoppedCallbackDelegate;
        private CallbackFunctionDelegate recognizedCallbackDelegate;
        private CallbackFunctionDelegate recognizingCallbackDelegate;
        private CallbackFunctionDelegate canceledCallbackDelegate;
        private CallbackFunctionDelegate activityReceivedCallbackDelegate;

        /// <summary>
        /// GC handle for callbacks for context.
        /// </summary>
        private GCHandle gch;

        /// <summary>
        /// disposed is a flag used to indicate if object is disposed.
        /// </summary>
        private volatile bool disposed = false;

        /// <summary>
        /// isDisposing is a flag used to indicate if object is being disposed.
        /// </summary>
        private volatile bool isDisposing = false;

        static DialogConnector GetConnectorFromContext(IntPtr context)
        {
            var connector = InteropSafeHandle.GetObjectFromWeakHandle<DialogConnector>(context);
            if (connector == null || connector.isDisposing)
            {
                return null;
            }
            return connector;
        }

        static void FireEvent<EvtArgs>(EvtArgs eventArgs, DialogConnector connector, EventHandler<EvtArgs> evt)
        {
            evt?.Invoke(connector, eventArgs);
        }

        /// <summary>
        /// Method to raise a SessionStarted C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        [MonoPInvokeCallback]
        private static void FireEvent_SessionStarted(IntPtr handle, IntPtr eventHandle, IntPtr context)
        {
            try
            {
                var connector = GetConnectorFromContext(context);
                FireEvent(new SessionEventArgs(eventHandle), connector, connector?.SessionStarted);
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }

        /// <summary>
        /// Method to raise a SessionStopped C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        [MonoPInvokeCallback]
        private static void FireEvent_SessionStopped(IntPtr handle, IntPtr eventHandle, IntPtr context)
        {
            try
            {
                var connector = GetConnectorFromContext(context);
                FireEvent(new SessionEventArgs(eventHandle), connector, connector?.SessionStopped);
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }

        /// <summary>
        /// Method to raise a Recognizing C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        [MonoPInvokeCallback]
        private static void FireEvent_Recognizing(IntPtr handle, IntPtr eventHandle, IntPtr context)
        {
            try
            {
                var connector = GetConnectorFromContext(context);
                FireEvent(new SpeechRecognitionEventArgs(eventHandle), connector, connector?.Recognizing);
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }

        /// <summary>
        /// Method to raise a Recognized C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        [MonoPInvokeCallback]
        private static void FireEvent_Recognized(IntPtr handle, IntPtr eventHandle, IntPtr context)
        {
            try
            {
                var connector = GetConnectorFromContext(context);
                FireEvent(new SpeechRecognitionEventArgs(eventHandle), connector, connector?.Recognized);
            }
            catch (InvalidOperationException)
            {
                SpxExceptionThrower.LogError(SpxError.InvalidHandle);
            }
        }


        /// <summary>
        /// Method to raise a Canceled C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        [MonoPInvokeCallback]
        private static void FireEvent_Canceled(IntPtr handle, IntPtr eventHandle, IntPtr context)
        {
            try
            {
                var connector = GetConnectorFromContext(context);
                FireEvent(new SpeechRecognitionCanceledEventArgs(eventHandle), connector, connector?.Canceled);
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }


        /// <summary>
        /// Method to raise a ActivityReceived C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        [MonoPInvokeCallback]
        private static void FireEvent_ActivityReceived(IntPtr handle, IntPtr eventHandle, IntPtr context)
        {
            try
            {
                var connector = GetConnectorFromContext(context);
                FireEvent(new ActivityReceivedEventArgs(eventHandle), connector, connector?.ActivityReceived);
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }

        private void AssertNotDisposed()
        {
            if (disposed || isDisposing)
            {
                throw new ObjectDisposedException(this.GetType().Name);
            }
        }

        /// <summary>
        /// Connects with the back end.
        /// </summary>
        /// <returns>An asynchronous operation that starts the connection.</returns>
        public Task ConnectAsync()
        {
            AssertNotDisposed();

            return Task.Run(() =>
            {
                ThrowIfNull(dialogConnectorHandle, "Invalid connector handle");
                ThrowIfFail(Internal.DialogConnector.dialog_connector_connect(dialogConnectorHandle));
            });
        }

        /// <summary>
        /// Disconnects from the back end.
        /// </summary>
        /// <returns>An asynchronous operation that starts the disconnection.</returns>
        public Task DisconnectAsync()
        {
            AssertNotDisposed();

            return Task.Run(() =>
            {
                ThrowIfNull(dialogConnectorHandle, "Invalid connector handle");
                ThrowIfFail(Internal.DialogConnector.dialog_connector_disconnect(dialogConnectorHandle));
            });
        }

        /// <summary>
        /// Sends an activity to the backing dialog.
        /// </summary>
        /// <param name="activityJSON">Activity to send as a serialized JSON</param>
        /// <returns>An asynchronous operation that starts the operation.</returns>
        public Task<string> SendActivityAsync(string activityJSON)
        {
            AssertNotDisposed();

            return Task.Run(() =>
            {
                IntPtr activityPtr = IntPtr.Zero;
                ThrowIfNull(dialogConnectorHandle, "Invalid connector handle");
                IntPtr activityJSONPtr = Utf8StringMarshaler.MarshalManagedToNative(activityJSON);
                try
                {
                    ThrowIfFail(Internal.Activity.activity_from_string(activityJSONPtr, out activityPtr));
                }
                finally
                {
                    Marshal.FreeHGlobal(activityJSONPtr);
                }
                const int guidSize = 50;
                var buffer = new StringBuilder(guidSize);
                ThrowIfFail(Internal.DialogConnector.dialog_connector_send_activity(dialogConnectorHandle, activityPtr, buffer));
                ThrowIfFail(Internal.Activity.activity_handle_release(activityPtr));
                return buffer.ToString();
            });
        }

        /// <summary>
        /// Initiates keyword recognition.
        /// </summary>
        /// <param name="model">Specifies the keyword model to be used.</param>
        /// <returns>An asynchronous operation that starts the operation.</returns>
        public Task StartKeywordRecognitionAsync(KeywordRecognitionModel model)
        {
            AssertNotDisposed();

            ThrowIfNull(dialogConnectorHandle, "Invalid connector handle");
            return Task.Run(() =>
            {
                ThrowIfNull(dialogConnectorHandle, "Invalid connector handle");
                ThrowIfFail(Internal.DialogConnector.dialog_connector_start_keyword_recognition(dialogConnectorHandle, model.keywordHandle));
            });
        }

        /// <summary>
        /// Stop keyword recognition.
        /// </summary>
        /// <returns>An asynchronous operation that starts the operation.</returns>
        public Task StopKeywordRecognitionAsync()
        {
            AssertNotDisposed();

            return Task.Run(() =>
            {
                ThrowIfNull(dialogConnectorHandle, "Invalid connector handle");
                ThrowIfFail(Internal.DialogConnector.dialog_connector_stop_keyword_recognition(dialogConnectorHandle));
            });
        }

        /// <summary>
        /// Starts a listening session that will terminate after the first utterance.
        /// </summary>
        /// <returns>An asynchronous operation that starts the operation.</returns>
        public Task ListenOnceAsync()
        {
            AssertNotDisposed();

            return Task.Run(() =>
            {
                ThrowIfNull(dialogConnectorHandle, "Invalid connector handle");
                ThrowIfFail(Internal.DialogConnector.dialog_connector_listen_once(dialogConnectorHandle));
            });
        }

        internal delegate IntPtr GetRecognizerFromConfigDelegate(out IntPtr phreco, InteropSafeHandle speechconfig, InteropSafeHandle audioInput);

        internal static InteropSafeHandle FromConfig(GetRecognizerFromConfigDelegate fromConfig, SpeechConfig speechConfig, Audio.AudioConfig audioConfig)
        {
            if (speechConfig == null) throw new ArgumentNullException(nameof(speechConfig));
            if (audioConfig == null) throw new ArgumentNullException(nameof(audioConfig));

            IntPtr recoHandlePtr = IntPtr.Zero;
            ThrowIfFail(fromConfig(out recoHandlePtr, speechConfig.configHandle, audioConfig.configHandle));
            InteropSafeHandle recoHandle = new InteropSafeHandle(recoHandlePtr, Internal.DialogConnector.dialog_connector_handle_release);
            GC.KeepAlive(speechConfig);
            GC.KeepAlive(audioConfig);
            return recoHandle;
        }

        internal static InteropSafeHandle FromConfig(GetRecognizerFromConfigDelegate fromConfig, SpeechConfig speechConfig)
        {
            if (speechConfig == null) throw new ArgumentNullException(nameof(speechConfig));

            IntPtr recoHandlePtr = IntPtr.Zero;
            IntPtr audioConfigPtr = IntPtr.Zero;
            InteropSafeHandle audioConfigHandle = new InteropSafeHandle(audioConfigPtr, null);
            ThrowIfFail(fromConfig(out recoHandlePtr, speechConfig.configHandle, audioConfigHandle));
            InteropSafeHandle recoHandle = new InteropSafeHandle(recoHandlePtr, Internal.DialogConnector.dialog_connector_handle_release);
            GC.KeepAlive(speechConfig);
            return recoHandle;
        }


    }
}
