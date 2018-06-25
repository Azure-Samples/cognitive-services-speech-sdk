package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
  * Performs speech recognition from microphone, file, or other audio input streams, and gets transcribed text as result.
  * 
  */
public final class SpeechRecognizer extends com.microsoft.cognitiveservices.speech.Recognizer
{
    /**
      * The event IntermediateResultReceived signals that an intermediate recognition result is received.
      */
    final public EventHandlerImpl<SpeechRecognitionResultEventArgs> IntermediateResultReceived = new EventHandlerImpl<SpeechRecognitionResultEventArgs>();

    /**
      * The event FinalResultReceived signals that a final recognition result is received.
      */
    final public EventHandlerImpl<SpeechRecognitionResultEventArgs> FinalResultReceived = new EventHandlerImpl<SpeechRecognitionResultEventArgs>();

    /**
      * The event RecognitionErrorRaised signals that an error occurred during recognition.
      */
    final public EventHandlerImpl<RecognitionErrorEventArgs> RecognitionErrorRaised = new EventHandlerImpl<RecognitionErrorEventArgs>();

    /**
      * SpeechRecognizer constructor.
      * @param recoImpl The recognizer implementation
      * @param ais An optional audio input stream associated with the recognizer
      */
    public SpeechRecognizer(com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer recoImpl, AudioInputStream ais) {
        super(ais);

        Contracts.throwIfNull(recoImpl, "recoImpl");
        this.recoImpl = recoImpl;

        intermediateResultHandler = new ResultHandlerImpl(this, /*isFinalResultHandler:*/ false);
        recoImpl.getIntermediateResult().addEventListener(intermediateResultHandler);

        finalResultHandler = new ResultHandlerImpl(this, /*isFinalResultHandler:*/ true);
        recoImpl.getFinalResult().addEventListener(finalResultHandler);

        errorHandler = new ErrorHandlerImpl(this);
        recoImpl.getNoMatch().addEventListener(errorHandler);
        recoImpl.getCanceled().addEventListener(errorHandler);

        recoImpl.getSessionStarted().addEventListener(sessionStartedHandler);
        recoImpl.getSessionStopped().addEventListener(sessionStoppedHandler);
        recoImpl.getSpeechStartDetected().addEventListener(speechStartDetectedHandler);
        recoImpl.getSpeechEndDetected().addEventListener(speechEndDetectedHandler);

        _Parameters = new ParameterCollection<SpeechRecognizer>(this);
    }

    /**
      * Gets the deployment id of a customized speech model that is used for speech recognition.
      * @return the deployment id of a customized speech model that is used for speech recognition.
      */
    public String getDeploymentId() {
        return _Parameters.getString(RecognizerParameterNames.SpeechModelId);
    }
    
    /**
      * Sets the deployment id of a customized speech model that is used for speech recognition.
      * @param value The deployment id of a customized speech model that is used for speech recognition.
      */
    public void setDeploymentId(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        _Parameters.set(RecognizerParameterNames.SpeechModelId, value);
    }

    /**
      * Gets the spoken language of recognition.
      * @return The spoken language of recognition.
      */
    public String getLanguage() {
        return _Parameters.getString(RecognizerParameterNames.SpeechRecognitionLanguage);
    }

    /**
      * The collection of parameters and their values defined for this SpeechRecognizer.
      * @return The collection of parameters and their values defined for this SpeechRecognizer.
      */
    public ParameterCollection<SpeechRecognizer> getParameters() {
        return _Parameters;
    }// { get; }
    private ParameterCollection<SpeechRecognizer> _Parameters;

    /**
      * Starts speech recognition, and stops after the first utterance is recognized. The task returns the recognition text as result.
      * @return A task representing the recognition operation. The task returns a value of SpeechRecognitionResult
      */
    public Future<SpeechRecognitionResult> recognizeAsync() {
        return s_executorService.submit(() -> {
                return new SpeechRecognitionResult(recoImpl.recognize()); 
            });
    }

    /**
      * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
      * User must subscribe to events to receive recognition results.
      * @return A task representing the asynchronous operation that starts the recognition.
      */
    public Future<Void> startContinuousRecognitionAsync() {
        return s_executorService.submit(() -> {
                recoImpl.startContinuousRecognitionAsync();
                return null;
            });
    }

    /**
      * Stops continuous speech recognition.
      * @return A task representing the asynchronous operation that stops the recognition.
      */
    public Future<Void> stopContinuousRecognitionAsync() {
        return s_executorService.submit(() -> {
                recoImpl.stopContinuousRecognitionAsync();
                return null;
            });
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
                recoImpl.startKeywordRecognition(model.getModelImpl());
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
                recoImpl.stopKeywordRecognition();
                return null;
            });
    }
    
    @Override
    protected void dispose(boolean disposing)
    {
        if (disposed) {
            return;
        }

        if (disposing) {
            getRecoImpl().getIntermediateResult().removeEventListener(intermediateResultHandler);
            getRecoImpl().getFinalResult().removeEventListener(finalResultHandler);
            getRecoImpl().getNoMatch().removeEventListener(errorHandler);
            getRecoImpl().getCanceled().removeEventListener(errorHandler);
            getRecoImpl().getSessionStarted().removeEventListener(sessionStartedHandler);
            getRecoImpl().getSessionStopped().removeEventListener(sessionStoppedHandler);
            getRecoImpl().getSpeechStartDetected().removeEventListener(speechStartDetectedHandler);
            getRecoImpl().getSpeechEndDetected().removeEventListener(speechEndDetectedHandler);

            intermediateResultHandler.delete();
            finalResultHandler.delete();
            errorHandler.delete();
            getRecoImpl().delete();
            _Parameters.close();
            disposed = true;
            super.dispose(disposing);
        }
    }

    public com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer getRecoImpl() {
        return recoImpl;
    }

    private com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer recoImpl;
    private ResultHandlerImpl intermediateResultHandler;
    private ResultHandlerImpl finalResultHandler;
    private ErrorHandlerImpl errorHandler;
    private boolean disposed = false;

    // Defines an internal class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class ResultHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventListener {
        
        ResultHandlerImpl(SpeechRecognizer recognizer, boolean isFinalResultHandler) {
            Contracts.throwIfNull(recognizer, "recognizer");

            this.recognizer = recognizer;
            this.isFinalResultHandler = isFinalResultHandler;
        }

        @Override
        public void execute(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            
            if (recognizer.disposed) {
                return;
            }

            SpeechRecognitionResultEventArgs resultEventArg = new SpeechRecognitionResultEventArgs(eventArgs);
            EventHandlerImpl<SpeechRecognitionResultEventArgs> handler = isFinalResultHandler ? recognizer.FinalResultReceived : recognizer.IntermediateResultReceived;
            if (handler != null) {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private SpeechRecognizer recognizer;
        private boolean isFinalResultHandler;
    }

    // Defines an internal class to raise an event for error during recognition when a corresponding callback is invoked by the native layer.
    private class ErrorHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventListener {
        
        ErrorHandlerImpl(SpeechRecognizer recognizer) {
            Contracts.throwIfNull(recognizer, "recognizer");

            this.recognizer = recognizer;
        }

        @Override
        public void execute(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs eventArgs) {
            
            if (recognizer.disposed) {
                return;
            }

            RecognitionErrorEventArgs resultEventArg = new RecognitionErrorEventArgs(eventArgs.getSessionId(), eventArgs.getResult().getReason());
            EventHandlerImpl<RecognitionErrorEventArgs> handler = this.recognizer.RecognitionErrorRaised;

            if (handler != null) {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private SpeechRecognizer recognizer;
    }
}
