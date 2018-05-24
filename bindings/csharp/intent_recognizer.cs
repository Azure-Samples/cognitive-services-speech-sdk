//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace Microsoft.CognitiveServices.Speech.Intent
{
    /// <summary>
    /// Perform intent recognition on the speech input. It returns both recognized text and recognized intent.
    /// </summary>
    public sealed class IntentRecognizer : Recognizer
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

            Parameters = new RecognizerParametersImpl(recoImpl.Parameters);
        }

        internal IntentRecognizer(Internal.IntentRecognizer recoImpl, AudioInputStream stream) : this(recoImpl)
        {
            streamInput = stream;
        }

        /// <summary>
        /// Gets the language name that was set when the recognizer was created.
        /// </summary>
        public string Language
        {
            get
            {
                return Parameters.Get<string>(SpeechParameterNames.RecognitionLanguage);
            }
        }

        /// <summary>
        /// The collection of parameters and their values defined for this <see cref="IntentRecognizer"/>.
        /// </summary>
        public IRecognizerParameters Parameters { get; internal set; }

        /// <summary>
        /// Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
        /// </summary>
        /// <returns>A task representing the recognition operation. The task returns a value of <see cref="IntentRecognitionResult"/></returns>
        public Task<IntentRecognitionResult> RecognizeAsync()
        {
            return Task.Run(() => { return new IntentRecognitionResult(this.recoImpl.Recognize()); });
        }

        /// <summary>
        /// Starts speech recognition on a continuous audio stream, until StopContinuousRecognitionAsync() is called.
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
        /// Starts speech recognition on a continuous audio stream with keyword spotting, until StopKeywordRecognitionAsync() is called.
        /// User must subscribe to events to receive recognition results.
        /// Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
        /// </summary>
        /// <param name="model">The keyword recognition model that specifies the keyword to be recognized.</param>
        /// <returns>A task representing the asynchronous operation that starts the recognition.</returns>
        public Task StartKeywordRecognitionAsync(KeywordRecognitionModel model)
        {
            return Task.Run(() => { this.recoImpl.StartKeywordRecognition(model.modelImpl); });
        }

        /// <summary>
        /// Stops continuous speech recognition with keyword spotting.
        /// Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that stops the recognition.</returns>
        public Task StopKeywordRecognitionAsync()
        {
            return Task.Run(() => { this.recoImpl.StopKeywordRecognition(); });
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
                disposed = true;
                base.Dispose(disposing);
            }
        }

        private bool disposed = false;
        internal readonly Internal.IntentRecognizer recoImpl;
        private readonly IntentHandlerImpl intermediateResultHandler;
        private readonly IntentHandlerImpl finalResultHandler;
        private readonly ErrorHandlerImpl errorHandler;
        private readonly AudioInputStream streamInput;

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
                var resultEventArg = new RecognitionErrorEventArgs(eventArgs.SessionId, eventArgs.Result.Reason, eventArgs.Result.ErrorDetails);
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
