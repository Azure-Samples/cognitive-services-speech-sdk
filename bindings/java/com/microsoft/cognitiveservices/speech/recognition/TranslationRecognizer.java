package com.microsoft.cognitiveservices.speech.recognition;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.recognition.translation.TranslationSynthesisResultEventArgs;
import com.microsoft.cognitiveservices.speech.recognition.translation.TranslationTextResult;
import com.microsoft.cognitiveservices.speech.recognition.translation.TranslationTextResultEventArgs;
import com.microsoft.cognitiveservices.speech.util.EventHandler;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Task;
import com.microsoft.cognitiveservices.speech.util.TaskRunner;

import java.io.IOException;
import java.util.Vector;

/**
  * Performs translation on the speech input.
  */
public final class TranslationRecognizer extends com.microsoft.cognitiveservices.speech.recognition.Recognizer
{
    /**
      * The event IntermediateResultReceived signals that an intermediate recognition result is received.
      */
    public EventHandlerImpl<TranslationTextResultEventArgs> IntermediateResultReceived;

    /**
      * The event FinalResultReceived signals that a final recognition result is received.
      */
    public EventHandlerImpl<TranslationTextResultEventArgs> FinalResultReceived;

    /**
      * The event RecognitionErrorRaised signals that an error occurred during recognition.
      */
    public EventHandlerImpl<RecognitionErrorEventArgs> RecognitionErrorRaised;

    /**
      * The event SynthesisResultReceived signals that a translation synthesis result is received.
      */
    public EventHandlerImpl<TranslationSynthesisResultEventArgs> SynthesisResultReceived;

    TranslationRecognizer(com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer recoImpl)
    {
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
        //recoImpl.getSoundStarted().addEventListener(soundStartedHandler);
        //recoImpl.getSoundStopped().addEventListener(soundStoppedHandler);

        _Parameters = new com.microsoft.cognitiveservices.speech.ParameterCollection<TranslationRecognizer>(this);
    }

    /**
      * Gets/sets the spoken language of audio input.
      * @return The spoken language of audio input.
      */
    public String getSourceLanguage()
    {
            return _Parameters.getString(ParameterNames.TranslationFromLanguage);
    }

    /**
      * Gets/sets the spoken language of audio input.
      * @param value The spoken language of audio input.
      */
    public void setSourceLanguage(String value)
    {
            _Parameters.set(ParameterNames.TranslationFromLanguage, value);
    }

    /**
      * Gets/sets target languages for translation. The language is specified in BCP-47 format.
      * The translation will provide translated text for each of language in the specified list.
      * @return Gets/sets target languages for translation. The language is specified in BCP-47 format. The translation will provide translated text for each of language in the specified list.
      */
    public Vector<String> getTargetLanguages()
    {
        return _targetLanguages;
    }
    // { get; set; }
    private Vector<String> _targetLanguages = new Vector<String>();

    /**
      * Gets/sets a boolean value which indicates whether a voice output of the translated text is desired.
      * @return true, if a voice output of the translated text is desired.
      */
    public boolean getIsVoiceOutputDesired()
    {
        return _isVoidOutputDesired;
    };//{ get; set; }

    /**
      * Gets/sets a boolean value which indicates whether a voice output of the translated text is desired.
      * @param value true, if a voice output of the translated text is desired.
      */
    public void setIsVoiceOutputDesired(boolean value)
    {
        _isVoidOutputDesired = value;
    };//{ get; set; }
    private boolean _isVoidOutputDesired;
    
    /**
      * The collection of parameters and their values defined for this TranslationRecognizer.
      * @return The collection of parameters and their values defined for this TranslationRecognizer.
      */
    public final com.microsoft.cognitiveservices.speech.ParameterCollection<TranslationRecognizer> getParameters()
    {
        return _Parameters;
    }; // { get; }
    com.microsoft.cognitiveservices.speech.ParameterCollection<TranslationRecognizer> _Parameters;

    /**
      * Starts recognition and translation, and stops after the first utterance is recognized. The task returns the translation text as result.
      * @return A task representing the recognition operation. The task returns a value of TranslationTextResult
      * 
      * Create a translation recognizer, get and print the recognition result
      * 
      * static void TranslationRecognizerSample()
      * {
      *   TranslationRecognizer reco = factory.CreateTranslationRecognizer("audioFileName", "en-us", "de-de");
      *
      *   // Starts recognition.
      *   var result = await reco.RecognizeAsync();
      *
      *   Console.WriteLine("Translation Recognition: Recognition result: " + result);
      * }
      * 
      * 
      */
    public Task<TranslationTextResult> recognizeAsync()
    {
        Task<TranslationTextResult> t = new Task<TranslationTextResult>(new TaskRunner() {
            TranslationTextResult result;
            
            @Override
            public void run() {
                result = new TranslationTextResult(recoImpl.recognize());
            }

            @Override
            public Object result() {
                return result;
            }});

        return t;
    }

    /**
      * Starts recognition and translation on a continous audio stream, until StopContinuousRecognitionAsync() is called.
      * User must subscribe to events to receive translation results.
      * @return A task representing the asynchronous operation that starts the recognition.
      */
    public Task<?> startTranslationRecognizer()
    {
        Task<?> t = new Task(new TaskRunner() {

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
      * Stops continuous recognition and translation.
      * @return A task representing the asynchronous operation that stops the translation.
      */
    public Task<?> stopContinuousRecognitionAsync()
    {
        Task<?> t = new Task(new TaskRunner() {

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

    @Override
    protected void dispose(boolean disposing) throws IOException
    {
        if (disposed)
        {
            return;
        }

        if (disposing)
        {
            recoImpl.getIntermediateResult().removeEventListener(intermediateResultHandler);
            recoImpl.getFinalResult().removeEventListener(finalResultHandler);
            recoImpl.getNoMatch().removeEventListener(errorHandler);
            recoImpl.getCanceled().removeEventListener(errorHandler);
            recoImpl.getSessionStarted().removeEventListener(sessionStartedHandler);
            recoImpl.getSessionStopped().removeEventListener(sessionStoppedHandler);

            intermediateResultHandler.delete();
            finalResultHandler.delete();
            errorHandler.delete();
            recoImpl.delete();
            _Parameters.close();
            disposed = true;
            super.dispose(disposing);
        }
    }

    com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer recoImpl;
    private ResultHandlerImpl intermediateResultHandler;
    private ResultHandlerImpl finalResultHandler;
    private ErrorHandlerImpl errorHandler;
    private boolean disposed = false;

    // Defines an internal class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class ResultHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.TranslationTexEventListener
    {
        public ResultHandlerImpl(TranslationRecognizer recognizer, boolean isFinalResultHandler)
        {
            this.recognizer = recognizer;
            this.isFinalResultHandler = isFinalResultHandler;
        }

        @Override
        public void execute(com.microsoft.cognitiveservices.speech.internal.TranslationTextResultEventArgs eventArgs)
        {
            if (recognizer.disposed)
            {
                return;
            }

            TranslationTextResultEventArgs resultEventArg = null; // = new TranslationTextResultEventArgs(eventArgs);
            EventHandlerImpl<TranslationTextResultEventArgs> handler = isFinalResultHandler ? recognizer.FinalResultReceived : recognizer.IntermediateResultReceived;
            if (handler != null)
            {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private TranslationRecognizer recognizer;
        private boolean isFinalResultHandler;
    }

    // Defines an internal class to raise an event for error during recognition when a corresponding callback is invoked by the native layer.
    private class ErrorHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.TranslationTexEventListener
    {
        public ErrorHandlerImpl(TranslationRecognizer recognizer)
        {
            this.recognizer = recognizer;
        }

        @Override
        public void execute(com.microsoft.cognitiveservices.speech.internal.TranslationTextResultEventArgs eventArgs)
        {
            com.microsoft.cognitiveservices.speech.internal.TranslationTexEventListener a = new com.microsoft.cognitiveservices.speech.internal.TranslationTexEventListener();
            if (recognizer.disposed)
            {
                return;
            }

            RecognitionErrorEventArgs resultEventArg = new RecognitionErrorEventArgs(eventArgs.getSessionId(), eventArgs.getResult().getReason());
            EventHandlerImpl<RecognitionErrorEventArgs> handler = this.recognizer.RecognitionErrorRaised;

            if (handler != null)
            {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private TranslationRecognizer recognizer;
    }
}
