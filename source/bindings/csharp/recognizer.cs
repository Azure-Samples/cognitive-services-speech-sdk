//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Threading;

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

        internal Recognizer()
        {
            sessionStartedHandler = new SessionEventHandlerImpl(this, SessionEventType.SessionStartedEvent);
            sessionStoppedHandler = new SessionEventHandlerImpl(this, SessionEventType.SessionStoppedEvent);
            speechStartDetectedHandler = new RecognitionEventHandlerImpl(this, RecognitionEventType.SpeechStartDetectedEvent);
            speechEndDetectedHandler = new RecognitionEventHandlerImpl(this, RecognitionEventType.SpeechEndDetectedEvent);
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            lock (recognizerLock)
            {
                if (activeAsyncRecognitionCounter != 0)
                {
                    throw new InvalidOperationException("Cannot dispose a recognizer while async recognition is running. Await async recognitions to avoid unexpected disposals.");
                }
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
                // disconnect
                sessionStartedHandler?.Dispose();
                sessionStoppedHandler?.Dispose();
                speechStartDetectedHandler?.Dispose();
                speechEndDetectedHandler?.Dispose();
            }

            disposed = true;
        }

        internal SessionEventHandlerImpl sessionStartedHandler;
        internal SessionEventHandlerImpl sessionStoppedHandler;
        internal RecognitionEventHandlerImpl speechStartDetectedHandler;
        internal RecognitionEventHandlerImpl speechEndDetectedHandler;
        private bool disposed = false;
        private readonly object recognizerLock = new object();
        private int activeAsyncRecognitionCounter = 0;

        /// <summary>
        /// Define an internal class which raise a C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        internal class SessionEventHandlerImpl : Internal.SessionEventListener
        {
            public SessionEventHandlerImpl(Recognizer recognizer, SessionEventType eventType)
            {
                this.recognizer = recognizer;
                this.eventType = eventType;
            }

            public override void Execute(Internal.SessionEventArgs eventArgs)
            {
                if (recognizer.disposed)
                {
                    return;
                }

                var arg = new SessionEventArgs(eventArgs);

                var handler = eventType == SessionEventType.SessionStartedEvent
                    ? this.recognizer.SessionStarted
                    : this.recognizer.SessionStopped;

                if (handler != null)
                {
                    handler(this.recognizer, arg);
                }
            }

            private Recognizer recognizer;
            private SessionEventType eventType;
        }

        /// <summary>
        /// Define an internal class which raises a C# event when a corresponding callback is invoked from the native layer.
        /// </summary>
        internal class RecognitionEventHandlerImpl : Internal.RecognitionEventListener
        {
            public RecognitionEventHandlerImpl(Recognizer recognizer, RecognitionEventType eventType)
            {
                this.recognizer = recognizer;
                this.eventType = eventType;
            }

            public override void Execute(Internal.RecognitionEventArgs eventArgs)
            {
                if (recognizer.disposed)
                {
                    return;
                }

                var arg = new RecognitionEventArgs(eventArgs);
                var handler = eventType == RecognitionEventType.SpeechStartDetectedEvent
                    ? this.recognizer.SpeechStartDetected
                    : this.recognizer.SpeechEndDetected;

                if (handler != null)
                {
                    handler(this.recognizer, arg);
                }
            }

            private Recognizer recognizer;
            private RecognitionEventType eventType;
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
            if (disposed)
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
