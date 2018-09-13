package com.microsoft.cognitiveservices.speech.intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.RecognizerProperties;
import com.microsoft.cognitiveservices.speech.SpeechPropertyId;
import com.microsoft.cognitiveservices.speech.RecognitionErrorEventArgs;
import com.microsoft.cognitiveservices.speech.internal.IntentTrigger;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
  * Perform intent recognition on the speech input. It returns both recognized text and recognized intent.
  */
public final class IntentRecognizer extends com.microsoft.cognitiveservices.speech.Recognizer {
    /**
      * The event IntermediateResultReceived signals that an intermediate recognition result is received.
      */
    final public EventHandlerImpl<IntentRecognitionResultEventArgs> IntermediateResultReceived = new EventHandlerImpl<IntentRecognitionResultEventArgs>();

    /**
      * The event FinalResultReceived signals that a final recognition result is received.
      */
    final public EventHandlerImpl<IntentRecognitionResultEventArgs> FinalResultReceived = new EventHandlerImpl<IntentRecognitionResultEventArgs>();

    /**
      * The event RecognitionErrorRaised signals that an error occurred during recognition.
      */
    final public EventHandlerImpl<RecognitionErrorEventArgs> RecognitionErrorRaised = new EventHandlerImpl<RecognitionErrorEventArgs>();

    /**
      * Initializes an instance of the IntentRecognizer.
      * @param recoImpl The internal recognizer implementation.
      * @param audioInput An audio input configuration associated with the recognizer.
      */
    private IntentRecognizer(com.microsoft.cognitiveservices.speech.internal.IntentRecognizer recoImpl, AudioConfig audioInput) {
        super(audioInput);

        Contracts.throwIfNull(recoImpl, "recoImpl");
        this.recoImpl = recoImpl;

        intermediateResultHandler = new IntentHandlerImpl(this, /*isFinalResultHandler:*/ false);
        recoImpl.getIntermediateResult().AddEventListener(intermediateResultHandler);

        finalResultHandler = new IntentHandlerImpl(this, /*isFinalResultHandler:*/ true);
        recoImpl.getFinalResult().AddEventListener(finalResultHandler);

        errorHandler = new ErrorHandlerImpl(this);
        recoImpl.getCanceled().AddEventListener(errorHandler);

        recoImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
        recoImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
        recoImpl.getSpeechStartDetected().AddEventListener(speechStartDetectedHandler);
        recoImpl.getSpeechEndDetected().AddEventListener(speechEndDetectedHandler);
    
        _Parameters = new RecognizerProperties<IntentRecognizer>(this);
    }

    /**
      * Create a new instance of an intent recognizer.
      * @return a new instance of an intent recognizer.
      * @param speechConfig speech configuration.
      */
    public IntentRecognizer(com.microsoft.cognitiveservices.speech.SpeechConfig speechConfig)
    {
        this(com.microsoft.cognitiveservices.speech.internal.IntentRecognizer.FromConfig(speechConfig.getImpl(), null), null);
    }

    /**
      * Create a new instance of an intent recognizer.
      * @return a new instance of an intent recognizer.
      * @param speechConfig speech configuration.
      * @param speechConfig audio configuration.
      */
    public IntentRecognizer(com.microsoft.cognitiveservices.speech.SpeechConfig speechConfig, AudioConfig audioConfig)
    {
        this(com.microsoft.cognitiveservices.speech.internal.IntentRecognizer.FromConfig(speechConfig.getImpl(), audioConfig.getConfigImpl()), audioConfig); 
    }

    /**
      * Gets the spoken language of recognition.
      * @return the spoken language of recognition.
      */
    public String getSpeechRecognitionLanguage() {
        return _Parameters.getProperty(SpeechPropertyId.SpeechServiceConnection_RecoLanguage);
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
      * sets the authorization token used to communicate with the service.
      * @return Authorization token.
      */
    public String getAuthorizationToken() {
        return recoImpl.GetAuthorizationToken();
    }

    /**
      * The collection of parameters and their values defined for this IntentRecognizer.
      * @return The collection of parameters and their values defined for this IntentRecognizer.
      */
    public PropertyCollection getParameters() {
        return _Parameters;
    }

    private RecognizerProperties<IntentRecognizer> _Parameters;

    /**
      * Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
      * Note: RecognizeAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
      * @return A task representing the recognition operation. The task returns a value of IntentRecognitionResult
      */
    public Future<IntentRecognitionResult> recognizeAsync() {
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
      * Adds a phrase that should be recognized as intent.
      * @param intentId A String that represents the identifier of the intent to be recognized.
      * @param phrase A String that specifies the phrase representing the intent.
      */
    public void addIntent(String intentId, String phrase) {
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");
        Contracts.throwIfNullOrWhitespace(phrase, "phrase");

        recoImpl.AddIntent(intentId, phrase);
    }

    /**
      * Adds an intent from Language Understanding service for recognition.
      * @param intentId A String that represents the identifier of the intent to be recognized. Ignored if intentName is empty.
      * @param model The intent model from Language Understanding service.
      * @param intentName The intent name defined in the intent model. If it empty, all intent names defined in the model will be added.
      */
    public void addIntent(String intentId, LanguageUnderstandingModel model, String intentName) {
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");
        Contracts.throwIfNullOrWhitespace(intentName, "intentName");
        Contracts.throwIfNull(model, "model");

        IntentTrigger trigger = com.microsoft.cognitiveservices.speech.internal.IntentTrigger.From(model.getModelImpl(), intentName);
        recoImpl.AddIntent(intentId, trigger);
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
            recoImpl.getIntermediateResult().RemoveEventListener(intermediateResultHandler);
            recoImpl.getFinalResult().RemoveEventListener(finalResultHandler);
            recoImpl.getCanceled().RemoveEventListener(errorHandler);
            recoImpl.getSessionStarted().RemoveEventListener(sessionStartedHandler);
            recoImpl.getSessionStopped().RemoveEventListener(sessionStoppedHandler);
            recoImpl.getSpeechStartDetected().RemoveEventListener(speechStartDetectedHandler);
            recoImpl.getSpeechEndDetected().RemoveEventListener(speechEndDetectedHandler);

            intermediateResultHandler.delete();
            finalResultHandler.delete();
            errorHandler.delete();
            recoImpl.delete();
            _Parameters.close();
            disposed = true;
            super.dispose(disposing);
        }
    }

    
    // TODO should only visible to property collection
    public com.microsoft.cognitiveservices.speech.internal.IntentRecognizer getRecoImpl() {
        return recoImpl;
    }
     
    private boolean disposed = false;
    private final com.microsoft.cognitiveservices.speech.internal.IntentRecognizer recoImpl;
    private IntentHandlerImpl intermediateResultHandler;
    private IntentHandlerImpl finalResultHandler;
    private ErrorHandlerImpl errorHandler;

    // Defines an internal class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class IntentHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.IntentEventListener {
        
        public IntentHandlerImpl(IntentRecognizer recognizer, boolean isFinalResultHandler) {
            Contracts.throwIfNull(recognizer, "recognizer");

            this.recognizer = recognizer;
            this.isFinalResultHandler = isFinalResultHandler;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            
            IntentRecognitionResultEventArgs resultEventArg = new IntentRecognitionResultEventArgs(eventArgs);
            EventHandlerImpl<IntentRecognitionResultEventArgs> handler = isFinalResultHandler ? recognizer.FinalResultReceived : recognizer.IntermediateResultReceived;
            
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        }

        private IntentRecognizer recognizer;
        private boolean isFinalResultHandler;
    }

    // Defines an internal class to raise an event for error during recognition when a corresponding callback is invoked by the native layer.
    private class ErrorHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.IntentEventListener {
        
        public ErrorHandlerImpl(IntentRecognizer recognizer) {
            this.recognizer = recognizer;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            
            RecognitionErrorEventArgs resultEventArg = new RecognitionErrorEventArgs(eventArgs.getSessionId(), eventArgs.GetResult().getReason());
            EventHandlerImpl<RecognitionErrorEventArgs>  handler = this.recognizer.RecognitionErrorRaised;

            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        }

        private IntentRecognizer recognizer;
    }
}
