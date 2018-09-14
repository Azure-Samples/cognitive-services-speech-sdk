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
        /// The event <see cref="Recognizing"/> signals that an intermediate recognition result is received.
        /// </summary>
        public event EventHandler<IntentRecognitionResultEventArgs> Recognizing;

        /// <summary>
        /// The event <see cref="Recognized"/> signals that a final recognition result is received.
        /// </summary>
        public event EventHandler<IntentRecognitionResultEventArgs> Recognized;

        /// <summary>
        /// The event <see cref="Canceled"/> signals that the intent recognition was canceled.
        /// </summary>
        public event EventHandler<IntentRecognitionCanceledEventArgs> Canceled;

        /// <summary>
        /// Creates a new instance of IntentRecognizer.
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        public IntentRecognizer(SpeechConfig speechConfig)
            : this(speechConfig != null ? speechConfig.configImpl : throw new ArgumentNullException(nameof(speechConfig)), null)
        {
        }

        /// <summary>
        /// Creates a new instance of IntentRecognizer.
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        /// <param name="audioConfig">Audio configuration</param>
        public IntentRecognizer(SpeechConfig speechConfig, Audio.AudioConfig audioConfig)
            : this(speechConfig != null ? speechConfig.configImpl : throw new ArgumentNullException(nameof(speechConfig)),
                   audioConfig != null ? audioConfig.configImpl : throw new ArgumentNullException(nameof(audioConfig)))
        {
            this.audioConfig = audioConfig;
        }

        internal IntentRecognizer(Internal.SpeechConfig config, Internal.AudioConfig audioConfig)
        {
            this.recoImpl = Internal.IntentRecognizer.FromConfig(config, audioConfig);

            recognizingHandler = new IntentHandlerImpl(this, isRecognizedHandler: false);
            recoImpl.Recognizing.Connect(recognizingHandler);

            recognizedHandler = new IntentHandlerImpl(this, isRecognizedHandler: true);
            recoImpl.Recognized.Connect(recognizedHandler);

            canceledHandler = new CanceledHandlerImpl(this);
            recoImpl.Canceled.Connect(canceledHandler);

            recoImpl.SessionStarted.Connect(sessionStartedHandler);
            recoImpl.SessionStopped.Connect(sessionStoppedHandler);
            recoImpl.SpeechStartDetected.Connect(speechStartDetectedHandler);
            recoImpl.SpeechEndDetected.Connect(speechEndDetectedHandler);

            Parameters = new PropertyCollectionImpl(recoImpl.Parameters);
        }

        /// <summary>
        /// Gets the language name that is used for recognition.
        /// </summary>
        public string SpeechRecognitionLanguage
        {
            get
            {
                return Parameters.Get(PropertyId.SpeechServiceConnection_IntentSourceLanguage);
            }
        }

        /// <summary>
        /// Gets/sets authorization token used to communicate with the service.
        /// </summary>
        public string AuthorizationToken
        {
            get
            {
                return this.recoImpl.GetAuthorizationToken();
            }

            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException(nameof(value));
                }

                this.recoImpl.SetAuthorizationToken(value);
            }
        }

        /// <summary>
        /// Gets the collection of parameters and their values defined for this <see cref="IntentRecognizer"/>.
        /// </summary>
        public IPropertyCollection Parameters { get; internal set; }

        /// <summary>
        /// Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
        /// Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
        /// </summary>
        /// <returns>A task representing the recognition operation. The task returns a value of <see cref="IntentRecognitionResult"/></returns>
        public Task<IntentRecognitionResult> RecognizeOnceAsync()
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
        /// Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
        /// </summary>
        /// <param name="simplePhrase">The phrase corresponding to the intent.</param>
        /// <remarks>Once recognized, the IntentRecognitionResult's IntentId property will match the simplePhrase specified here.</remarks>
        public void AddIntent(string simplePhrase)
        {
            recoImpl.AddIntent(simplePhrase);
        }

        /// <summary>
        /// Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
        /// </summary>
        /// <param name="simplePhrase">The phrase corresponding to the intent.</param>
        /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
        /// <remarks>Once recognized, the result's intent id will match the id supplied here.</remarks>
        public void AddIntent(string simplePhrase, string intentId)
        {
            recoImpl.AddIntent(simplePhrase, intentId);
        }

        /// <summary>
        /// Adds a single intent by name from the specified Language Understanding Model.
        /// </summary>
        /// <param name="model">The language understanding model containing the intent.</param>
        /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
        /// <remarks>Once recognized, the IntentRecognitionResult's IntentId property will contain the intentName specified here.</remarks>
        public void AddIntent(LanguageUnderstandingModel model, string intentName)
        {
            var trigger = Microsoft.CognitiveServices.Speech.Internal.IntentTrigger.From(model.modelImpl, intentName);
            recoImpl.AddIntent(trigger, intentName);
        }

        /// <summary>
        /// Adds a single intent by name from the specified Language Understanding Model.
        /// </summary>
        /// <param name="model">The language understanding model containing the intent.</param>
        /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
        /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
        public void AddIntent(LanguageUnderstandingModel model, string intentName, string intentId)
        {
            var trigger = Microsoft.CognitiveServices.Speech.Internal.IntentTrigger.From(model.modelImpl, intentName);
            recoImpl.AddIntent(trigger, intentId);
        }

        /// <summary>
        /// Adds all intents from the specified Language Understanding Model.
        /// </summary>
        /// <param name="model">The language understanding model from Language Understanding service.</param>
        /// <param name="intentId">A custom string id to be returned in the IntentRecognitionResult's IntentId property.</param>
        public void AddAllIntents(LanguageUnderstandingModel model, string intentId)
        {
            var trigger = Microsoft.CognitiveServices.Speech.Internal.IntentTrigger.From(model.modelImpl);
            recoImpl.AddIntent(trigger, intentId);
        }

        protected override void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                recoImpl.Recognizing.Disconnect(recognizingHandler);
                recoImpl.Recognized.Disconnect(recognizedHandler);
                recoImpl.Canceled.Disconnect(canceledHandler);
                recoImpl.SessionStarted.Disconnect(sessionStartedHandler);
                recoImpl.SessionStopped.Disconnect(sessionStoppedHandler);
                recoImpl.SpeechStartDetected.Disconnect(speechStartDetectedHandler);
                recoImpl.SpeechEndDetected.Disconnect(speechEndDetectedHandler);

                recognizingHandler?.Dispose();
                recognizedHandler?.Dispose();
                canceledHandler?.Dispose();
                recoImpl?.Dispose();
                disposed = true;
                base.Dispose(disposing);
            }
        }

        private bool disposed = false;
        internal readonly Internal.IntentRecognizer recoImpl;
        private readonly IntentHandlerImpl recognizingHandler;
        private readonly IntentHandlerImpl recognizedHandler;
        private readonly CanceledHandlerImpl canceledHandler;
        private readonly Audio.AudioConfig audioConfig;

        // Defines an internal class to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.
        private class IntentHandlerImpl : Internal.IntentEventListener
        {
            public IntentHandlerImpl(IntentRecognizer recognizer, bool isRecognizedHandler)
            {
                this.recognizer = recognizer;
                this.isRecognizedHandler = isRecognizedHandler;
            }

            public override void Execute(Internal.IntentRecognitionEventArgs eventArgs)
            {
                var resultEventArg = new IntentRecognitionResultEventArgs(eventArgs);
                var handler = isRecognizedHandler ? recognizer.Recognized : recognizer.Recognizing;
                if (handler != null)
                {
                    handler(this, resultEventArg);
                }
            }

            private IntentRecognizer recognizer;
            private bool isRecognizedHandler;
        }

        // Defines an internal class to raise a C# event for error during recognition when a corresponding callback is invoked by the native layer.
        private class CanceledHandlerImpl : Internal.IntentCanceledEventListener
        {
            public CanceledHandlerImpl(IntentRecognizer recognizer)
            {
                this.recognizer = recognizer;
            }

            public override void Execute(Microsoft.CognitiveServices.Speech.Internal.IntentRecognitionCanceledEventArgs eventArgs)
            {
                if (recognizer.disposed)
                {
                    return;
                }

                var canceledEventArgs = new IntentRecognitionCanceledEventArgs(eventArgs);
                var handler = this.recognizer.Canceled;

                if (handler != null)
                {
                    handler(this, canceledEventArgs);
                }
            }

            private IntentRecognizer recognizer;
        }
    }
}
