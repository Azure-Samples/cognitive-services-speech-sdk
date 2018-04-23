package com.microsoft.cognitiveservices.speech.recognition;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import java.io.IOException;

import com.microsoft.cognitiveservices.speech.SessionEventArgs;
import com.microsoft.cognitiveservices.speech.SessionEventType;
import com.microsoft.cognitiveservices.speech.util.EventHandler;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;

/**
  * Defines the base class Recognizer which mainly contains common event handlers.
  */
public class Recognizer implements Closeable
{
    /**
      * Defines event handler for session events, e.g. sessionStarted/Stopped, speechStartDetected / stopDetected
      * 
      * Create a speech recognizer, setup an event handler for session events
      */
    final public EventHandlerImpl<SessionEventArgs> SessionEvent = new EventHandlerImpl<SessionEventArgs>();

    protected Recognizer()
    {
        sessionStartedHandler = new SessionEventHandlerImpl(this, SessionEventType.SessionStartedEvent);
        sessionStoppedHandler = new SessionEventHandlerImpl(this, SessionEventType.SessionStoppedEvent);
        speechStartDetectedHandler = new SessionEventHandlerImpl(this, SessionEventType.SpeechStartDetectedEvent);
        speechEndDetectedHandler = new SessionEventHandlerImpl(this, SessionEventType.SpeechEndDetectedEvent);
    }

    /**
      * Dispose of associated resources.
      */
    public void close()
    {
        try {
            dispose(true);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
      * This method performs cleanup of resources.
      * The Boolean parameter disposing indicates whether the method is called from Dispose (if disposing is true) or from the finalizer (if disposing is false).
      * Derived classes should override this method to dispose resource if needed.
      * @param disposing Flag to request disposal.
      * @throws java.io.IOException is native resource could not be disposed.
      */
    protected void dispose(boolean disposing) throws IOException
    {
        if (disposed)
        {
            return;
        }

        if (disposing)
        {
            // disconnect
            sessionStartedHandler.delete();
            sessionStoppedHandler.delete();
            speechStartDetectedHandler.delete();
            speechEndDetectedHandler.delete();
        }

        disposed = true;
    }

    protected SessionEventHandlerImpl sessionStartedHandler;
    protected SessionEventHandlerImpl sessionStoppedHandler;
    protected SessionEventHandlerImpl speechStartDetectedHandler;
    protected SessionEventHandlerImpl speechEndDetectedHandler;
    private boolean disposed = false;

    /**
      * Define an internal class which raise an event when a corresponding callback is invoked from the native layer. 
      */
    class SessionEventHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SessionEventListener
    {
        public SessionEventHandlerImpl(Recognizer recognizer, SessionEventType eventType)
        {
            this.recognizer = recognizer;
            this.eventType = eventType;
        }

        @Override
        public void execute(com.microsoft.cognitiveservices.speech.internal.SessionEventArgs eventArgs)
        {
            if (recognizer.disposed)
            {
                return;
            }

            SessionEventArgs arg = new SessionEventArgs(eventType, eventArgs);
            EventHandlerImpl<SessionEventArgs>  handler = this.recognizer.SessionEvent;

            if (handler != null)
            {
                handler.fireEvent(this.recognizer, arg);
            }
        }

        private Recognizer recognizer;
        private SessionEventType eventType;
        
    }
}
