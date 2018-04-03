//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using Carbon;

namespace Carbon.Recognition
{
    /// <summary>
    /// Defines the base class Recognizer which mainly contains common event handlers.
    /// </summary>
    public class Recognizer
    {
        /// <summary>
        /// Defines event handler for session events, e.g. SessionStarted/Stopped, SoundStarted/Stopped.
        /// </summary>
        /// <example>
        /// Create a speech recognizer, setup an event handler for session events
        /// <code>
        /// static void MySessionEventHandler(object sender, SpeechSessionEventArgs e)
        /// {
        ///    Console.WriteLine(String.Format("Speech recogniton: session event: {0} ", e.ToString()));
        /// }
        ///
        /// static void SpeechRecognizerSample()
        /// {
        ///   SpeechRecognizer reco = factory.CreateSpeechRecognizer("audioFileName");
        ///
        ///   reco.OnSessionEvent += MySessionEventHandler;
        ///
        ///   // Starts recognition.
        ///   var result = await reco.RecognizeAsync();
        ///
        ///   reco.OnSessionEvent -= MySessionEventHandler;
        ///  
        ///   Console.WriteLine("Speech Recognition: Recognition result: " + result);
        /// }
        /// </code>
        /// </example>
        public event EventHandler<SessionEventArgs> OnSessionEvent;

        internal Recognizer()
        {
            sessionStartedHandler = new SessionEventHandlerImpl(this, SessionEventType.SessionStartedEvent);
            sessionStoppedHandler = new SessionEventHandlerImpl(this, SessionEventType.SessionStoppedEvent);
            soundStartedHandler = new SessionEventHandlerImpl(this, SessionEventType.SoundStartedEvent);
            soundStoppedHandler = new SessionEventHandlerImpl(this, SessionEventType.SessionStoppedEvent);
        }

        ~Recognizer()
        {
        }

        internal SessionEventHandlerImpl sessionStartedHandler;
        internal SessionEventHandlerImpl sessionStoppedHandler;
        internal SessionEventHandlerImpl soundStartedHandler;
        internal SessionEventHandlerImpl soundStoppedHandler;

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
    }

    
}
