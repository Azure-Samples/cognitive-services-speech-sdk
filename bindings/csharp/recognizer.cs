//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines the base class Recognizer which mainly contains common event handlers.
    /// </summary>
    public class Recognizer : IDisposable
    {
        /// <summary>
        /// Defines event handler for session events, e.g. SessionStarted/Stopped, SpeechStartDetected / StopDetected
        /// </summary>
        /// <example>
        /// Create a speech recognizer, setup an event handler for session events
        /// <code>
        /// static void MySessionEventHandler(object sender, SpeechSessionEventArgs e)
        /// {
        ///    Console.WriteLine(string.Format(CultureInfo.InvariantCulture,"Speech recognition: session event: {0} ", e.ToString()));
        /// }
        ///
        /// static void SpeechRecognizerSample()
        /// {
        ///   SpeechRecognizer reco = factory.CreateSpeechRecognizerWithFileInput("audioFileName");
        ///
        ///   reco.OnSessionEvent += MySessionEventHandler;
        ///
        ///   // Starts recognition.
        ///   var result = await reco.RecognizeAsync();
        ///
        ///   reco.OnSessionEvent -= MySessionEventHandler;
        ///   Console.WriteLine("Speech Recognition: Recognition result: " + result);
        /// }
        /// </code>
        /// </example>
        public event EventHandler<SessionEventArgs> OnSessionEvent;

        public event EventHandler<RecogntionEventArgs> OnSpeechDetectectedEvent;

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
            Dispose(true);
            GC.SuppressFinalize(this);
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

                var arg = new SessionEventArgs(eventType, eventArgs);
                var handler = this.recognizer.OnSessionEvent;

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

                var arg = new RecogntionEventArgs(eventType, eventArgs);
                var handler = this.recognizer.OnSpeechDetectectedEvent;

                if (handler != null)
                {
                    handler(this.recognizer, arg);
                }
            }

            private Recognizer recognizer;
            private RecognitionEventType eventType;
        }
    }
}
