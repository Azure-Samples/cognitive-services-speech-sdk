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

        /// <summary>
        /// Defines event handler for session events, e.g., SpeechStartDetectedEvent and SpeechEndDetectedEvent.
        /// </summary>
        public event EventHandler<RecognitionEventArgs> OnSpeechDetectectedEvent;

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
                var handler = this.recognizer.OnSpeechDetectectedEvent;

                if (handler != null)
                {
                    handler(this.recognizer, arg);
                }
            }

            private Recognizer recognizer;
            private RecognitionEventType eventType;
        }

        internal class RecognizerParametersImpl : IRecognizerParameters
        {
            private Internal.RecognizerParameterValueCollection recognizerParameterImpl;

            public RecognizerParametersImpl(Internal.RecognizerParameterValueCollection internalRecognizerParameters)
            {
                recognizerParameterImpl = internalRecognizerParameters;
            }

            public bool Is<T>(RecognizerParameterKind propertyKind)
            {
                if (typeof(T) == typeof(string))
                {
                    return recognizerParameterImpl.Get((Internal.RecognizerParameter)propertyKind).IsString();
                }
                else if (typeof(T) == typeof(int))
                {
                    return recognizerParameterImpl.Get((Internal.RecognizerParameter)propertyKind).IsNumber();
                }
                else if (typeof(T) == typeof(bool))
                {
                    return recognizerParameterImpl.Get((Internal.RecognizerParameter)propertyKind).IsBool();
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public bool Is<T>(string propertyName)
            {
                if (typeof(T) == typeof(string))
                {
                    return recognizerParameterImpl.Get(propertyName).IsString();
                }
                else if (typeof(T) == typeof(int))
                {
                    return recognizerParameterImpl.Get(propertyName).IsNumber();
                }
                else if (typeof(T) == typeof(bool))
                {
                    return recognizerParameterImpl.Get(propertyName).IsBool();
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public T Get<T>(RecognizerParameterKind propertyKind)
            {
                T defaultT;
                if (typeof(T) == typeof(string))
                {
                    defaultT = (T)Convert.ChangeType(string.Empty, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    defaultT = (T)Convert.ChangeType(0, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    defaultT = (T)Convert.ChangeType(false, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }

                return Get<T>(propertyKind, defaultT);
            }

            public T Get<T>(string propertyName)
            {
                T defaultT;
                if (typeof(T) == typeof(string))
                {
                    defaultT = (T)Convert.ChangeType(string.Empty, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    defaultT = (T)Convert.ChangeType(0, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    defaultT = (T)Convert.ChangeType(false, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }

                return Get<T>(propertyName, defaultT);
            }

            public T Get<T>(RecognizerParameterKind propertyKind, T defaultValue)
            {
                if (typeof(T) == typeof(string))
                {
                    var defaultInT = (string)Convert.ChangeType(defaultValue, typeof(string), CultureInfo.InvariantCulture);
                    var ret = recognizerParameterImpl.Get((Internal.RecognizerParameter)propertyKind).GetString(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    var defaultInT = (int)Convert.ChangeType(defaultValue, typeof(int), CultureInfo.InvariantCulture);
                    var ret = recognizerParameterImpl.Get((Internal.RecognizerParameter)propertyKind).GetNumber(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    var defaultInT = (bool)Convert.ChangeType(defaultValue, typeof(bool), CultureInfo.InvariantCulture);
                    var ret = recognizerParameterImpl.Get((Internal.RecognizerParameter)propertyKind).GetBool(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public T Get<T>(string propertyName, T defaultValue)
            {
                if (typeof(T) == typeof(string))
                {
                    var defaultInT = (string)Convert.ChangeType(defaultValue, typeof(string), CultureInfo.InvariantCulture);
                    var ret = recognizerParameterImpl.Get(propertyName).GetString(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    var defaultInT = (int)Convert.ChangeType(defaultValue, typeof(int), CultureInfo.InvariantCulture);
                    var ret = recognizerParameterImpl.Get(propertyName).GetNumber(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    var defaultInT = (bool)Convert.ChangeType(defaultValue, typeof(bool), CultureInfo.InvariantCulture);
                    var ret = recognizerParameterImpl.Get(propertyName).GetBool(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public void Set(RecognizerParameterKind propertyKind, string value)
            {
                recognizerParameterImpl.Get((Internal.RecognizerParameter)propertyKind).SetString(value);
            }

            public void Set(string propertyName, string value)
            {
                recognizerParameterImpl.Get(propertyName).SetString(value);
            }

            public void Set(RecognizerParameterKind propertyKind, int value)
            {
                recognizerParameterImpl.Get((Internal.RecognizerParameter)propertyKind).SetNumber(value);
            }

            public void Set(string propertyName, int value)
            {
                recognizerParameterImpl.Get(propertyName).SetNumber(value);
            }

            public void Set(RecognizerParameterKind propertyKind, bool value)
            {
                recognizerParameterImpl.Get((Internal.RecognizerParameter)propertyKind).SetBool(value);
            }

            public void Set(string propertyName, bool value)
            {
                recognizerParameterImpl.Get(propertyName).SetBool(value);
            }
        }
    }
}
