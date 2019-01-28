//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Threading;
using System.Runtime.InteropServices;

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

        internal Recognizer(Internal.Recognizer recoImpl)
        {
            this.recoImpl = recoImpl;
            gch = GCHandle.Alloc(this, GCHandleType.Normal);
            speechStartDetectedCallbackDelegate = FireEvent_SpeechStartDetected;
            speechEndDetectedCallbackDelegate = FireEvent_SpeechEndDetected;
            sessionStartedCallbackDelegate = FireEvent_SetSessionStarted;
            sessionStoppedCallbackDelegate = FireEvent_SetSessionStopped;

            recoImpl.SetSpeechStartDetectedCallback(speechStartDetectedCallbackDelegate, GCHandle.ToIntPtr(gch));
            recoImpl.SetSpeechEndDetectedCallback(speechEndDetectedCallbackDelegate, GCHandle.ToIntPtr(gch));
            recoImpl.SetSessionStartedCallback(sessionStartedCallbackDelegate, GCHandle.ToIntPtr(gch));
            recoImpl.SetSessionStoppedCallback(sessionStoppedCallbackDelegate, GCHandle.ToIntPtr(gch));
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            try
            {
                lock (recognizerLock)
                {
                    isDisposing = true;
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

            if (disposing)
            {
            }

            if (gch.IsAllocated)
            {
                gch.Free();
            }

            speechStartDetectedCallbackDelegate = null;
            speechEndDetectedCallbackDelegate = null;
            sessionStartedCallbackDelegate = null;
            sessionStoppedCallbackDelegate = null;

            lock (recognizerLock)
            {
                disposed = true;
            }
        }

        internal readonly Internal.Recognizer recoImpl;
        private Internal.CallbackFunctionDelegate speechStartDetectedCallbackDelegate;
        private Internal.CallbackFunctionDelegate speechEndDetectedCallbackDelegate;
        private Internal.CallbackFunctionDelegate sessionStartedCallbackDelegate;
        private Internal.CallbackFunctionDelegate sessionStoppedCallbackDelegate;

        /// <summary>
        /// GC handle for callbacks for context.
        /// </summary>
        protected GCHandle gch;
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
        protected readonly object recognizerLock = new object();
        private int activeAsyncRecognitionCounter = 0;

        /// <summary>
        /// Define a private methods which raise a C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        ///

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_SetSessionStarted(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                EventHandler<SessionEventArgs> eventHandle;
                Recognizer recognizer = (Recognizer)GCHandle.FromIntPtr(pvContext).Target;
                lock (recognizer.recognizerLock)
                {
                    if (recognizer.isDisposing) return;
                    eventHandle = recognizer.SessionStarted;
                }
                var eventArgs = new Internal.SessionEventArgs(hevent);
                var resultEventArg = new SessionEventArgs(eventArgs);
                eventHandle?.Invoke(recognizer, resultEventArg);
                // event do not need to be hold for SessionEventArgs
                eventArgs.Dispose();
            }
            catch (InvalidOperationException)
            {
                Internal.SpxExceptionThrower.LogError(Internal.SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_SetSessionStopped(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                EventHandler<SessionEventArgs> eventHandle;
                Recognizer recognizer = (Recognizer)GCHandle.FromIntPtr(pvContext).Target;
                lock (recognizer.recognizerLock)
                {
                    if (recognizer.isDisposing) return;
                    eventHandle = recognizer.SessionStopped;
                }
                var eventArgs = new Internal.SessionEventArgs(hevent);
                var resultEventArg = new SessionEventArgs(eventArgs);
                eventHandle?.Invoke(recognizer, resultEventArg);
                // event do not need to be hold for SessionEventArgs
                eventArgs.Dispose();
            }
            catch (InvalidOperationException)
            {
                Internal.SpxExceptionThrower.LogError(Internal.SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_SpeechStartDetected(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                EventHandler<RecognitionEventArgs> eventHandle;
                Recognizer recognizer = (Recognizer)GCHandle.FromIntPtr(pvContext).Target;
                lock (recognizer.recognizerLock)
                {
                    if (recognizer.isDisposing) return;
                    eventHandle = recognizer.SpeechStartDetected;
                }
                var eventArgs = new Internal.RecognitionEventArgs(hevent);
                var resultEventArg = new RecognitionEventArgs(eventArgs);
                eventHandle?.Invoke(recognizer, resultEventArg);
                // event do not need to be hold for RecognitionEventArgs
                eventArgs.Dispose();
            }
            catch (InvalidOperationException)
            {
                Internal.SpxExceptionThrower.LogError(Internal.SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_SpeechEndDetected(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                EventHandler<RecognitionEventArgs> eventHandle;
                Recognizer recognizer = (Recognizer)GCHandle.FromIntPtr(pvContext).Target;
                lock (recognizer.recognizerLock)
                {
                    if (recognizer.isDisposing) return;
                    eventHandle = recognizer.SpeechEndDetected;
                }
                var eventArgs = new Internal.RecognitionEventArgs(hevent);
                var resultEventArg = new RecognitionEventArgs(eventArgs);
                eventHandle?.Invoke(recognizer, resultEventArg);
                // event do not need to be hold for RecognitionEventArgs
                eventArgs.Dispose();
            }
            catch (InvalidOperationException)
            {
                Internal.SpxExceptionThrower.LogError(Internal.SpxError.InvalidHandle);
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
    }
}
