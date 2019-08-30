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
    public sealed class DialogServiceConnector : IDisposable
    {
        private event EventHandler<SessionEventArgs> _SessionStarted;
        private event EventHandler<SessionEventArgs> _SessionStopped;
        private event EventHandler<SpeechRecognitionEventArgs> _Recognized;
        private event EventHandler<SpeechRecognitionEventArgs> _Recognizing;
        private event EventHandler<SpeechRecognitionCanceledEventArgs> _Canceled;
        private event EventHandler<ActivityReceivedEventArgs> _ActivityReceived;


        /// <summary>
        /// Signal that indicates the start of a listening session.
        /// </summary>
        public event EventHandler<SessionEventArgs> SessionStarted
        {
            add
            {
                if (this._SessionStarted == null)
                {
                    ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_session_started_set_callback(dialogServiceConnectorHandle, sessionStartedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._SessionStarted += value;
            }
            remove
            {
                this._SessionStarted -= value;
                if (this._SessionStarted == null)
                {
                    LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_session_started_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// Signal that indicates the end of a listening session.
        /// </summary>
        public event EventHandler<SessionEventArgs> SessionStopped
        {
            add
            {
                if (this._SessionStopped == null)
                {
                    ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_session_stopped_set_callback(dialogServiceConnectorHandle, sessionStoppedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._SessionStopped += value;
            }
            remove
            {
                this._SessionStopped -= value;
                if (this._SessionStopped == null)
                {
                    LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_session_stopped_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// Signal for events containing speech recognition results.
        /// </summary>
        public event EventHandler<SpeechRecognitionEventArgs> Recognized
        {
            add
            {
                if (this._Recognized == null)
                {
                    ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_recognized_set_callback(dialogServiceConnectorHandle, recognizedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Recognized += value;
            }
            remove
            {
                this._Recognized -= value;
                if (this._Recognized == null)
                {
                    LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_recognized_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// Signal for events containing intermediate recognition results.
        /// </summary>
        public event EventHandler<SpeechRecognitionEventArgs> Recognizing
        {
            add
            {
                if (this._Recognizing == null)
                {
                    ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_recognizing_set_callback(dialogServiceConnectorHandle, recognizingCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Recognizing += value;
            }
            remove
            {
                this._Recognizing -= value;
                if (this._Recognizing == null)
                {
                    LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_recognizing_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// Signal for events relating to the cancellation of an interaction.
        /// </summary>
        public event EventHandler<SpeechRecognitionCanceledEventArgs> Canceled
        {
            add
            {
                if (this._Canceled == null)
                {
                    ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_canceled_set_callback(dialogServiceConnectorHandle, canceledCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Canceled += value;
            }
            remove
            {
                this._Canceled -= value;
                if (this._Canceled == null)
                {
                    LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_canceled_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// Signal that an activity was received from the backing dialog.
        /// </summary>
        public event EventHandler<ActivityReceivedEventArgs> ActivityReceived
        {
            add
            {
                if (this._ActivityReceived == null)
                {
                    ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_activity_received_set_callback(dialogServiceConnectorHandle, activityReceivedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._ActivityReceived += value;
            }
            remove
            {
                this._ActivityReceived -= value;
                if (this._ActivityReceived == null)
                {
                    LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_activity_received_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// Creates a dialog service connector using the default microphone input for a specified dialog service configuration.
        /// </summary>
        /// <param name="config">Dialog service config.</param>
        public DialogServiceConnector(DialogServiceConfig config)
            : this(FromConfig(SpxFactory.dialog_service_connector_create_dialog_service_connector_from_config, config))
        {
        }

        /// <summary>
        /// Creates a dialog service connector using the specified dialog and audio configuration.
        /// </summary>
        /// <param name="config">Dialog service config.</param>
        /// <param name="audioConfig">Audio config.</param>
        public DialogServiceConnector(DialogServiceConfig config, Audio.AudioConfig audioConfig)
            : this(FromConfig(SpxFactory.dialog_service_connector_create_dialog_service_connector_from_config, config, audioConfig))
        {
        }

        internal DialogServiceConnector(InteropSafeHandle dialogPtr)
        {
            ThrowIfNull(dialogPtr);
            dialogServiceConnectorHandle = dialogPtr;
            gch = GCHandle.Alloc(this, GCHandleType.Weak);

            sessionStartedCallbackDelegate = FireEvent_SessionStarted;
            sessionStoppedCallbackDelegate = FireEvent_SessionStopped;
            recognizedCallbackDelegate = FireEvent_Recognized;
            recognizingCallbackDelegate = FireEvent_Recognizing;
            canceledCallbackDelegate = FireEvent_Canceled;
            activityReceivedCallbackDelegate = FireEvent_ActivityReceived;
        }

        ~DialogServiceConnector()
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

            if (dialogServiceConnectorHandle != null)
            {
                LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_session_started_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_session_stopped_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_recognized_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_recognizing_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_canceled_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.DialogServiceConnector.dialog_service_connector_activity_received_set_callback(dialogServiceConnectorHandle, null, IntPtr.Zero));
            }

            // Dispose of managed resources
            if (disposing)
            {
                dialogServiceConnectorHandle.Dispose();
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

        internal InteropSafeHandle dialogServiceConnectorHandle;
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

        static DialogServiceConnector GetConnectorFromContext(IntPtr context)
        {
            var connector = InteropSafeHandle.GetObjectFromWeakHandle<DialogServiceConnector>(context);
            if (connector == null || connector.isDisposing)
            {
                return null;
            }
            return connector;
        }

        static void FireEvent<EvtArgs>(EvtArgs eventArgs, DialogServiceConnector connector, EventHandler<EvtArgs> evt)
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
                FireEvent(new SessionEventArgs(eventHandle), connector, connector?._SessionStarted);
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
                FireEvent(new SessionEventArgs(eventHandle), connector, connector?._SessionStopped);
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
                FireEvent(new SpeechRecognitionEventArgs(eventHandle), connector, connector?._Recognizing);
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
                FireEvent(new SpeechRecognitionEventArgs(eventHandle), connector, connector?._Recognized);
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
                FireEvent(new SpeechRecognitionCanceledEventArgs(eventHandle), connector, connector?._Canceled);
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
                FireEvent(new ActivityReceivedEventArgs(eventHandle), connector, connector?._ActivityReceived);
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
                ThrowIfNull(dialogServiceConnectorHandle, "Invalid connector handle");
                ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_connect(dialogServiceConnectorHandle));
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
                ThrowIfNull(dialogServiceConnectorHandle, "Invalid connector handle");
                ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_disconnect(dialogServiceConnectorHandle));
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
                ThrowIfNull(dialogServiceConnectorHandle, "Invalid connector handle");
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
                ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_send_activity(dialogServiceConnectorHandle, activityPtr, buffer));
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

            ThrowIfNull(dialogServiceConnectorHandle, "Invalid connector handle");
            return Task.Run(() =>
            {
                ThrowIfNull(dialogServiceConnectorHandle, "Invalid connector handle");
                ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_start_keyword_recognition(dialogServiceConnectorHandle, model.keywordHandle));
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
                ThrowIfNull(dialogServiceConnectorHandle, "Invalid connector handle");
                ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_stop_keyword_recognition(dialogServiceConnectorHandle));
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
                ThrowIfNull(dialogServiceConnectorHandle, "Invalid connector handle");
                ThrowIfFail(Internal.DialogServiceConnector.dialog_service_connector_listen_once(dialogServiceConnectorHandle));
            });
        }

        internal delegate IntPtr GetRecognizerFromConfigDelegate(out IntPtr phreco, InteropSafeHandle speechconfig, InteropSafeHandle audioInput);

        internal static InteropSafeHandle FromConfig(GetRecognizerFromConfigDelegate fromConfig, SpeechConfig speechConfig, Audio.AudioConfig audioConfig)
        {
            if (speechConfig == null) throw new ArgumentNullException(nameof(speechConfig));
            if (audioConfig == null) throw new ArgumentNullException(nameof(audioConfig));

            IntPtr recoHandlePtr = IntPtr.Zero;
            ThrowIfFail(fromConfig(out recoHandlePtr, speechConfig.configHandle, audioConfig.configHandle));
            InteropSafeHandle recoHandle = new InteropSafeHandle(recoHandlePtr, Internal.DialogServiceConnector.dialog_service_connector_handle_release);
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
            InteropSafeHandle recoHandle = new InteropSafeHandle(recoHandlePtr, Internal.DialogServiceConnector.dialog_service_connector_handle_release);
            GC.KeepAlive(speechConfig);
            return recoHandle;
        }


    }
}
