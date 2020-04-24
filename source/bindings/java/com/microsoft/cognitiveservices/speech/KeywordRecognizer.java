//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.util.concurrent.Future;
import java.io.Closeable;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionCanceledEventArgs;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;

/**
 * Performs keyword recognition on the speech input.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 */
public final class KeywordRecognizer implements Closeable {

    /*! \cond PROTECTED */

    protected AtomicInteger eventCounter = new AtomicInteger(0);
    protected Integer backgroundAttempts = 0;

    /*! \endcond */

    /**
     * The signal for events related to the recognition of keywords
     */
    final public EventHandlerImpl<KeywordRecognitionEventArgs> recognized = new EventHandlerImpl<KeywordRecognitionEventArgs>(eventCounter);

    /**
     * The signal for events relating to the cancellation of an interaction. The event indicates if the reason is a direct cancellation or an error.
     */
    final public EventHandlerImpl<SpeechRecognitionCanceledEventArgs> canceled = new EventHandlerImpl<SpeechRecognitionCanceledEventArgs>(eventCounter);

    /**
     * Creates a new instance of a keyword recognizer. If no audio config is provided as input parameter, it will be equivalent to calling with a config 
     * constructed with AudioConfig::FromDefaultMicrophoneInput.
     * @param audioConfig audio configuration.
     * @return a new instance of a keyword recognizer.
     */
    public KeywordRecognizer(com.microsoft.cognitiveservices.speech.audio.AudioConfig audioConfig) {

        this.recoHandle = new SafeHandle(0, SafeHandleType.KeywordRecognizer);
        Contracts.throwIfFail(createKeywordRecognizerFromConfig(recoHandle, audioConfig.getImpl()));
        audioInputKeepAlive = audioConfig;
        initialize();
    }


    /**
     * The collection of properties and their values defined for KeywordRecognizer.
     * @return The collection of properties and their values defined for KeywordRecognizer.
     */
    public PropertyCollection getProperties() {
        return propertyHandle;
    }

    /**
     * Starts a keyword recognition session. This session will last until the first keyword is recognized. When this happens,
     * Recognized event will be raised and the session will end. To rearm the keyword, the method needs to be called
     * again after the event is emitted.
     * Note: If no keyword is detected in the input, the task will never resolve unless StopRecognition method is called.
     * @param model The keyword recognition model that specifies the keyword to be recognized.
     * @return A task representing the keyword recognition operation. The task returns a value of KeywordRecognitionResult.
     */
    public Future<KeywordRecognitionResult> recognizeOnceAsync(final KeywordRecognitionModel model) {

        return executorService.submit(new java.util.concurrent.Callable<KeywordRecognitionResult>() {
            public KeywordRecognitionResult  call() {
                IntRef result = new IntRef(0);
                Contracts.throwIfFail(recognizeOnce(recoHandle, model.getImpl(), result));
                return new KeywordRecognitionResult(result.getValue());
            }
        });
    }

    /**
     * Stops a currently active keyword recognition session.
     * @return A task representing the asynchronous operation that stops the recognition.
     */
    public Future<Void> stopRecognitionAsync() {
        final KeywordRecognizer thisReco = this;

        return executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { thisReco.stopRecognition(recoHandle); }};
                return null;
        }});
    }
    
    /**
     * Dispose of associated resources.
     * Note: close() must be called in order to relinquish underlying resources held by the object.
     */
    @Override
    public void close() {
        dispose(true);
    }

    /*! \cond PROTECTED */

    private void dispose(final boolean disposing) {
        if (disposed) {
            return;
        }

        if (disposing) {
            if(this.eventCounter.get() != 0 && backgroundAttempts <= 50 ) {
                // There is an event callback in progress, closing while in an event call results in SWIG problems, so 
                // spin a thread to try again in 500ms and return.
                getProperties().getProperty("Backgrounding release " + backgroundAttempts.toString() + " " + this.eventCounter.get(), ""); 
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
                // Trigger graceful shutdown of executor service
                executorService.shutdown();

                if (propertyHandle != null) {
                    propertyHandle.close();
                    propertyHandle = null;
                }
                if (recoHandle != null) {
                    recoHandle.close();
                    recoHandle = null;
                }
                // Deref audioInputKeepAlive by setting to null, this will trigger finalizer if only reference.
                audioInputKeepAlive = null;
                keywordRecognizerObjects.remove(this);
                
                // If the executor service has not been shut down, force shut down
                if (!executorService.isShutdown()) {
                    executorService.shutdownNow();
                }
                disposed = true;
            }
        }
    }

    /*! \endcond */

    /*! \cond INTERNAL */

    /**
     * This is used to keep any instance of this class alive that is subscribed to downstream events.
     */
    static java.util.Set<KeywordRecognizer> keywordRecognizerObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<KeywordRecognizer>());

    // TODO Remove this... After tests are updated to no longer depend upon this
    public SafeHandle getRecoImpl() {
        return recoHandle;
    }

    /*! \endcond */

    private void initialize() {
        final KeywordRecognizer _this = this;

        executorService = Executors.newCachedThreadPool();

        this.recognized.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                keywordRecognizerObjects.add(_this);
                Contracts.throwIfFail(recognizedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.canceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                keywordRecognizerObjects.add(_this);
                Contracts.throwIfFail(canceledSetCallback(_this.recoHandle.getValue()));                
            }
        });

        IntRef propHandle = new IntRef(0);
        Contracts.throwIfFail(getPropertyBagFromRecognizerHandle(_this.recoHandle, propHandle));
        propertyHandle = new PropertyCollection(propHandle);
    }

    private void recognizedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            KeywordRecognitionEventArgs resultEventArg = new KeywordRecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<KeywordRecognitionEventArgs> handler = this.recognized;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private void canceledEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            SpeechRecognitionCanceledEventArgs resultEventArg = new SpeechRecognitionCanceledEventArgs(eventArgs, true);
            EventHandlerImpl<SpeechRecognitionCanceledEventArgs> handler = this.canceled;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private final native long createKeywordRecognizerFromConfig(SafeHandle recoHandle, SafeHandle audioConfigHandle);
    private final native long recognizeOnce(SafeHandle recoHandle, SafeHandle keywordModel, IntRef resultHandle);
    private final native long stopRecognition(SafeHandle recoHandle);
    private final native long recognizedSetCallback(long recoHandle);
    private final native long canceledSetCallback(long recoHandle);
    private final native long getPropertyBagFromRecognizerHandle(SafeHandle recoHandle, IntRef propertyHandle);

    private ExecutorService executorService;
    private SafeHandle recoHandle = null;
    private AudioConfig audioInputKeepAlive = null;
    private PropertyCollection propertyHandle = null;
    private boolean disposed = false;
}
