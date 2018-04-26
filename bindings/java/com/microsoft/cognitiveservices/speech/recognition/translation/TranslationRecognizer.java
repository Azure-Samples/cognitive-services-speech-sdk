/**
 * 
 */
package com.microsoft.cognitiveservices.speech.recognition.translation;

import java.io.IOException;
import java.util.ArrayList;

import com.microsoft.cognitiveservices.speech.ParameterCollection;
import com.microsoft.cognitiveservices.speech.recognition.ParameterNames;
import com.microsoft.cognitiveservices.speech.recognition.RecognitionErrorEventArgs;
import com.microsoft.cognitiveservices.speech.recognition.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Task;
import com.microsoft.cognitiveservices.speech.util.TaskRunner;

//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

 /**
   * Performs translation on the speech input.
   */
 public final class TranslationRecognizer extends com.microsoft.cognitiveservices.speech.recognition.Recognizer
 {
     /**
       * The event IntermediateResultReceived signals that an intermediate recognition result is received.
       */
     public final EventHandlerImpl<TranslationTextResultEventArgs> IntermediateResultReceived = new EventHandlerImpl<TranslationTextResultEventArgs>();

     /**
       * The event FinalResultReceived signals that a final recognition result is received.
       */
     public final EventHandlerImpl<TranslationTextResultEventArgs> FinalResultReceived = new EventHandlerImpl<TranslationTextResultEventArgs>();

     /**
       * The event RecognitionErrorRaised signals that an error occurred during recognition.
       */
     public final EventHandlerImpl<RecognitionErrorEventArgs> RecognitionErrorRaised = new EventHandlerImpl<RecognitionErrorEventArgs>();

     /**
       * The event SynthesisResultReceived signals that a translation synthesis result is received.
       */
     public final EventHandlerImpl<TranslationSynthesisResultEventArgs> SynthesisResultReceived = new EventHandlerImpl<TranslationSynthesisResultEventArgs>();

     TranslationRecognizer(com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer recoImpl)
     {
         this.recoImpl = recoImpl;

         intermediateResultHandler = new ResultHandlerImpl(this, /*isFinalResultHandler:*/ false);
         recoImpl.getIntermediateResult().addEventListener(intermediateResultHandler);

         finalResultHandler = new ResultHandlerImpl(this, /*isFinalResultHandler:*/ true);
         recoImpl.getFinalResult().addEventListener(finalResultHandler);

         synthesisResultHandler = new SynthesisHandlerImpl(this);
         recoImpl.getTranslationSynthesisResultEvent().addEventListener(synthesisResultHandler);

         errorHandler = new ErrorHandlerImpl(this);
         recoImpl.getNoMatch().addEventListener(errorHandler);
         recoImpl.getCanceled().addEventListener(errorHandler);

         recoImpl.getSessionStarted().addEventListener(sessionStartedHandler);
         recoImpl.getSessionStopped().addEventListener(sessionStoppedHandler);
         recoImpl.getSpeechStartDetected().addEventListener(speechStartDetectedHandler);
         recoImpl.getSpeechEndDetected().addEventListener(speechEndDetectedHandler);

         _Parameters = new ParameterCollection<TranslationRecognizer>(this);
     }

     /**
       * Gets the language name that was set when the recognizer was created.
       * @return Gets the language name that was set when the recognizer was created.
       */
     public String getSourceLanguage()
     {
         return _Parameters.getString(ParameterNames.TranslationFromLanguage);
     }

     /**
       * Gets target languages for translation that were set when the recognizer was created.
       * The language is specified in BCP-47 format. The translation will provide translated text for each of language.
       * @return Gets target languages for translation that were set when the recognizer was created.
       */
     public ArrayList<String> TargetLanguages() {
         return new ArrayList<String>(_TargetLanguages);
     }     // { get; }
     ArrayList<String> _TargetLanguages = new ArrayList<String>();

     /**
       * Gets a boolean value which indicates whether a voice output of the translated text is desired.
       * @return Gets a boolean value which indicates whether a voice output of the translated text is desired.
       */
     public boolean getIsVoiceOutputDesired() {
         return _IsVoiceOutputDesired;
     }//     { get; set; }
     
     /**
      * Sets a boolean value which indicates whether a voice output of the translated text is desired.
      * @param value The new value for the voice flag.
      */
     public void setIsVoiceOutputDesired(boolean value) {
         _IsVoiceOutputDesired = value;
     }//     { get; set; }
     private boolean _IsVoiceOutputDesired;

     /**
       * The collection of parameters and their values defined for this TranslationRecognizer.
       * @return The collection of parameters and their values defined for this TranslationRecognizer.
       */
     public ParameterCollection<TranslationRecognizer> getParameters() {
         return _Parameters;
     }// { get; }
     private ParameterCollection<TranslationRecognizer> _Parameters;

     /**
       * Starts recognition and translation, and stops after the first utterance is recognized. The task returns the translation text as result.
       * @return A task representing the recognition operation. The task returns a value of TranslationTextResult.
       */
     public Task<TranslationTextResult> RecognizeAsync()
     {
         Task<TranslationTextResult> t = new Task<TranslationTextResult>(new TaskRunner<TranslationTextResult>() {
             TranslationTextResult result;
             
             @Override
             public void run() {
                 result = new TranslationTextResult(recoImpl.recognize());
             }

             @Override
             public TranslationTextResult result() {
                 return result;
             }});
         
         return t;
     }

     /**
       * Starts recognition and translation on a continuous audio stream, until StopContinuousRecognitionAsync() is called.
       * User must subscribe to events to receive translation results.
       * @return A task representing the asynchronous operation that starts the recognition.
       */
     public Task<?> StartContinuousRecognitionAsync()
     {
         Task<Object> t = new Task<Object>(new TaskRunner<Object>() {
             IntentRecognitionResult result;
             
             @Override
             public void run() {
                 recoImpl.startContinuousRecognitionAsync();
             }

             @Override
             public Object result() {
                 return result;
             }});
         
         return t;
     }

     /**
       * Stops continuous recognition and translation.
       * @return A task representing the asynchronous operation that stops the translation.
       */
     public Task<?> StopContinuousRecognitionAsync()
     {
         Task<Object> t = new Task<Object>(new TaskRunner<Object>() {
             IntentRecognitionResult result;
             
             @Override
             public void run() {
                 recoImpl.stopContinuousRecognitionAsync();
             }

             @Override
             public Object result() {
                 return result;
             }});
         
         return t;
     }

    /**
      * Starts speech recognition on a continous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
      * User must subscribe to events to receive recognition results.
      * @param keyword The keyword to recognize.
      * @return A task representing the asynchronous operation that starts the recognition.
      */
    public Task<?> startKeywordRecognitionAsync(String keyword)
    {
        Task<?> t = new Task(new TaskRunner() {

            @Override
            public void run() {
                recoImpl.startKeywordRecognitionAsync(keyword);
            }

            @Override
            public Object result() {
                return null;
            }});
        
        return t;
    }

    /**
      * Stops continuous speech recognition.
      * @return A task representing the asynchronous operation that stops the recognition.
      */
    public Task<?> stopKeywordRecognitionAsync()
    {
        Task<?> t = new Task(new TaskRunner() {

            @Override
            public void run() {
                recoImpl.stopKeywordRecognitionAsync();
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
             recoImpl.getSpeechStartDetected().removeEventListener(speechStartDetectedHandler);
             recoImpl.getSpeechEndDetected().removeEventListener(speechEndDetectedHandler);
             recoImpl.getTranslationSynthesisResultEvent().removeEventListener(synthesisResultHandler);

             intermediateResultHandler.delete();
             finalResultHandler.delete();
             errorHandler.delete();
             recoImpl.delete();
             _Parameters.close();
             disposed = true;
             super.dispose(disposing);
         }
     }

     public com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer getRecoImpl() {
         return recoImpl;
     }
     
     private final com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer recoImpl;
     private ResultHandlerImpl intermediateResultHandler;
     private ResultHandlerImpl finalResultHandler;
     private SynthesisHandlerImpl synthesisResultHandler;
     private ErrorHandlerImpl errorHandler;
     private boolean disposed = false;

     // Defines an internal class to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.
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

             TranslationTextResultEventArgs resultEventArg = new TranslationTextResultEventArgs(eventArgs);
             EventHandlerImpl<TranslationTextResultEventArgs>  handler = isFinalResultHandler ? recognizer.FinalResultReceived : recognizer.IntermediateResultReceived;
             if (handler != null)
             {
                 handler.fireEvent(this.recognizer, resultEventArg);
             }
         }

         private TranslationRecognizer recognizer;
         private boolean isFinalResultHandler;
     }

     // Defines an internal class to raise a C# event for error during recognition when a corresponding callback is invoked by the native layer.
     class ErrorHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.TranslationTexEventListener {
         public ErrorHandlerImpl(TranslationRecognizer recognizer) {
             this.recognizer = recognizer;
         }

         @Override
         public void execute(com.microsoft.cognitiveservices.speech.internal.TranslationTextResultEventArgs eventArgs)
         {
             if (recognizer.disposed)
             {
                 return;
             }

             RecognitionErrorEventArgs resultEventArg = null; // new RecognitionErrorEventArgs(eventArgs.SessionId, eventArgs.Result.Reason);
             EventHandlerImpl<RecognitionErrorEventArgs> handler = this.recognizer.RecognitionErrorRaised;

             if (handler != null)
             {
                 handler.fireEvent(this.recognizer, resultEventArg);
             }
         }

         private TranslationRecognizer recognizer;
     }

     // Defines an internal class to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.
     private class SynthesisHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisEventListener
     {
         public SynthesisHandlerImpl(TranslationRecognizer recognizer)
         {
             this.recognizer = recognizer;
         }

             
         @Override
         public void execute(com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResultEventArgs eventArgs)
         {
             if (recognizer.disposed)
             {
                 return;
             }

             TranslationSynthesisResultEventArgs resultEventArg = new TranslationSynthesisResultEventArgs(eventArgs);
             EventHandlerImpl<TranslationSynthesisResultEventArgs> handler = recognizer.SynthesisResultReceived;
             if (handler != null)
             {
                 handler.fireEvent(this.recognizer, resultEventArg);
             }
         }

         private TranslationRecognizer recognizer;
     }
 }
