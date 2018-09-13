//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines the base class Recognizer which mainly contains common event handlers.
    /// </summary>
    public class Recognizer : IDisposable
    {
        /// <summary>
        /// Defines event handler for session events, e.g., SessionStartedEvent and SessionStoppedEvent.
        /// </summary>
        public event EventHandler<SessionEventArgs> OnSessionEvent;

        /// <summary>
        /// Defines event handler for session events, e.g., SpeechStartDetectedEvent and SpeechEndDetectedEvent.
        /// </summary>
        public event EventHandler<RecognitionEventArgs> OnSpeechDetectedEvent;

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

                var arg = new RecognitionEventArgs(eventType, eventArgs);
                var handler = this.recognizer.OnSpeechDetectedEvent;

                if (handler != null)
                {
                    handler(this.recognizer, arg);
                }
            }

            private Recognizer recognizer;
            private RecognitionEventType eventType;
        }

        // TODO: Evil code duplication, merge with another implmentation.
        internal class PropertyCollectionImpl : IPropertyCollection
        {
            private Internal.RecognizerPropertyCollection recognizerParameterImpl;

            public PropertyCollectionImpl(Internal.RecognizerPropertyCollection internalRecognizerParameters)
            {
                recognizerParameterImpl = internalRecognizerParameters;
            }
            
            public string Get(SpeechPropertyId id)
            {
                return Get(id, string.Empty);
            }

            public string Get(string propertyName)
            {
                return Get(propertyName, string.Empty);
            }

            public string Get(SpeechPropertyId id, string defaultValue)
            {
                return recognizerParameterImpl.GetProperty((Internal.SpeechPropertyId)id, defaultValue);
            }

            public string Get(string propertyName, string defaultValue)
            {
                return recognizerParameterImpl.GetProperty(propertyName, defaultValue);
            }

            public void Set(SpeechPropertyId id, string value)
            {
                recognizerParameterImpl.SetProperty((Internal.SpeechPropertyId)id, value);
            }

            public void Set(string propertyName, string value)
            {
                recognizerParameterImpl.SetProperty(propertyName, value);
            }
        }
    }
}
