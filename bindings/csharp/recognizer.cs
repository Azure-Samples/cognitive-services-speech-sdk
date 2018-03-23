//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading;
using System.Threading.Tasks;
using Carbon;

namespace Carbon.Recognition
{
    /// <summary>
    /// Defines the base class Recognizer which mainly contains common event handlers.
    /// </summary>
    public class Recognizer
    {
        /// <summary>
        /// Defines event handler for the session start.
        /// </summary>
        public event EventHandler<SessionEventArgs> OnSessionStarted;

        /// <summary>
        /// Defines event handler for the session stop event.
        /// </summary>
        public event EventHandler<SessionEventArgs> OnSessionStopped;

        /// <summary>
        /// Defines event handler for the audio input start event.
        /// </summary>
        public event EventHandler<SessionEventArgs> OnSoundStarted;

        /// <summary>
        /// Defines event handler for the audio stop event.
        /// </summary>
        public event EventHandler<SessionEventArgs> OnSoundStopped;

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
        /// Define event types for internal use only.
        /// </summary>
        internal enum SessionEventType
        {
            SessionStartedEvent,
            SessionStoppedEvent,
            SoundStartedEvent,
            SoundStoppedEvent
        }

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
                var arg = new SessionEventArgs(eventArgs);
                EventHandler<SessionEventArgs> handler;

                switch (eventType)
                {
                    case SessionEventType.SessionStartedEvent:
                        handler = recognizer.OnSessionStarted;
                        break;
                    case SessionEventType.SessionStoppedEvent:
                        handler = recognizer.OnSessionStopped;
                        break;
                    case SessionEventType.SoundStartedEvent:
                        handler = recognizer.OnSoundStarted;
                        break;
                    case SessionEventType.SoundStoppedEvent:
                        handler = recognizer.OnSoundStopped;
                        break;
                    default:
                        throw new InvalidOperationException("Unknown event type");
                }

                if (handler != null)
                {
                    handler(this, arg);
                }
            }

            private Recognizer recognizer;
            private SessionEventType eventType;
        }
    }

    
}
