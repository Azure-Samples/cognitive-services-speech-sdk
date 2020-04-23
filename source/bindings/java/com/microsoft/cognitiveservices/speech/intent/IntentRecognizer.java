//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.intent;

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionCanceledEventArgs;
import com.microsoft.cognitiveservices.speech.intent.IntentTrigger;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;

/**
 * Performs intent recognition on the speech input. It returns both recognized text and recognized intent.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 */
public final class IntentRecognizer extends com.microsoft.cognitiveservices.speech.Recognizer {
    /**
     * The event recognizing signals that an intermediate recognition result is received.
     */
    final public EventHandlerImpl<IntentRecognitionEventArgs> recognizing = new EventHandlerImpl<IntentRecognitionEventArgs>(eventCounter);

    /**
     * The event recognized signals that a final recognition result is received.
     */
    final public EventHandlerImpl<IntentRecognitionEventArgs> recognized = new EventHandlerImpl<IntentRecognitionEventArgs>(eventCounter);

    /**
     * The event canceled signals that the intent recognition was canceled.
     */
    final public EventHandlerImpl<IntentRecognitionCanceledEventArgs> canceled = new EventHandlerImpl<IntentRecognitionCanceledEventArgs>(eventCounter);

    /**
     * Creates a new instance of an intent recognizer.
     * @param speechConfig speech configuration.
     * @return a new instance of an intent recognizer.
     */
    public IntentRecognizer(com.microsoft.cognitiveservices.speech.SpeechConfig speechConfig) {
        super(null);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        Contracts.throwIfNull(recoHandle, "recoHandle");
        Contracts.throwIfFail(createIntentRecognizerFromConfig(recoHandle, speechConfig.getImpl(), null));
        initialize();
    }

    /**
     * Creates a new instance of an intent recognizer.
     * @param speechConfig speech configuration.
     * @param audioConfig audio configuration.
     * @return a new instance of an intent recognizer.
     */
    public IntentRecognizer(com.microsoft.cognitiveservices.speech.SpeechConfig speechConfig, AudioConfig audioConfig) {
        super(audioConfig);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        if (audioConfig == null) {
            Contracts.throwIfFail(createIntentRecognizerFromConfig(recoHandle, speechConfig.getImpl(), null));
        } else {
            Contracts.throwIfFail(createIntentRecognizerFromConfig(recoHandle, speechConfig.getImpl(), audioConfig.getImpl()));
        }

        initialize();
    }

    /**
     * Gets the spoken language of recognition.
     * @return the spoken language of recognition.
     */
    public String getSpeechRecognitionLanguage() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
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
     * The collection of properties and their values defined for this IntentRecognizer.
     * @return The collection of properties and their values defined for this IntentRecognizer.
     */
    public PropertyCollection getProperties() {
        return propertyHandle;
    }

    /**
     * Starts intent recognition, and returns after a single utterance is recognized. The end of a
     * single utterance is determined by listening for silence at the end or until a maximum of 15
     * seconds of audio is processed.  The task returns the recognition text as result.
     * Note: Since recognizeOnceAsync() returns only a single utterance, it is suitable only for single
     * shot recognition like command or query.
     * For long-running multi-utterance recognition, use startContinuousRecognitionAsync() instead.
     * @return A task representing the recognition operation. The task returns a value of IntentRecognitionResult
     */
    public Future<IntentRecognitionResult> recognizeOnceAsync() {
        final IntentRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<IntentRecognitionResult>() {
            public IntentRecognitionResult  call() {
                // A variable defined in an enclosing scope must be final or effectively final.
                // The compiler treats an array initialized once as an effectively final.
                final IntentRecognitionResult[] result = new IntentRecognitionResult[1];

                Runnable runnable = new Runnable() { public void run() { result[0] = new IntentRecognitionResult(thisReco.recognize()); }};
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
        final IntentRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { thisReco.startContinuousRecognition(recoHandle); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /**
     * Stops continuous intent recognition.
     * @return A task representing the asynchronous operation that stops the recognition.
     */
    public Future<Void> stopContinuousRecognitionAsync() {
        final IntentRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { thisReco.stopContinuousRecognition(recoHandle); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /**
     * Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
     * @param simplePhrase The phrase corresponding to the intent.
     */
    public void addIntent(String simplePhrase) {
        Contracts.throwIfNullOrWhitespace(simplePhrase, "simplePhrase");

        IntentTrigger trigger = IntentTrigger.fromPhrase(simplePhrase);
        Contracts.throwIfFail(addIntent(recoHandle, simplePhrase, trigger.getImpl()));
    }

    /**
     * Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
     * @param simplePhrase The phrase corresponding to the intent.
     * @param intentId A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
     */
    public void addIntent(String simplePhrase, String intentId) {
        Contracts.throwIfNullOrWhitespace(simplePhrase, "simplePhrase");
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");

        IntentTrigger trigger = IntentTrigger.fromPhrase(simplePhrase);
        Contracts.throwIfFail(addIntent(recoHandle, intentId, trigger.getImpl()));
    }

    /**
     * Adds a single intent by name from the specified Language Understanding Model.
     * @param model The language understanding model containing the intent.
     * @param intentName The name of the single intent to be included from the language understanding model.
     */
    public void addIntent(LanguageUnderstandingModel model, String intentName) {
        Contracts.throwIfNull(model, "model");
        Contracts.throwIfNullOrWhitespace(intentName, "intentName");

        IntentTrigger trigger = IntentTrigger.fromModel(model.getImpl(), intentName);
        Contracts.throwIfFail(addIntent(recoHandle, intentName, trigger.getImpl()));
    }

    /**
     * Adds a single intent by name from the specified Language Understanding Model.
     * @param model The language understanding model containing the intent.
     * @param intentName The name of the single intent to be included from the language understanding model.
     * @param intentId A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
     */
    public void addIntent(LanguageUnderstandingModel model, String intentName, String intentId) {
        Contracts.throwIfNull(model, "model");
        Contracts.throwIfNullOrWhitespace(intentName, "intentName");
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");

        IntentTrigger trigger = IntentTrigger.fromModel(model.getImpl(), intentName);
        Contracts.throwIfFail(addIntent(recoHandle, intentId, trigger.getImpl()));
    }

    /**
     * Adds all intents from the specified Language Understanding Model.
     * @param model The language understanding model containing the intents.
     * @param intentId A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
     */
    public void addAllIntents(LanguageUnderstandingModel model, String intentId) {
        Contracts.throwIfNull(model, "model");
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");

        IntentTrigger trigger = IntentTrigger.fromModel(model.getImpl());
        Contracts.throwIfFail(addIntent(recoHandle, intentId, trigger.getImpl()));
    }

    /**
     * Adds all intents from the specified Language Understanding Model.
     * @param model The language understanding model containing the intents.
     */
    public void addAllIntents(LanguageUnderstandingModel model) {
        Contracts.throwIfNull(model, "model");
        IntentTrigger trigger = IntentTrigger.fromModel(model.getImpl());
        Contracts.throwIfFail(addIntent(recoHandle, null, trigger.getImpl()));
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

        final IntentRecognizer thisReco = this;
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
        final IntentRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { thisReco.stopKeywordRecognition(recoHandle); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /*! \cond PROTECTED */

    @Override
    protected void dispose(final boolean disposing) {
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
                intentRecognizerObjects.remove(this);
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
    static java.util.Set<IntentRecognizer> intentRecognizerObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<IntentRecognizer>());

    // TODO Remove this... After tests are updated to no longer depend upon this
    public SafeHandle getRecoImpl() {
        return recoHandle;
    }

    /*! \endcond */

    private void initialize() {
        final IntentRecognizer _this = this;

        this.recognizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                intentRecognizerObjects.add(_this);
                Contracts.throwIfFail(recognizingSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.recognized.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                intentRecognizerObjects.add(_this);
                Contracts.throwIfFail(recognizedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.canceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                intentRecognizerObjects.add(_this);
                Contracts.throwIfFail(canceledSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.sessionStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                intentRecognizerObjects.add(_this);
                Contracts.throwIfFail(sessionStartedSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.sessionStopped.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                intentRecognizerObjects.add(_this);
                Contracts.throwIfFail(sessionStoppedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.speechStartDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                intentRecognizerObjects.add(_this);
                Contracts.throwIfFail(speechStartDetectedSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.speechEndDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                intentRecognizerObjects.add(_this);
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
            IntentRecognitionEventArgs resultEventArg = new IntentRecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<IntentRecognitionEventArgs> handler = this.recognizing;
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
            IntentRecognitionEventArgs resultEventArg = new IntentRecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<IntentRecognitionEventArgs> handler = this.recognized;
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
            IntentRecognitionCanceledEventArgs resultEventArg = new IntentRecognitionCanceledEventArgs(eventArgs, true);
            EventHandlerImpl<IntentRecognitionCanceledEventArgs> handler = this.canceled;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private final native long createIntentRecognizerFromConfig(SafeHandle recoHandle, SafeHandle speechConfigHandle, SafeHandle audioConfigHandle);
    private final native long addIntent(SafeHandle recoHandle, String intentId, SafeHandle triggerHandle);

    private PropertyCollection propertyHandle = null;
    private boolean disposed = false;
}
