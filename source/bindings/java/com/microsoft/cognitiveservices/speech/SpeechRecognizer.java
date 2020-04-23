//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
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
 * Performs speech recognition from microphone, file, or other audio input streams, and gets transcribed text as result.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 *
 */
public final class SpeechRecognizer extends com.microsoft.cognitiveservices.speech.Recognizer {
    /**
     * The event recognizing signals that an intermediate recognition result is received.
     */
    final public EventHandlerImpl<SpeechRecognitionEventArgs> recognizing = new EventHandlerImpl<SpeechRecognitionEventArgs>(eventCounter);

    /**
     * The event recognized signals that a final recognition result is received.
     */
    final public EventHandlerImpl<SpeechRecognitionEventArgs> recognized = new EventHandlerImpl<SpeechRecognitionEventArgs>(eventCounter);

    /**
     * The event canceled signals that the recognition was canceled.
     */
    final public EventHandlerImpl<SpeechRecognitionCanceledEventArgs> canceled = new EventHandlerImpl<SpeechRecognitionCanceledEventArgs>(eventCounter);

    /**
     * Initializes a new instance of Speech Recognizer.
     * @param speechConfig speech configuration.
     */
    public SpeechRecognizer(SpeechConfig speechConfig) {
        super(null);
        Contracts.throwIfNull(speechConfig, "speechConfig");
        Contracts.throwIfNull(recoHandle, "recoHandle");
        Contracts.throwIfFail(createSpeechRecognizerFromConfig(recoHandle, speechConfig.getImpl(), null));
        initialize();
    }

    /**
     * Initializes a new instance of Speech Recognizer.
     * @param speechConfig speech configuration.
     * @param audioConfig audio configuration.
     */
    public SpeechRecognizer(SpeechConfig speechConfig, AudioConfig audioConfig) {
        super(audioConfig);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        if (audioConfig == null) {
            Contracts.throwIfFail(createSpeechRecognizerFromConfig(recoHandle, speechConfig.getImpl(), null));
        } else {
            Contracts.throwIfFail(createSpeechRecognizerFromConfig(recoHandle, speechConfig.getImpl(), audioConfig.getImpl()));
        }
        initialize();
    }

    /**
     * Initializes a new instance of Speech Recognizer.
     * @param speechConfig speech configuration.
     * @param autoDetectSourceLangConfig the configuration for auto detecting source language
     */
    public SpeechRecognizer(SpeechConfig speechConfig, AutoDetectSourceLanguageConfig autoDetectSourceLangConfig) {
        super(null);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        Contracts.throwIfNull(autoDetectSourceLangConfig, "autoDetectSourceLangConfig");
        Contracts.throwIfFail(createSpeechRecognizerFromAutoDetectSourceLangConfig(super.recoHandle, speechConfig.getImpl(), autoDetectSourceLangConfig.getImpl(), null));
        initialize();
    }

    /**
     * Initializes a new instance of Speech Recognizer.
     * @param speechConfig speech configuration.
     * @param autoDetectSourceLangConfig the configuration for auto detecting source language
     * @param audioConfig audio configuration.
     */
    public SpeechRecognizer(SpeechConfig speechConfig, AutoDetectSourceLanguageConfig autoDetectSourceLangConfig, AudioConfig audioConfig) {
        super(audioConfig);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        Contracts.throwIfNull(autoDetectSourceLangConfig, "autoDetectSourceLangConfig");
        if (audioConfig == null) {
            Contracts.throwIfFail(createSpeechRecognizerFromAutoDetectSourceLangConfig(super.recoHandle, speechConfig.getImpl(), autoDetectSourceLangConfig.getImpl(), null));
        } else {
            Contracts.throwIfFail(createSpeechRecognizerFromAutoDetectSourceLangConfig(super.recoHandle, speechConfig.getImpl(), autoDetectSourceLangConfig.getImpl(), audioConfig.getImpl()));
        }
        initialize();
    }

    /**
     * Initializes a new instance of Speech Recognizer.
     * @param speechConfig speech configuration.
     * @param sourceLanguageConfig the configuration for source language
     */
    public SpeechRecognizer(SpeechConfig speechConfig, SourceLanguageConfig sourceLanguageConfig) {
        super(null);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        Contracts.throwIfNull(sourceLanguageConfig, "sourceLanguageConfig");
        Contracts.throwIfFail(createSpeechRecognizerFromSourceLangConfig(super.recoHandle, speechConfig.getImpl(), sourceLanguageConfig.getImpl(), null));
        initialize();
    }

    /**
     * Initializes a new instance of Speech Recognizer.
     * @param speechConfig speech configuration.
     * @param sourceLanguageConfig the configuration for source language
     * @param audioConfig audio configuration.
     */
    public SpeechRecognizer(SpeechConfig speechConfig, SourceLanguageConfig sourceLanguageConfig, AudioConfig audioConfig) {
        super(audioConfig);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        Contracts.throwIfNull(sourceLanguageConfig, "sourceLanguageConfig");
        if (audioConfig == null) {
            Contracts.throwIfFail(createSpeechRecognizerFromSourceLangConfig(super.recoHandle, speechConfig.getImpl(), sourceLanguageConfig.getImpl(), null));
        } else {
            Contracts.throwIfFail(createSpeechRecognizerFromSourceLangConfig(super.recoHandle, speechConfig.getImpl(), sourceLanguageConfig.getImpl(), audioConfig.getImpl()));
        }
        initialize();
    }

    /**
     * Initializes a new instance of Speech Recognizer.
     * @param speechConfig speech configuration.
     * @param sourceLanguage the recognition source language
     */
    public SpeechRecognizer(SpeechConfig speechConfig, String sourceLanguage) {
        super(null);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        Contracts.throwIfIllegalLanguage(sourceLanguage, "invalid language value");
        Contracts.throwIfFail(createSpeechRecognizerFromSourceLangConfig(super.recoHandle, speechConfig.getImpl(), SourceLanguageConfig.fromLanguage(sourceLanguage).getImpl(), null));
        initialize();
    }

    /**
     * Initializes a new instance of Speech Recognizer.
     * @param speechConfig speech configuration.
     * @param sourceLanguage the recognition source language
     * @param audioConfig audio configuration.
     */
    public SpeechRecognizer(SpeechConfig speechConfig, String sourceLanguage, AudioConfig audioConfig) {
        super(audioConfig);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        Contracts.throwIfIllegalLanguage(sourceLanguage, "invalid language value");
        if (audioConfig == null) {
            Contracts.throwIfFail(createSpeechRecognizerFromSourceLangConfig(super.recoHandle, speechConfig.getImpl(), SourceLanguageConfig.fromLanguage(sourceLanguage).getImpl(), null));
        } else {
            Contracts.throwIfFail(createSpeechRecognizerFromSourceLangConfig(super.recoHandle, speechConfig.getImpl(), SourceLanguageConfig.fromLanguage(sourceLanguage).getImpl(), audioConfig.getImpl()));
        }
        initialize();
    }

    /**
     * Gets the endpoint ID of a customized speech model that is used for speech recognition.
     * @return the endpoint ID of a customized speech model that is used for speech recognition.
     */
    public String getEndpointId() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_EndpointId);
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires,
     * the caller needs to refresh it by calling this setter with a new valid token.
     * Otherwise, the recognizer will encounter errors during recognition.
     * @param token Authorization token.
     */
    public void setAuthorizationToken(String token) {
        Contracts.throwIfNullOrWhitespace(token, "token");
        propertyHandle.setProperty(PropertyId.SpeechServiceAuthorization_Token, token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public String getAuthorizationToken() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceAuthorization_Token);
    }

    /**
     * Gets the spoken language of recognition.
     * @return The spoken language of recognition.
     */
    public String getSpeechRecognitionLanguage() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets the output format of recognition.
     * @return The output format of recognition.
     */
    public OutputFormat getOutputFormat() {
        if (propertyHandle.getProperty(PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse).equals("true")) {
            return OutputFormat.Detailed;
        } else {
            return OutputFormat.Simple;
        }
    }

    /**
     * The collection of properties and their values defined for this SpeechRecognizer.
     * @return The collection of properties and their values defined for this SpeechRecognizer.
     */
    public PropertyCollection getProperties() {
        return propertyHandle;
    }

    /**
     * Starts speech recognition, and returns after a single utterance is recognized. The end of a
     * single utterance is determined by listening for silence at the end or until a maximum of 15
     * seconds of audio is processed.  The task returns the recognition text as result.
     * Note: Since recognizeOnceAsync() returns only a single utterance, it is suitable only for single
     * shot recognition like command or query.
     * For long-running multi-utterance recognition, use startContinuousRecognitionAsync() instead.
     * @return A task representing the recognition operation. The task returns a value of SpeechRecognitionResult
     */
    public Future<SpeechRecognitionResult> recognizeOnceAsync() {
        final SpeechRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<SpeechRecognitionResult>() {
            public SpeechRecognitionResult  call() {
                // A variable defined in an enclosing scope must be final or effectively final.
                // The compiler treats an array initialized once as an effectively final.
                final SpeechRecognitionResult[] result = new SpeechRecognitionResult[1];

                Runnable runnable = new Runnable() { public void run() { result[0] = new SpeechRecognitionResult(thisReco.recognize()); }};
                thisReco.doAsyncRecognitionAction(runnable);

                return result[0];
            }});
    }

    /**
     * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public Future<Void> startContinuousRecognitionAsync() {
        final SpeechRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { thisReco.startContinuousRecognition(recoHandle); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /**
     * Stops continuous speech recognition.
     * @return A task representing the asynchronous operation that stops the recognition.
     */
    public Future<Void> stopContinuousRecognitionAsync() {
        final SpeechRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { thisReco.stopContinuousRecognition(recoHandle); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /**
     * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Keyword spotting (KWS) functionality might work with any microphone type, official KWS support, however, is currently limited to the microphone arrays found in the Azure Kinect DK hardware or the Speech Devices SDK.
     * @param model The keyword recognition model that specifies the keyword to be recognized.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public Future<Void> startKeywordRecognitionAsync(KeywordRecognitionModel model) {
        Contracts.throwIfNull(model, "model");

        final SpeechRecognizer thisReco = this;
        final KeywordRecognitionModel model2 = model;
        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { thisReco.startKeywordRecognition(recoHandle, model2.getImpl()); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /**
     * Stops continuous speech recognition.
     * Note: Keyword spotting (KWS) functionality might work with any microphone type, official KWS support, however, is currently limited to the microphone arrays found in the Azure Kinect DK hardware or the Speech Devices SDK.
     * @return A task representing the asynchronous operation that stops the recognition.
     */
    public Future<Void> stopKeywordRecognitionAsync() {
        final SpeechRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { thisReco.stopKeywordRecognition(recoHandle); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /*! \cond PROTECTED */

    @Override
    protected void dispose(final boolean disposing)
    {
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
                if (propertyHandle != null)
                {
                    propertyHandle.close();
                    propertyHandle = null;
                }
                if (recoHandle != null)
                {
                    recoHandle.close();
                    recoHandle = null;
                }
                speechRecognizerObjects.remove(this);
                disposed = true;
                super.dispose(disposing);                
            }    
        }
    }

    /*! \endcond */

    /*! \cond INTERNAL */

    /**
     * This is used to keep any instance of this class alive that is subscribed to downstream events.
     */
    static java.util.Set<SpeechRecognizer> speechRecognizerObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<SpeechRecognizer>());

    // TODO Remove this... After tests are updated to no longer depend upon this
    public SafeHandle getRecoImpl() {
        return recoHandle;
    }

    /*! \endcond */

    private void initialize() {
        final SpeechRecognizer _this = this;

        this.recognizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                speechRecognizerObjects.add(_this);
                Contracts.throwIfFail(recognizingSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.recognized.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                speechRecognizerObjects.add(_this);
                Contracts.throwIfFail(recognizedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.canceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                speechRecognizerObjects.add(_this);
                Contracts.throwIfFail(canceledSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.sessionStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                speechRecognizerObjects.add(_this);
                Contracts.throwIfFail(sessionStartedSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.sessionStopped.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                speechRecognizerObjects.add(_this);
                Contracts.throwIfFail(sessionStoppedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.speechStartDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                speechRecognizerObjects.add(_this);
                Contracts.throwIfFail(speechStartDetectedSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.speechEndDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                speechRecognizerObjects.add(_this);
                Contracts.throwIfFail(speechEndDetectedSetCallback(_this.recoHandle.getValue()));
            }
        });

        IntRef propHandle = new IntRef(0);
        Contracts.throwIfFail(getPropertyBagFromRecognizerHandle(_this.recoHandle, propHandle));
        propertyHandle = new PropertyCollection(propHandle);
    }

    private void recognizingEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            SpeechRecognitionEventArgs resultEventArg = new SpeechRecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<SpeechRecognitionEventArgs> handler = this.recognizing;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception e) {}
    }

    private void recognizedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            SpeechRecognitionEventArgs resultEventArg = new SpeechRecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<SpeechRecognitionEventArgs> handler = this.recognized;
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

    private final native long createSpeechRecognizerFromConfig(SafeHandle recoHandle, SafeHandle speechConfigHandle, SafeHandle audioConfigHandle);
    private final native long createSpeechRecognizerFromAutoDetectSourceLangConfig(SafeHandle recoHandle, SafeHandle speechConfigHandle, SafeHandle autoDetectSourceLangConfigHandle, SafeHandle audioConfigHandle);      
    private final native long createSpeechRecognizerFromSourceLangConfig(SafeHandle recoHandle, SafeHandle speechConfigHandle, SafeHandle SourceLangConfigHandle, SafeHandle audioConfigHandle);

    private PropertyCollection propertyHandle = null;
    private boolean disposed = false;
}
