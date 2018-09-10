package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
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
      * @param audioInput An optional audio input configuration associated with the recognizer
      */
    public SpeechRecognizer(com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer recoImpl, AudioConfig audioInput) {
        super(audioInput);

        Contracts.throwIfNull(recoImpl, "recoImpl");
        this.recoImpl = recoImpl;

        intermediateResultHandler = new ResultHandlerImpl(this, /*isFinalResultHandler:*/ false);
        recoImpl.getIntermediateResult().AddEventListener(intermediateResultHandler);

        finalResultHandler = new ResultHandlerImpl(this, /*isFinalResultHandler:*/ true);
        recoImpl.getFinalResult().AddEventListener(finalResultHandler);

        errorHandler = new ErrorHandlerImpl(this);
        recoImpl.getCanceled().AddEventListener(errorHandler);

        recoImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
        recoImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
        recoImpl.getSpeechStartDetected().AddEventListener(speechStartDetectedHandler);
        recoImpl.getSpeechEndDetected().AddEventListener(speechEndDetectedHandler);

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
      * Gets the output format of recognition.
      * @return The output format of recognition.
      */
    public OutputFormat getOutputFormat() {
        if (_Parameters.getString(RecognizerParameterNames.OutputFormat).equals("DETAILED")) {
            return OutputFormat.Detailed;
        } else {
            return OutputFormat.Simple;
        }
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
      * Note: RecognizeAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
      * @return A task representing the recognition operation. The task returns a value of SpeechRecognitionResult
      */
    public Future<SpeechRecognitionResult> recognizeAsync() {
        return s_executorService.submit(() -> {
                return new SpeechRecognitionResult(recoImpl.Recognize()); 
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
      * Stops continuous speech recognition.
      * @return A task representing the asynchronous operation that stops the recognition.
      */
    public Future<Void> stopContinuousRecognitionAsync() {
        return s_executorService.submit(() -> {
                recoImpl.StopContinuousRecognition();
                return null;
            });
    }

    /**
      * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
      * User must subscribe to events to receive recognition results.
      * Note: Key word spotting functionality is only available on the Speech Devices SDK. This functionality is currently not included in the SDK itself.
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
      * Note: Key word spotting functionality is only available on the Speech Devices SDK. This functionality is currently not included in the SDK itself.
      * @return A task representing the asynchronous operation that stops the recognition.
      */
    public Future<Void> stopKeywordRecognitionAsync() {
        return s_executorService.submit(() -> {
                recoImpl.StopKeywordRecognition();
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
        public void Execute(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs eventArgs) {
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
        public void Execute(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs eventArgs) {
            
            if (recognizer.disposed) {
                return;
            }

            RecognitionErrorEventArgs resultEventArg = new RecognitionErrorEventArgs(eventArgs.getSessionId(), eventArgs.GetResult().getReason());
            EventHandlerImpl<RecognitionErrorEventArgs> handler = this.recognizer.RecognitionErrorRaised;

            if (handler != null) {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private SpeechRecognizer recognizer;
    }
}
