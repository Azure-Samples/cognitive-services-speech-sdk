//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Defines the base class Recognizer which mainly contains common event handlers.
 */
public class Recognizer implements Closeable
{
    /*! \cond PROTECTED */

    protected static ExecutorService s_executorService;

    protected AtomicInteger eventCounter = new AtomicInteger(0);
    protected Integer backgroundAttempts = 0;
    /*! \endcond */

    static {
        s_executorService = Executors.newCachedThreadPool();
    }

    /**
     * Defines event handler for session started event.
     */
    final public EventHandlerImpl<SessionEventArgs> sessionStarted = new EventHandlerImpl<SessionEventArgs>(eventCounter);

    /**
     * Defines event handler for session stopped event.
     */
    final public EventHandlerImpl<SessionEventArgs> sessionStopped = new EventHandlerImpl<SessionEventArgs>(eventCounter);

    /**
     * Defines event handler for speech start detected event.
     */
    final public EventHandlerImpl<RecognitionEventArgs> speechStartDetected = new EventHandlerImpl<RecognitionEventArgs>(eventCounter);

    /**
     * Defines event handler for speech end detected event.
     */
    final public EventHandlerImpl<RecognitionEventArgs> speechEndDetected = new EventHandlerImpl<RecognitionEventArgs>(eventCounter);

    /*! \cond PROTECTED */

    /**
     * Creates and initializes an instance of a Recognizer
     * @param audioInput An optional audio input configuration associated with the recognizer
     */
    protected Recognizer(AudioConfig audioInput) {
        recoHandle = new SafeHandle(0, SafeHandleType.Recognizer);
        // Note: Since ais is optional, no test for null reference
        audioInputKeepAlive = audioInput;
    }

    /*! \endcond */

    /**
     * Dispose of associated resources.
     * Note: close() must be called in order to relinquish underlying resources held by the object.
     */
    @Override
    public void close() {
        synchronized (recognizerLock) {
            if (activeAsyncRecognitionCounter != 0) {
                throw new IllegalStateException("Cannot dispose a recognizer while async recognition is running. Await async recognitions to avoid unexpected disposals.");
            }
            dispose(true);
        }
    }

    /*! \cond INTERNAL */
    /**
     * Returns the internal recognizer instance
     * @return The internal recognizer instance
     */
    public SafeHandle getImpl() {
        return recoHandle;
    }
    /*! \endcond */
    
    /*! \cond PROTECTED */
    /**
     * This method performs cleanup of resources.
     * The Boolean parameter disposing indicates whether the method is called from Dispose (if disposing is true) or from the finalizer (if disposing is false).
     * Derived classes should override this method to dispose resource if needed.
     * @param disposing Flag to request disposal.
     */
    protected void dispose(final boolean disposing) {
        if (disposed) {
            return;
        }

        if (disposing) {
            if(this.eventCounter.get() != 0 && backgroundAttempts <= 50 ) {
                // There is an event callback in progress, closing while in an event call results in SWIG problems, so 
                // spin a thread to try again in 500ms and return.
                Thread t = new Thread(
                    new Runnable(){
                        @Override
                        public void run() {
                            try{
                                Thread.sleep(500 * ++backgroundAttempts);
                                dispose(disposing);
                            } catch (Exception e){}
                        }
                    });
                t.start();
            }
            else {
            }
        }
        audioInputKeepAlive = null;
        disposed = true;
    }

    protected void doAsyncRecognitionAction(Runnable recoImplAction) {
        synchronized (recognizerLock) {
            activeAsyncRecognitionCounter++;
        }
        if (disposed) {
            throw new IllegalStateException(this.getClass().getName());
        }
        try {
            recoImplAction.run();
        } finally {
            synchronized (recognizerLock) {
                activeAsyncRecognitionCounter--;
            }
        }
    }

    protected long recognize()
    {
        Contracts.throwIfNull(recoHandle, "Invalid recognizer handle");
        IntRef result = new IntRef(0);
        Contracts.throwIfFail(recognizeOnce(recoHandle, result));
        return result.getValue();
    }

    /*! \endcond */

    private void sessionStartedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            SessionEventArgs arg = new SessionEventArgs(eventArgs, true);
            EventHandlerImpl<SessionEventArgs> handler = this.sessionStarted;
            if (handler != null) {
                handler.fireEvent(this, arg);
            }    
        } catch (Exception e) {}
    }

    private void sessionStoppedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            SessionEventArgs arg = new SessionEventArgs(eventArgs, true);
            EventHandlerImpl<SessionEventArgs> handler = this.sessionStopped;
            if (handler != null) {
                handler.fireEvent(this, arg);
            }    
        } catch (Exception e) {}
    }

    private void speechStartDetectedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            RecognitionEventArgs arg = new RecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<RecognitionEventArgs> handler = this.speechStartDetected;
            if (handler != null) {
                handler.fireEvent(this, arg);
            }    
        } catch (Exception e) {}
    }

    private void speechEndDetectedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            RecognitionEventArgs arg = new RecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<RecognitionEventArgs> handler = this.speechEndDetected;
            if (handler != null) {
                handler.fireEvent(this, arg);
            }    
        } catch (Exception e) {}
    }

    /*! \cond PROTECTED */
    protected SafeHandle recoHandle = null;
    protected final native long startContinuousRecognition(SafeHandle recoHandle);
    protected final native long stopContinuousRecognition(SafeHandle recoHandle);
    protected final native long startKeywordRecognition(SafeHandle recoHandle, SafeHandle keywordModelHandle);
    protected final native long stopKeywordRecognition(SafeHandle recoHandle);
    protected final native long recognizingSetCallback(long recoHandle);
    protected final native long recognizedSetCallback(long recoHandle);
    protected final native long canceledSetCallback(long recoHandle);
    protected final native long sessionStartedSetCallback(long recoHandle);
    protected final native long sessionStoppedSetCallback(long recoHandle);
    protected final native long speechStartDetectedSetCallback(long recoHandle);
    protected final native long speechEndDetectedSetCallback(long recoHandle);
    protected final native long getPropertyBagFromRecognizerHandle(SafeHandle recoHandle, IntRef propertyHandle);
    /*! \endcond */

    private final native long recognizeOnce(SafeHandle recoHandle, IntRef resultHandle);

    private AudioConfig audioInputKeepAlive = null;
    private boolean disposed = false;
    private final Object recognizerLock = new Object();
    private int activeAsyncRecognitionCounter = 0;
}