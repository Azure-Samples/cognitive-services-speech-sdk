//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines the base class Recognizer which mainly contains common event handlers.
    /// </summary>
    public class Recognizer : IDisposable
    {
        /// <summary>
        /// Defines event handler for session started event.
        /// </summary>
        public event EventHandler<SessionEventArgs> SessionStarted;

        /// <summary>
        /// Defines event handler for session stopped event.
        /// </summary>
        public event EventHandler<SessionEventArgs> SessionStopped;

        /// <summary>
        /// Defines event handler for speech start detected event.
        /// </summary>
        public event EventHandler<RecognitionEventArgs> SpeechStartDetected;

        /// <summary>
        /// Defines event handler for speech end detected event.
        /// </summary>
        public event EventHandler<RecognitionEventArgs> SpeechEndDetected;

        internal Recognizer(InteropSafeHandle recoPtr)
        {
            ThrowIfNull(recoPtr);
            recoHandle = recoPtr;

            speechStartDetectedCallbackDelegate = FireEvent_SpeechStartDetected;
            speechEndDetectedCallbackDelegate = FireEvent_SpeechEndDetected;
            sessionStartedCallbackDelegate = FireEvent_SetSessionStarted;
            sessionStoppedCallbackDelegate = FireEvent_SetSessionStopped;

            ThrowIfFail(Internal.Recognizer.recognizer_speech_start_detected_set_callback(recoHandle, speechStartDetectedCallbackDelegate, IntPtr.Zero));
            ThrowIfFail(Internal.Recognizer.recognizer_speech_end_detected_set_callback(recoHandle, speechEndDetectedCallbackDelegate, IntPtr.Zero));
            ThrowIfFail(Internal.Recognizer.recognizer_session_started_set_callback(recoHandle, sessionStartedCallbackDelegate, IntPtr.Zero));
            ThrowIfFail(Internal.Recognizer.recognizer_session_stopped_set_callback(recoHandle, sessionStoppedCallbackDelegate, IntPtr.Zero));
        }

        ~Recognizer()
        {
            isDisposing = true;
            Dispose(false);
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            try
            {
                isDisposing = true;
                lock (recognizerLock)
                {
                    if (activeAsyncRecognitionCounter != 0)
                    {
                        throw new InvalidOperationException("Cannot dispose a recognizer while async recognition is running. Await async recognitions to avoid unexpected disposals.");
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
        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            speechStartDetectedCallbackDelegate = null;
            speechEndDetectedCallbackDelegate = null;
            sessionStartedCallbackDelegate = null;
            sessionStoppedCallbackDelegate = null;

            disposed = true;
        }

        internal InteropSafeHandle recoHandle;
        private IntPtr asyncStartContinuousHandle = IntPtr.Zero;
        private IntPtr asyncStopContinuousHandle = IntPtr.Zero;
        private IntPtr asyncStartKeywordHandle = IntPtr.Zero;
        private IntPtr asyncStopKeywordHandle = IntPtr.Zero;
        private CallbackFunctionDelegate speechStartDetectedCallbackDelegate;
        private CallbackFunctionDelegate speechEndDetectedCallbackDelegate;
        private CallbackFunctionDelegate sessionStartedCallbackDelegate;
        private CallbackFunctionDelegate sessionStoppedCallbackDelegate;

        /// <summary>
        /// disposed is a flag used to indicate if object is disposed.
        /// </summary>
        protected volatile bool disposed = false;
        /// <summary>
        /// isDisposing is a flag used to indicate if object is being disposed.
        /// </summary>
        protected volatile bool isDisposing = false;
        /// <summary>
        /// recognizerLock is used to synchronize access to objects member variables from multiple threads
        /// </summary>
        protected object recognizerLock = new object();
        private int activeAsyncRecognitionCounter = 0;

        /// <summary>
        /// Define a private methods which raise a C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        ///

        [Internal.MonoPInvokeCallback]
        private void FireEvent_SetSessionStarted(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                if (isDisposing)
                {
                    return;
                }
                var resultEventArg = new SessionEventArgs(hevent);
                SessionStarted?.Invoke(this, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private void FireEvent_SetSessionStopped(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                if (isDisposing)
                {
                    return;
                }
                var resultEventArg = new SessionEventArgs(hevent);
                SessionStopped?.Invoke(this, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private void FireEvent_SpeechStartDetected(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                if (isDisposing)
                {
                    return;
                }
                var resultEventArg = new RecognitionEventArgs(hevent);
                SpeechStartDetected?.Invoke(this, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private void FireEvent_SpeechEndDetected(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                if (isDisposing)
                {
                    return;
                }
                var resultEventArg = new RecognitionEventArgs(hevent);
                SpeechEndDetected?.Invoke(this, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }

        /// <summary>
        /// This methods checks if a recognizer is disposed before performing async recognition action.
        /// The Action parameter <paramref name="recoImplAction"/> can be any internal async recognition method of Speech, Translation and Intent Recognizer.
        /// The method is called from all async recognition methods (e.g. <see cref="SpeechRecognizer.StartContinuousRecognitionAsync"/>).
        /// ObjectDisposedException will be thrown and the action will not be performed if its recognizer is not available anymore.
        /// The purpose of this method is to prevent possible race condition if async recognitions are not awaited.
        /// </summary>
        /// <param name="recoImplAction">Actual implementation.</param>
        protected void DoAsyncRecognitionAction(Action recoImplAction)
        {
            lock (recognizerLock)
            {
                activeAsyncRecognitionCounter++;
            }
            if (disposed || isDisposing)
            {
                throw new ObjectDisposedException(this.GetType().Name);
            }
            try
            {
                recoImplAction();
            }
            finally
            {
                lock (recognizerLock)
                {
                    activeAsyncRecognitionCounter--;
                }
            }
        }

        internal IntPtr RecognizeOnce()
        {
            IntPtr hresult = IntPtr.Zero;
            ThrowIfNull(recoHandle, "Invalid recognizer handle");
            ThrowIfFail(Internal.Recognizer.recognizer_recognize_once(recoHandle, out hresult));
            return hresult;
        }

        internal void StartContinuousRecognition()
        {
            if (asyncStartContinuousHandle != IntPtr.Zero)
            {
                ThrowIfFail(Internal.Recognizer.recognizer_async_handle_release(asyncStartContinuousHandle));
            }
            ThrowIfNull(recoHandle, "Invalid recognizer handle");
            ThrowIfFail(Internal.Recognizer.recognizer_start_continuous_recognition_async(recoHandle, out asyncStartContinuousHandle));
            ThrowIfFail(Internal.Recognizer.recognizer_start_continuous_recognition_async_wait_for(asyncStartContinuousHandle, UInt32.MaxValue));
            ThrowIfFail(Internal.Recognizer.recognizer_async_handle_release(asyncStartContinuousHandle));
            asyncStartContinuousHandle = IntPtr.Zero;
        }

        internal void StopContinuousRecognition()
        {
            if (asyncStopContinuousHandle != IntPtr.Zero)
            {
                ThrowIfFail(Internal.Recognizer.recognizer_async_handle_release(asyncStopContinuousHandle));
            }
            ThrowIfNull(recoHandle, "Invalid recognizer handle");
            ThrowIfFail(Internal.Recognizer.recognizer_stop_continuous_recognition_async(recoHandle, out asyncStopContinuousHandle));
            ThrowIfFail(Internal.Recognizer.recognizer_stop_continuous_recognition_async_wait_for(asyncStopContinuousHandle, UInt32.MaxValue));
            ThrowIfFail(Internal.Recognizer.recognizer_async_handle_release(asyncStopContinuousHandle));
            asyncStopContinuousHandle = IntPtr.Zero;
        }

        internal void StartKeywordRecognition(KeywordRecognitionModel model)
        {
            if (asyncStartKeywordHandle != IntPtr.Zero)
            {
                ThrowIfFail(Internal.Recognizer.recognizer_async_handle_release(asyncStartKeywordHandle));
            }
            ThrowIfNull(recoHandle, "Invalid recognizer handle");
            ThrowIfFail(Internal.Recognizer.recognizer_start_keyword_recognition_async(recoHandle, model.keywordHandle, out asyncStartKeywordHandle));
            GC.KeepAlive(model);
            ThrowIfFail(Internal.Recognizer.recognizer_start_keyword_recognition_async_wait_for(asyncStartKeywordHandle, UInt32.MaxValue));
            ThrowIfFail(Internal.Recognizer.recognizer_async_handle_release(asyncStartKeywordHandle));
            asyncStartKeywordHandle = IntPtr.Zero;
        }

        internal void StopKeywordRecognition()
        {
            if (asyncStopKeywordHandle != IntPtr.Zero)
            {
                ThrowIfFail(Internal.Recognizer.recognizer_async_handle_release(asyncStopKeywordHandle));
            }
            ThrowIfNull(recoHandle, "Invalid recognizer handle");
            ThrowIfFail(Internal.Recognizer.recognizer_stop_keyword_recognition_async(recoHandle, out asyncStopKeywordHandle));
            ThrowIfFail(Internal.Recognizer.recognizer_stop_keyword_recognition_async_wait_for(asyncStopKeywordHandle, UInt32.MaxValue));
            ThrowIfFail(Internal.Recognizer.recognizer_async_handle_release(asyncStopKeywordHandle));
            asyncStopKeywordHandle = IntPtr.Zero;
        }

        internal delegate IntPtr GetRecognizerFromConfigDelegate(out IntPtr phreco, InteropSafeHandle speechconfig, InteropSafeHandle audioInput);

        internal static InteropSafeHandle FromConfig(GetRecognizerFromConfigDelegate fromConfig, SpeechConfig speechConfig, Audio.AudioConfig audioConfig)
        {
            if (speechConfig == null) throw new ArgumentNullException(nameof(speechConfig));
            if (audioConfig == null) throw new ArgumentNullException(nameof(audioConfig));

            IntPtr recoHandlePtr = IntPtr.Zero;
            ThrowIfFail(fromConfig(out recoHandlePtr, speechConfig.configHandle, audioConfig.configImpl.configHandle));
            InteropSafeHandle recoHandle = new InteropSafeHandle(recoHandlePtr, Internal.Recognizer.recognizer_handle_release);
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
            InteropSafeHandle recoHandle = new InteropSafeHandle(recoHandlePtr, Internal.Recognizer.recognizer_handle_release);
            GC.KeepAlive(speechConfig);
            return recoHandle;
        }
    }
}
