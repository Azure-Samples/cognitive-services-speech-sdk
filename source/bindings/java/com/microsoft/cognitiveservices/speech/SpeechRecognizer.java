package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionCanceledEventArgs;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.PropertyCollection;


/**
  * Performs speech recognition from microphone, file, or other audio input streams, and gets transcribed text as result.
  * 
  */
public final class SpeechRecognizer extends com.microsoft.cognitiveservices.speech.Recognizer
{
    /**
      * The event recognizing signals that an intermediate recognition result is received.
      */
    final public EventHandlerImpl<SpeechRecognitionResultEventArgs> recognizing = new EventHandlerImpl<SpeechRecognitionResultEventArgs>();

    /**
      * The event recognized signals that a final recognition result is received.
      */
    final public EventHandlerImpl<SpeechRecognitionResultEventArgs> recognized = new EventHandlerImpl<SpeechRecognitionResultEventArgs>();

    /**
      * The event canceled signals that the recognition was canceled.
      */
    final public EventHandlerImpl<SpeechRecognitionCanceledEventArgs> canceled = new EventHandlerImpl<SpeechRecognitionCanceledEventArgs>();

    /**
      * SpeechRecognizer constructor.
      * @param recoImpl The recognizer implementation
      * @param audioInput An optional audio input configuration associated with the recognizer
      */
    private SpeechRecognizer(com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer recoImpl, AudioConfig audioConfig) {
        super(audioConfig);

        Contracts.throwIfNull(recoImpl, "recoImpl");
        this.recoImpl = recoImpl;

        recognizingHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ false);
        recoImpl.getRecognizing().AddEventListener(recognizingHandler);

        recognizedHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ true);
        recoImpl.getRecognized().AddEventListener(recognizedHandler);

        errorHandler = new CanceledHandlerImpl(this);
        recoImpl.getCanceled().AddEventListener(errorHandler);

        recoImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
        recoImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
        recoImpl.getSpeechStartDetected().AddEventListener(speechStartDetectedHandler);
        recoImpl.getSpeechEndDetected().AddEventListener(speechEndDetectedHandler);

        _Parameters = new PrivatePropertyCollection(recoImpl.getProperties());
    }

    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
          super(collection);
        }
    }

    /**
      * Initializes a new instance of Speech Recognizer.
      * @param speechConfig speech configuration.
      */
    public SpeechRecognizer(SpeechConfig speechConfig)
    {
        this(com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), null), null);
    }

    /**
      * Initializes a new instance of Speech Recognizer.
      * @param speechConfig speech configuration.
      * @param audioConfig audio configuration.
      */
    public SpeechRecognizer(SpeechConfig speechConfig, AudioConfig audioConfig)
    {
        this(com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer.FromConfig(speechConfig.getImpl(), audioConfig.getConfigImpl()), audioConfig);
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
      * Starts speech recognition, and stops after the first utterance is recognized. The task returns the recognition text as result.
      * Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
      * @return A task representing the recognition operation. The task returns a value of SpeechRecognitionResult
      */
    public Future<SpeechRecognitionResult> recognizeOnceAsync() {
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
    public com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer getRecoImpl() {
        return recoImpl;
    }

    private com.microsoft.cognitiveservices.speech.internal.SpeechRecognizer recoImpl;
    private ResultHandlerImpl recognizingHandler;
    private ResultHandlerImpl recognizedHandler;
    private CanceledHandlerImpl errorHandler;
    private boolean disposed = false;

    // Defines an internal class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
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

            SpeechRecognitionResultEventArgs resultEventArg = new SpeechRecognitionResultEventArgs(eventArgs);
            EventHandlerImpl<SpeechRecognitionResultEventArgs> handler = isRecognizedHandler ? recognizer.recognized : recognizer.recognizing;
            if (handler != null) {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private SpeechRecognizer recognizer;
        private boolean isRecognizedHandler;
    }

    // Defines an internal class to raise an event for error during recognition when a corresponding callback is invoked by the native layer.
    private class CanceledHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionCanceledEventListener {
        
        CanceledHandlerImpl(SpeechRecognizer recognizer) {
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
