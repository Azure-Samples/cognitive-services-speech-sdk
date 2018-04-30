package com.microsoft.cognitiveservices.speech.intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.ParameterCollection;
import com.microsoft.cognitiveservices.speech.RecognizerParameterNames;
import com.microsoft.cognitiveservices.speech.RecognitionErrorEventArgs;
import com.microsoft.cognitiveservices.speech.internal.IntentTrigger;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Task;
import com.microsoft.cognitiveservices.speech.util.TaskRunner;

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

    // TODO should only be visible internally for SpeechFactory
    /**
      * Initializes an instance of the IntentRecognizer.
      * @param recoImpl The internal recognizer implementation.
      */
    public IntentRecognizer(com.microsoft.cognitiveservices.speech.internal.IntentRecognizer recoImpl)
    {
        this.recoImpl = recoImpl;

        intermediateResultHandler = new IntentHandlerImpl(this, /*isFinalResultHandler:*/ false);
        recoImpl.getIntermediateResult().addEventListener(intermediateResultHandler);

        finalResultHandler = new IntentHandlerImpl(this, /*isFinalResultHandler:*/ true);
        recoImpl.getFinalResult().addEventListener(finalResultHandler);

        errorHandler = new ErrorHandlerImpl(this);
        recoImpl.getNoMatch().addEventListener(errorHandler);
        recoImpl.getCanceled().addEventListener(errorHandler);

        recoImpl.getSessionStarted().addEventListener(sessionStartedHandler);
        recoImpl.getSessionStopped().addEventListener(sessionStoppedHandler);
        recoImpl.getSpeechStartDetected().addEventListener(speechStartDetectedHandler);
        recoImpl.getSpeechEndDetected().addEventListener(speechEndDetectedHandler);
    }

    /**
      * Gets the spoken language of recognition.
      * @return the spoken language of recognition.
      */
    public String getLanguage() {
            return _Parameters.getString(RecognizerParameterNames.SpeechRecognitionLanguage);
    }

    /**
      * Sets the spoken language of recognition.
      * @param value the spoken language of recognition.
      */
    public void setLanguage(String value) {
            _Parameters.set(RecognizerParameterNames.SpeechRecognitionLanguage, value);
    }

    /**
      * The collection of parameters and their values defined for this IntentRecognizer.
      * @return The collection of parameters and their values defined for this IntentRecognizer.
      */
    public ParameterCollection<IntentRecognizer> getParameters() {
        return _Parameters;
    }// { get; }
    private ParameterCollection<IntentRecognizer> _Parameters;

    /**
      * Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
      * @return A task representing the recognition operation. The task returns a value of IntentRecognitionResult
      */
    public Task<IntentRecognitionResult> recognizeAsync() {
        
        Task<IntentRecognitionResult> t = new Task<IntentRecognitionResult>(new TaskRunner<IntentRecognitionResult>() {
            IntentRecognitionResult result;
            
            @Override
            public void run() {
                result = new IntentRecognitionResult(recoImpl.recognize());
            }

            @Override
            public IntentRecognitionResult result() {
                return result;
            }});
        
        return t;
    }

    /**
      * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
      * User must subscribe to events to receive recognition results.
      * @return A task representing the asynchronous operation that starts the recognition.
      */
    public Task<?> startContinuousRecognitionAsync() {
        
        Task<Object> t = new Task<Object>(new TaskRunner<Object>() {

            @Override
            public void run() {
                recoImpl.startContinuousRecognition();
            }

            @Override
            public Object result() {
                return null;
            }});
 
        return t;
    }

    /**
      * Stops continuous intent recognition.
      * @return A task representing the asynchronous operation that stops the recognition.
      */
    public Task<?> stopContinuousRecognitionAsync() {
        
        Task<Object> t = new Task<Object>(new TaskRunner<Object>() {

            @Override
            public void run() {
                recoImpl.stopContinuousRecognition();
            }

            @Override
            public Object result() {
                return null;
            }});

        return t;
    }

    /**
      * Adds a phrase that should be recognized as intent.
      * @param intentId A String that represents the identifier of the intent to be recognized.
      * @param phrase A String that specifies the phrase representing the intent.
      */
    public void addIntent(String intentId, String phrase) {
        recoImpl.addIntent(intentId, phrase);
    }

    /**
      * Adds an intent from Language Understanding service for recognition.
      * @param intentId A String that represents the identifier of the intent to be recognized.
      * @param model The intent model from Language Understanding service.
      * @param intentName The intent name defined in the intent model. If it is null, all intent names defined in the model will be added.
      */
    public void addIntent(String intentId, LanguageUnderstandingModel model, String intentName) {
        IntentTrigger trigger = com.microsoft.cognitiveservices.speech.internal.IntentTrigger.from(model.modelImpl, intentName);
        recoImpl.addIntent(intentId, trigger);
    }
   
    /**
      * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
      * User must subscribe to events to receive recognition results.
      * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
      * @param keyword The keyword to recognize.
      * @return A task representing the asynchronous operation that starts the recognition.
      */
    public Task<?> startKeywordRecognitionAsync(String keyword) {
        
        Task<?> t = new Task<Object>(new TaskRunner<Object>() {

            @Override
            public void run() {
                recoImpl.startKeywordRecognition(keyword);
            }

            @Override
            public Object result() {
                return null;
            }});
        
        return t;
    }

    /**
      * Stops continuous speech recognition.
      * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
      * @return A task representing the asynchronous operation that stops the recognition.
      */
    public Task<?> stopKeywordRecognitionAsync() {
        
        Task<?> t = new Task<Object>(new TaskRunner<Object>() {

            @Override
            public void run() {
                recoImpl.stopKeywordRecognition();
            }

            @Override
            public Object result() {
                return null;
            }});
        
        return t;
    }

    @Override
    protected void dispose(boolean disposing) {
        
        if (disposed) {
            return;
        }

        if (disposing) {
            recoImpl.getIntermediateResult().removeEventListener(intermediateResultHandler);
            recoImpl.getFinalResult().removeEventListener(finalResultHandler);
            recoImpl.getNoMatch().removeEventListener(errorHandler);
            recoImpl.getCanceled().removeEventListener(errorHandler);
            recoImpl.getSessionStarted().removeEventListener(sessionStartedHandler);
            recoImpl.getSessionStopped().removeEventListener(sessionStoppedHandler);
            recoImpl.getSpeechStartDetected().removeEventListener(speechStartDetectedHandler);
            recoImpl.getSpeechEndDetected().removeEventListener(speechEndDetectedHandler);

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
            this.recognizer = recognizer;
            this.isFinalResultHandler = isFinalResultHandler;
        }

        @Override
        public void execute(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs eventArgs) {
            
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
        public void execute(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs eventArgs) {
            
            RecognitionErrorEventArgs resultEventArg = new RecognitionErrorEventArgs(eventArgs.getSessionId(), eventArgs.getResult().getReason());
            EventHandlerImpl<RecognitionErrorEventArgs>  handler = this.recognizer.RecognitionErrorRaised;

            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        }

        private IntentRecognizer recognizer;
    }
}
