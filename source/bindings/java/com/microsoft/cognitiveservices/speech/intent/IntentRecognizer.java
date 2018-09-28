//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.intent;

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionCanceledEventArgs;
import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.internal.IntentTrigger;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Performs intent recognition on the speech input. It returns both recognized text and recognized intent.
 */
public final class IntentRecognizer extends com.microsoft.cognitiveservices.speech.Recognizer {
    /**
     * The event recognizing signals that an intermediate recognition result is received.
     */
    final public EventHandlerImpl<IntentRecognitionEventArgs> recognizing = new EventHandlerImpl<IntentRecognitionEventArgs>();

    /**
     * The event recognized signals that a final recognition result is received.
     */
    final public EventHandlerImpl<IntentRecognitionEventArgs> recognized = new EventHandlerImpl<IntentRecognitionEventArgs>();

    /**
     * The event canceled signals that the intent recognition was canceled.
     */
    final public EventHandlerImpl<IntentRecognitionCanceledEventArgs> canceled = new EventHandlerImpl<IntentRecognitionCanceledEventArgs>();

    /**
     * Initializes an instance of the IntentRecognizer.
     * @param recoImpl The internal recognizer implementation.
     * @param audioInput An audio input configuration associated with the recognizer.
     */
    private IntentRecognizer(com.microsoft.cognitiveservices.speech.internal.IntentRecognizer recoImpl, AudioConfig audioInput) {
        super(audioInput);

        Contracts.throwIfNull(recoImpl, "recoImpl");
        this.recoImpl = recoImpl;

        initialize();
    }

    /**
     * Creates a new instance of an intent recognizer.
     * @param speechConfig speech configuration.
     * @return a new instance of an intent recognizer.
     */
    public IntentRecognizer(com.microsoft.cognitiveservices.speech.SpeechConfig speechConfig) {
        super(null);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        this.recoImpl =  com.microsoft.cognitiveservices.speech.internal.IntentRecognizer.FromConfig(speechConfig.getImpl());
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
            this.recoImpl =  com.microsoft.cognitiveservices.speech.internal.IntentRecognizer.FromConfig(speechConfig.getImpl());

        } else {
            this.recoImpl =  com.microsoft.cognitiveservices.speech.internal.IntentRecognizer.FromConfig(speechConfig.getImpl(), audioConfig.getConfigImpl());
        }

        initialize();
    }

    /**
     * Gets the spoken language of recognition.
     * @return the spoken language of recognition.
     */
    public String getSpeechRecognitionLanguage() {
        return _Parameters.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Sets the authorization token used to communicate with the service.
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
     * The collection or properties and their values defined for this IntentRecognizer.
     * @return The collection or properties and their values defined for this IntentRecognizer.
     */
    public PropertyCollection getProperties() {
        return _Parameters;
    }

    private PropertyCollection _Parameters;

    /**
     * Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
     * Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @return A task representing the recognition operation. The task returns a value of IntentRecognitionResult
     */
    public Future<IntentRecognitionResult> recognizeOnceAsync() {
        return s_executorService.submit(() -> {
            return  new IntentRecognitionResult(recoImpl.Recognize());
        });
    }

    /**
     * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public Future<Void> startContinuousRecognitionAsync() {
        return s_executorService.submit(() -> {
            recoImpl.StartContinuousRecognition();
            return null;
        });
    }

    /**
     * Stops continuous intent recognition.
     * @return A task representing the asynchronous operation that stops the recognition.
     */
    public Future<Void> stopContinuousRecognitionAsync() {
        return s_executorService.submit(() -> {
            recoImpl.StopContinuousRecognition();
            return null;
        });
    }

    /**
     * Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
     * @param simplePhrase The phrase corresponding to the intent.
     */
    public void addIntent(String simplePhrase) {
        Contracts.throwIfNullOrWhitespace(simplePhrase, "simplePhrase");

        recoImpl.AddIntent(simplePhrase);
    }

    /**
     * Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
     * @param simplePhrase The phrase corresponding to the intent.
     * @param intentId A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
     */
    public void addIntent(String simplePhrase, String intentId) {
        Contracts.throwIfNullOrWhitespace(simplePhrase, "simplePhrase");
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");

        recoImpl.AddIntent(simplePhrase, intentId);
    }

    /**
     * Adds a single intent by name from the specified Language Understanding Model.
     * @param model The language understanding model containing the intent.
     * @param intentName The name of the single intent to be included from the language understanding model.
     */
    public void addIntent(LanguageUnderstandingModel model, String intentName) {
        Contracts.throwIfNull(model, "model");
        Contracts.throwIfNullOrWhitespace(intentName, "intentName");

        IntentTrigger trigger = com.microsoft.cognitiveservices.speech.internal.IntentTrigger.From(model.getModelImpl(), intentName);
        recoImpl.AddIntent(trigger, intentName);
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

        IntentTrigger trigger = com.microsoft.cognitiveservices.speech.internal.IntentTrigger.From(model.getModelImpl(), intentName);
        recoImpl.AddIntent(trigger, intentId);
    }

    /**
     * Adds all intents from the specified Language Understanding Model.
     * @param model The language understanding model containing the intents.
     * @param intentId A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
     */
    public void addAllIntents(LanguageUnderstandingModel model, String intentId) {
        Contracts.throwIfNull(model, "model");
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");

        recoImpl.AddAllIntents(model.getModelImpl(), intentId);
    }

    /**
     * Adds all intents from the specified Language Understanding Model.
     * @param model The language understanding model containing the intents.
     */
    public void addAllIntents(LanguageUnderstandingModel model) {
        Contracts.throwIfNull(model, "model");
        recoImpl.AddAllIntents(model.getModelImpl());
    }

    /**
     * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @param model The keyword recognition model that specifies the keyword to be recognized.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public Future<Void> startKeywordRecognitionAsync(KeywordRecognitionModel model) {
        Contracts.throwIfNull(model, "model");

        return s_executorService.submit(() -> {
            recoImpl.StartKeywordRecognition(model.getModelImpl());
            return null;
        });
    }

    /**
     * Stops continuous speech recognition.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @return A task representing the asynchronous operation that stops the recognition.
     */
    public Future<Void> stopKeywordRecognitionAsync() {
        return s_executorService.submit(() -> {
            recoImpl.StopKeywordRecognition();
            return null;
        });
    }

    @Override
    protected void dispose(boolean disposing) {

        if (disposed) {
            return;
        }

        if (disposing) {
            recoImpl.getRecognizing().RemoveEventListener(recognizingHandler);
            recoImpl.getRecognized().RemoveEventListener(recognizedHandler);
            recoImpl.getCanceled().RemoveEventListener(errorHandler);
            recoImpl.getSessionStarted().RemoveEventListener(sessionStartedHandler);
            recoImpl.getSessionStopped().RemoveEventListener(sessionStoppedHandler);
            recoImpl.getSpeechStartDetected().RemoveEventListener(speechStartDetectedHandler);
            recoImpl.getSpeechEndDetected().RemoveEventListener(speechEndDetectedHandler);

            recognizingHandler.delete();
            recognizedHandler.delete();
            errorHandler.delete();
            recoImpl.delete();
            _Parameters.close();
            disposed = true;
            super.dispose(disposing);
        }
    }

    // TODO Remove this... After tests are updated to no longer depend upon this
    public com.microsoft.cognitiveservices.speech.internal.IntentRecognizer getRecoImpl() {
        return recoImpl;
    }

    private void initialize() {
        recognizingHandler = new IntentHandlerImpl(this, /*isRecognizedHandler:*/ false);
        recoImpl.getRecognizing().AddEventListener(recognizingHandler);

        recognizedHandler = new IntentHandlerImpl(this, /*isRecognizedHandler:*/ true);
        recoImpl.getRecognized().AddEventListener(recognizedHandler);

        errorHandler = new CanceledHandlerImpl(this);
        recoImpl.getCanceled().AddEventListener(errorHandler);

        recoImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
        recoImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
        recoImpl.getSpeechStartDetected().AddEventListener(speechStartDetectedHandler);
        recoImpl.getSpeechEndDetected().AddEventListener(speechEndDetectedHandler);

        _Parameters = new PrivatePropertyCollection(recoImpl.getProperties());
    }

    private boolean disposed = false;
    private final com.microsoft.cognitiveservices.speech.internal.IntentRecognizer recoImpl;
    private IntentHandlerImpl recognizingHandler;
    private IntentHandlerImpl recognizedHandler;
    private CanceledHandlerImpl errorHandler;


    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
            super(collection);
        }
    }

    // Defines an internal class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class IntentHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.IntentEventListener {

        public IntentHandlerImpl(IntentRecognizer recognizer, boolean isRecognizedHandler) {
            Contracts.throwIfNull(recognizer, "recognizer");

            this.recognizer = recognizer;
            this.isRecognizedHandler = isRecognizedHandler;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            IntentRecognitionEventArgs resultEventArg = new IntentRecognitionEventArgs(eventArgs);
            EventHandlerImpl<IntentRecognitionEventArgs> handler = isRecognizedHandler ? recognizer.recognized : recognizer.recognizing;

            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        }

        private IntentRecognizer recognizer;
        private boolean isRecognizedHandler;
    }

    // Defines an internal class to raise an event for error during recognition when a corresponding callback is invoked by the native layer.
    private class CanceledHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.IntentCanceledEventListener {

        public CanceledHandlerImpl(IntentRecognizer recognizer) {
            Contracts.throwIfNull(recognizer, "recognizer");
            this.recognizer = recognizer;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionCanceledEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            if (recognizer.disposed) {
                return;
            }

            IntentRecognitionCanceledEventArgs canceledEventArgs = new IntentRecognitionCanceledEventArgs(eventArgs);
            EventHandlerImpl<IntentRecognitionCanceledEventArgs>  handler = this.recognizer.canceled;

            if (handler != null) {
                handler.fireEvent(this, canceledEventArgs);
            }
        }

        private IntentRecognizer recognizer;
    }
}
