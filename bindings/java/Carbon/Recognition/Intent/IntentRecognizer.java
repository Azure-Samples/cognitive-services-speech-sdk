package Carbon.Recognition.Intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.IOException;

import Carbon.ParameterCollection;
import Carbon.Internal.IntentTrigger;
import Carbon.Recognition.IntentModel;
import Carbon.Recognition.ParameterNames;
import Carbon.Recognition.RecognitionErrorEventArgs;
import Carbon.Recognition.Translation.TranslationTextResult;
import Carbon.Util.EventHandler;
import Carbon.Util.EventHandlerImpl;
import Carbon.Util.Task;
import Carbon.Util.TaskRunner;

/// <summary>
/// Perform intent recognition on the speech input. It returns both recognized text and recognized intent.
/// </summary>
public final class IntentRecognizer extends Carbon.Recognition.Recognizer
{
    /// <summary>
    /// The event <see cref="IntermediateResultReceived"/> signals that an intermediate recognition result is received.
    /// </summary>
    public EventHandlerImpl<IntentRecognitionResultEventArgs> IntermediateResultReceived;

    /// <summary>
    /// The event <see cref="FinalResultReceived"/> signals that a final recognition result is received.
    /// </summary>
    public EventHandlerImpl<IntentRecognitionResultEventArgs> FinalResultReceived;

    /// <summary>
    /// The event <see cref="RecognitionErrorRaised"/> signals that an error occurred during recognition.
    /// </summary>
    public EventHandlerImpl<RecognitionErrorEventArgs> RecognitionErrorRaised;

    public IntentRecognizer(Carbon.Internal.IntentRecognizer recoImpl)
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

    /// <summary>
    /// Gets/sets the spoken language of recognition.
    /// </summary>
    public String getLanguage()
    {
            return _Parameters.getString(ParameterNames.SpeechRecognitionLanguage);
        }

    public void setLanguage(String value)
    {
            _Parameters.set(ParameterNames.SpeechRecognitionLanguage, value);
    }

    /// <summary>
    /// The collection of parameters and their values defined for this <see cref="IntentRecognizer"/>.
    /// </summary>
    public ParameterCollection<IntentRecognizer> getParameters()
    {
        return _Parameters;
    }// { get; }
    private ParameterCollection<IntentRecognizer> _Parameters;

    /// <summary>
    /// Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
    /// </summary>
    /// <returns>A task representing the recognition operation. The task returns a value of <see cref="IntentRecognitionResult"/></returns>
    public Task<IntentRecognitionResult> recognizeAsync()
    {
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

    /// <summary>
    /// Starts speech recognition on a continous audio stream, until StopContinuousRecognitionAsync() is called.
    /// User must subscribe to events to receive recognition results.
    /// </summary>
    /// <returns>A task representing the asynchronous operation that starts the recognition.</returns>
    public Task<?> startContinuousRecognitionAsync()
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

    /// <summary>
    /// Stops continuous intent recognition.
    /// </summary>
    /// <returns>A task representing the asynchronous operation that stops the recognition.</returns>
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

    /// <summary>
    /// Adds a phrase that should be recognized as intent.
    /// </summary>
    /// <param name="intentId">A String that represents the identifier of the intent to be recognized.</param>
    /// <param name="phrase">A String that specifies the phrase representing the intent.</param>
    public void addIntent(String intentId, String phrase)
    {
        recoImpl.addIntent(intentId, phrase);
    }

    /// <summary>
    /// Adds an intent from Language Understanding service for recognition.
    /// </summary>
    /// <param name="intentId">A String that represents the identifier of the intent to be recognized.</param>
    /// <param name="model">The intent model from Language Understanding service.</param>
    /// <param name="intentName">The intent name defined in the intent model. If it is null, all intent names defined in the model will be added.</param>
    public void addIntent(String intentId, IntentModel model, String intentName)
    {
        IntentTrigger trigger = Carbon.Internal.IntentTrigger.from(model.modelImpl, intentName);
        recoImpl.addIntent(intentId, trigger);
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

    private boolean disposed = false;
    private final Carbon.Internal.IntentRecognizer recoImpl;
    private IntentHandlerImpl intermediateResultHandler;
    private IntentHandlerImpl finalResultHandler;
    private ErrorHandlerImpl errorHandler;

    // Defines an internal class to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class IntentHandlerImpl extends Carbon.Internal.IntentEventListener
    {
        public IntentHandlerImpl(IntentRecognizer recognizer, boolean isFinalResultHandler)
        {
            this.recognizer = recognizer;
            this.isFinalResultHandler = isFinalResultHandler;
        }

        @Override
        public void execute(Carbon.Internal.IntentRecognitionEventArgs eventArgs)
        {
            IntentRecognitionResultEventArgs resultEventArg = new IntentRecognitionResultEventArgs(eventArgs);
            EventHandlerImpl<IntentRecognitionResultEventArgs> handler = isFinalResultHandler ? recognizer.FinalResultReceived : recognizer.IntermediateResultReceived;
            if (handler != null)
            {
                handler.fireEvent(this, resultEventArg);
            }
        }

        private IntentRecognizer recognizer;
        private boolean isFinalResultHandler;
    }

    // Defines an internal class to raise a C# event for error during recognition when a corresponding callback is invoked by the native layer.
    private class ErrorHandlerImpl extends Carbon.Internal.IntentEventListener
    {
        public ErrorHandlerImpl(IntentRecognizer recognizer)
        {
            this.recognizer = recognizer;
        }

        @Override
        public void execute(Carbon.Internal.IntentRecognitionEventArgs eventArgs)
        {
            RecognitionErrorEventArgs resultEventArg = new RecognitionErrorEventArgs(eventArgs.getSessionId(), eventArgs.getResult().getReason());
            EventHandlerImpl<RecognitionErrorEventArgs>  handler = this.recognizer.RecognitionErrorRaised;

            if (handler != null)
            {
                handler.fireEvent(this, resultEventArg);
            }
        }

        private IntentRecognizer recognizer;
    }
}
