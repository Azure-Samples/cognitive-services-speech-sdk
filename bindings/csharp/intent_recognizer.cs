//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace Microsoft.CognitiveServices.Speech.Recognition.Intent
{
    /// <summary>
    /// Perform intent recognition on the speech input. It returns both recognized text and recognized intent.
    /// </summary>
    public sealed class IntentRecognizer : Recognition.Recognizer
    {
        /// <summary>
        /// The event <see cref="IntermediateResultReceived"/> signals that an intermediate recognition result is received.
        /// </summary>
        public event EventHandler<IntentRecognitionResultEventArgs> IntermediateResultReceived;

        /// <summary>
        /// The event <see cref="FinalResultReceived"/> signals that a final recognition result is received.
        /// </summary>
        public event EventHandler<IntentRecognitionResultEventArgs> FinalResultReceived;

        /// <summary>
        /// The event <see cref="RecognitionErrorRaised"/> signals that an error occurred during recognition.
        /// </summary>
        public event EventHandler<RecognitionErrorEventArgs> RecognitionErrorRaised;

        internal IntentRecognizer(Internal.IntentRecognizer recoImpl)
        {
            this.recoImpl = recoImpl;

            intermediateResultHandler = new IntentHandlerImpl(this, isFinalResultHandler: false);
            recoImpl.IntermediateResult.Connect(intermediateResultHandler);

            finalResultHandler = new IntentHandlerImpl(this, isFinalResultHandler: true);
            recoImpl.FinalResult.Connect(finalResultHandler);

            errorHandler = new ErrorHandlerImpl(this);
            recoImpl.NoMatch.Connect(errorHandler);
            recoImpl.Canceled.Connect(errorHandler);

            recoImpl.SessionStarted.Connect(sessionStartedHandler);
            recoImpl.SessionStopped.Connect(sessionStoppedHandler);
            recoImpl.SpeechStartDetected.Connect(speechStartDetectedHandler);
            recoImpl.SpeechEndDetected.Connect(speechEndDetectedHandler);
        }

        /// <summary>
        /// Gets the language name that was set when the recognizer was created.
        /// </summary>
        public string Language
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.SpeechRecognitionLanguage);
            }
        }

        /// <summary>
        /// The collection of parameters and their values defined for this <see cref="IntentRecognizer"/>.
        /// </summary>
        public ParameterCollection<IntentRecognizer> Parameters { get; }

        /// <summary>
        /// Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
        /// </summary>
        /// <returns>A task representing the recognition operation. The task returns a value of <see cref="IntentRecognitionResult"/></returns>
        public Task<IntentRecognitionResult> RecognizeAsync()
        {
            return Task.Run(() => { return new IntentRecognitionResult(this.recoImpl.Recognize()); });
        }

        /// <summary>
        /// Starts speech recognition on a continous audio stream, until StopContinuousRecognitionAsync() is called.
        /// User must subscribe to events to receive recognition results.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that starts the recognition.</returns>
        public Task StartContinuousRecognitionAsync()
        {
            return Task.Run(() => { this.recoImpl.StartContinuousRecognition(); });
        }

        /// <summary>
        /// Stops continuous intent recognition.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that stops the recognition.</returns>
        public Task StopContinuousRecognitionAsync()
        {
            return Task.Run(() => { this.recoImpl.StopContinuousRecognition(); });
        }

        /// <summary>
        /// Adds a phrase that should be recognized as intent.
        /// </summary>
        /// <param name="intentId">A string that represents the identifier of the intent to be recognized.</param>
        /// <param name="phrase">A string that specifies the phrase representing the intent.</param>
        public void AddIntent(string intentId, string phrase)
        {
            recoImpl.AddIntent(intentId, phrase);
        }

        /// <summary>
        /// Adds an intent from Language Understanding service for recognition.
        /// </summary>
        /// <param name="intentId">A string that represents the identifier of the intent to be recognized.</param>
        /// <param name="model">The language understanding model from Language Understanding service.</param>
        /// <param name="intentName">The intent name defined in the language understanding model. If it is null, all intent names defined in the model will be added.</param>
        public void AddIntent(string intentId, LanguageUnderstandingModel model, string intentName = null)
        {
            var trigger = Microsoft.CognitiveServices.Speech.Internal.IntentTrigger.From(model.modelImpl, intentName);
            recoImpl.AddIntent(intentId, trigger);
        }
        
        protected override void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                recoImpl.IntermediateResult.Disconnect(intermediateResultHandler);
                recoImpl.FinalResult.Disconnect(finalResultHandler);
                recoImpl.NoMatch.Disconnect(errorHandler);
                recoImpl.Canceled.Disconnect(errorHandler);
                recoImpl.SessionStarted.Disconnect(sessionStartedHandler);
                recoImpl.SessionStopped.Disconnect(sessionStoppedHandler);
                recoImpl.SpeechStartDetected.Disconnect(speechStartDetectedHandler);
                recoImpl.SpeechEndDetected.Disconnect(speechEndDetectedHandler);

                intermediateResultHandler?.Dispose();
                finalResultHandler?.Dispose();
                errorHandler?.Dispose();
                recoImpl?.Dispose();
                Parameters?.Dispose();
                disposed = true;
                base.Dispose(disposing);
            }
        }

        private bool disposed = false;
        internal Internal.IntentRecognizer recoImpl;
        private IntentHandlerImpl intermediateResultHandler;
        private IntentHandlerImpl finalResultHandler;
        private ErrorHandlerImpl errorHandler;

        // Defines an internal class to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.
        private class IntentHandlerImpl : Internal.IntentEventListener
        {
            public IntentHandlerImpl(IntentRecognizer recognizer, bool isFinalResultHandler)
            {
                this.recognizer = recognizer;
                this.isFinalResultHandler = isFinalResultHandler;
            }

            public override void Execute(Internal.IntentRecognitionEventArgs eventArgs)
            {
                var resultEventArg = new IntentRecognitionResultEventArgs(eventArgs);
                var handler = isFinalResultHandler ? recognizer.FinalResultReceived : recognizer.IntermediateResultReceived;
                if (handler != null)
                {
                    handler(this, resultEventArg);
                }
            }

            private IntentRecognizer recognizer;
            private bool isFinalResultHandler;
        }

        // Defines an internal class to raise a C# event for error during recognition when a corresponding callback is invoked by the native layer.
        private class ErrorHandlerImpl : Internal.IntentEventListener
        {
            public ErrorHandlerImpl(IntentRecognizer recognizer)
            {
                this.recognizer = recognizer;
            }

            public override void Execute(Microsoft.CognitiveServices.Speech.Internal.IntentRecognitionEventArgs eventArgs)
            {
                var resultEventArg = new RecognitionErrorEventArgs(eventArgs.SessionId, eventArgs.Result.Reason);
                var handler = this.recognizer.RecognitionErrorRaised;

                if (handler != null)
                {
                    handler(this, resultEventArg);
                }
            }

            private IntentRecognizer recognizer;
        }
    }

    
}
