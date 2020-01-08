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
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.PropertyCollection;


/**
 * Performs speech recognition from microphone, file, or other audio input streams, and gets transcribed text as result.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 *
 */
public final class SpeechRecognizer extends com.microsoft.cognitiveservices.speech.Recognizer
{
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
        this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl());
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
            this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl());
        } else {
            this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), audioConfig.getConfigImpl());
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
        this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), autoDetectSourceLangConfig.getImpl());
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
            this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), autoDetectSourceLangConfig.getImpl());
        } else {
            this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), autoDetectSourceLangConfig.getImpl(), audioConfig.getConfigImpl());
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
        this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), sourceLanguageConfig.getImpl());
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
            this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), sourceLanguageConfig.getImpl());
        } else {
            this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), sourceLanguageConfig.getImpl(), audioConfig.getConfigImpl());
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
        this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), sourceLanguage);
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
            this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), sourceLanguage);
        } else {
            this.recoImpl = com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), sourceLanguage, audioConfig.getConfigImpl());
        }
        initialize();
    }

    /**
     * Gets the endpoint ID of a customized speech model that is used for speech recognition.
     * @return the endpoint ID of a customized speech model that is used for speech recognition.
     */
    public String getEndpointId() {
        return recoImpl.GetEndpointId();
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
        recoImpl.SetAuthorizationToken(token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public String getAuthorizationToken() {
        return recoImpl.GetAuthorizationToken();
    }

    /**
     * Gets the spoken language of recognition.
     * @return The spoken language of recognition.
     */
    public String getSpeechRecognitionLanguage() {
        return _Parameters.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets the output format of recognition.
     * @return The output format of recognition.
     */
    public OutputFormat getOutputFormat() {
        if (_Parameters.getProperty(PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse).equals("true")) {
            return OutputFormat.Detailed;
        } else {
            return OutputFormat.Simple;
        }
    }

    /**
     * The collection or properties and their values defined for this SpeechRecognizer.
     * @return The collection or properties and their values defined for this SpeechRecognizer.
     */
    public PropertyCollection getProperties() {
        return _Parameters;
    }

    private com.microsoft.cognitiveservices.speech.PropertyCollection _Parameters;

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

                Runnable runnable = new Runnable() { public void run() { result[0] = new SpeechRecognitionResult(recoImpl.Recognize()); }};
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
                Runnable runnable = new Runnable() { public void run() { recoImpl.StartContinuousRecognition(); }};
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
                Runnable runnable = new Runnable() { public void run() { recoImpl.StopContinuousRecognition(); }};
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
                Runnable runnable = new Runnable() { public void run() { recoImpl.StartKeywordRecognition(model2.getModelImpl()); }};
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
                Runnable runnable = new Runnable() { public void run() { recoImpl.StopKeywordRecognition(); }};
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
            } else {
                if (this.recognizing.isUpdateNotificationOnConnectedFired())
                    recoImpl.getRecognizing().RemoveEventListener(recognizingHandler);
                if (this.recognized.isUpdateNotificationOnConnectedFired())
                    recoImpl.getRecognized().RemoveEventListener(recognizedHandler);
                if (this.canceled.isUpdateNotificationOnConnectedFired())
                    recoImpl.getCanceled().RemoveEventListener(errorHandler);
                if (this.sessionStarted.isUpdateNotificationOnConnectedFired())
                    recoImpl.getSessionStarted().RemoveEventListener(sessionStartedHandler);
                if (this.sessionStopped.isUpdateNotificationOnConnectedFired())
                    recoImpl.getSessionStopped().RemoveEventListener(sessionStoppedHandler);
                if (this.speechStartDetected.isUpdateNotificationOnConnectedFired())
                    recoImpl.getSpeechStartDetected().RemoveEventListener(speechStartDetectedHandler);
                if (this.speechEndDetected.isUpdateNotificationOnConnectedFired())
                    recoImpl.getSpeechEndDetected().RemoveEventListener(speechEndDetectedHandler);

                recognizingHandler.delete();
                recognizedHandler.delete();
                errorHandler.delete();
                recoImpl.delete();
                _Parameters.close();

                _speechRecognizerObjects.remove(this);
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
    static java.util.Set<SpeechRecognizer> _speechRecognizerObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<SpeechRecognizer>());

    // TODO Remove this... After tests are updated to no longer depend upon this
    public com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer getRecoImpl() {
        return recoImpl;
    }

    /*! \endcond */

    private void initialize() {
        super.internalRecognizerImpl = this.recoImpl;

        final SpeechRecognizer _this = this;

        recognizingHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ false);
        this.recognizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _speechRecognizerObjects.add(_this);
                recoImpl.getRecognizing().AddEventListener(recognizingHandler);
            }
        });

        recognizedHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ true);
        this.recognized.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _speechRecognizerObjects.add(_this);
                recoImpl.getRecognized().AddEventListener(recognizedHandler);
            }
        });

        errorHandler = new CanceledHandlerImpl(this);
        this.canceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _speechRecognizerObjects.add(_this);
                recoImpl.getCanceled().AddEventListener(errorHandler);
            }
        });

        this.sessionStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _speechRecognizerObjects.add(_this);
                recoImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
            }
        });

        this.sessionStopped.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _speechRecognizerObjects.add(_this);
                recoImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
            }
        });

        this.speechStartDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _speechRecognizerObjects.add(_this);
                recoImpl.getSpeechStartDetected().AddEventListener(speechStartDetectedHandler);
            }
        });

        this.speechEndDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _speechRecognizerObjects.add(_this);
                recoImpl.getSpeechEndDetected().AddEventListener(speechEndDetectedHandler);
            }
        });

        _Parameters = new PrivatePropertyCollection(recoImpl.getProperties());
    }

    private com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer recoImpl;
    private ResultHandlerImpl recognizingHandler;
    private ResultHandlerImpl recognizedHandler;
    private CanceledHandlerImpl errorHandler;
    private boolean disposed = false;

    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
            super(collection);
        }
    }

    // Defines a private class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class ResultHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventListener {

        ResultHandlerImpl(SpeechRecognizer recognizer, boolean isRecognizedHandler) {
            Contracts.throwIfNull(recognizer, "recognizer");

            this.recognizer = recognizer;
            this.isRecognizedHandler = isRecognizedHandler;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            if (recognizer.disposed) {
                return;
            }

            SpeechRecognitionEventArgs resultEventArg = new SpeechRecognitionEventArgs(eventArgs);
            EventHandlerImpl<SpeechRecognitionEventArgs> handler = isRecognizedHandler ? recognizer.recognized : recognizer.recognizing;
            if (handler != null) {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private SpeechRecognizer recognizer;
        private boolean isRecognizedHandler;
    }

    // Defines a private class to raise an event for error during recognition when a corresponding callback is invoked by the native layer.
    private class CanceledHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionCanceledEventListener {

        CanceledHandlerImpl(SpeechRecognizer recognizer ) {
            Contracts.throwIfNull(recognizer, "recognizer");
            this.recognizer = recognizer;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionCanceledEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            if (recognizer.disposed) {
                return;
            }

            SpeechRecognitionCanceledEventArgs resultEventArg = new SpeechRecognitionCanceledEventArgs(eventArgs);
            EventHandlerImpl<SpeechRecognitionCanceledEventArgs> handler = this.recognizer.canceled;

            if (handler != null) {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private SpeechRecognizer recognizer;
    }
}
