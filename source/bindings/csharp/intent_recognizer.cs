//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using System.Runtime.InteropServices;

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
        public event EventHandler<IntentRecognitionEventArgs> Recognizing;

        /// <summary>
        /// The event <see cref="Recognized"/> signals that a final recognition result is received.
        /// </summary>
        public event EventHandler<IntentRecognitionEventArgs> Recognized;

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
            : this(Internal.IntentRecognizer.FromConfig(config, audioConfig))
        {
        }

        internal IntentRecognizer(Internal.IntentRecognizer recoImpl) : base(recoImpl)
        {
            this.recoImpl = recoImpl;

            recognizingCallbackDelegate = FireEvent_Recognizing;
            recognizedCallbackDelegate = FireEvent_Recognized;
            canceledCallbackDelegate = FireEvent_Canceled;

            recoImpl.SetRecognizingCallback(recognizingCallbackDelegate, GCHandle.ToIntPtr(gch));
            recoImpl.SetRecognizedCallback(recognizedCallbackDelegate, GCHandle.ToIntPtr(gch));
            recoImpl.SetCanceledCallback(canceledCallbackDelegate, GCHandle.ToIntPtr(gch));

            Properties = new PropertyCollection(recoImpl.Properties);
        }

        /// <summary>
        /// Gets the language name that is used for recognition.
        /// </summary>
        public string SpeechRecognitionLanguage
        {
            get
            {
                return Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
            }
        }

        /// <summary>
        /// Gets/sets authorization token used to communicate with the service.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expires, the caller needs to refresh it by calling this setter with a new valid token.
        /// Otherwise, the recognizer will encounter errors during recognition.
        /// </summary>
        public string AuthorizationToken
        {
            get
            {
                return this.recoImpl.AuthorizationToken;
            }

            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException(nameof(value));
                }

                this.recoImpl.AuthorizationToken = value;
            }
        }

        /// <summary>
        /// Gets the collection or properties and their values defined for this <see cref="IntentRecognizer"/>.
        /// </summary>
        public PropertyCollection Properties { get; internal set; }

        /// <summary>
        /// Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
        /// Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
        /// </summary>
        /// <returns>A task representing the recognition operation. The task returns a value of <see cref="IntentRecognitionResult"/></returns>
        public Task<IntentRecognitionResult> RecognizeOnceAsync()
        {
            return Task.Run(() =>
            {
                IntentRecognitionResult result = null;
                base.DoAsyncRecognitionAction(() => result = new IntentRecognitionResult(this.recoImpl.RecognizeOnce()));
                return result;
            });
        }

        /// <summary>
        /// Starts speech recognition on a continuous audio stream, until StopContinuousRecognitionAsync() is called.
        /// User must subscribe to events to receive recognition results.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that starts the recognition.</returns>
        public Task StartContinuousRecognitionAsync()
        {
            return Task.Run(() =>
            {
                base.DoAsyncRecognitionAction(this.recoImpl.StartContinuousRecognition);
            });
        }

        /// <summary>
        /// Stops continuous intent recognition.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that stops the recognition.</returns>
        public Task StopContinuousRecognitionAsync()
        {
            return Task.Run(() =>
            {
                base.DoAsyncRecognitionAction(this.recoImpl.StopContinuousRecognition);
            });
        }

        /// <summary>
        /// Starts speech recognition on a continuous audio stream with keyword spotting, until StopKeywordRecognitionAsync() is called.
        /// User must subscribe to events to receive recognition results.
        /// </summary>
        /// <param name="model">The keyword recognition model that specifies the keyword to be recognized.</param>
        /// <returns>A task representing the asynchronous operation that starts the recognition.</returns>
        public Task StartKeywordRecognitionAsync(KeywordRecognitionModel model)
        {
            return Task.Run(() =>
            {
                base.DoAsyncRecognitionAction(() => this.recoImpl.StartKeywordRecognition(model.modelImpl));
            });
        }

        /// <summary>
        /// Stops continuous speech recognition with keyword spotting.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that stops the recognition.</returns>
        public Task StopKeywordRecognitionAsync()
        {
            return Task.Run(() =>
            {
                base.DoAsyncRecognitionAction(this.recoImpl.StopKeywordRecognition);
            });
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
            recoImpl.AddIntent(model.modelImpl, intentName);
        }

        /// <summary>
        /// Adds a single intent by name from the specified Language Understanding Model.
        /// </summary>
        /// <param name="model">The language understanding model containing the intent.</param>
        /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
        /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
        public void AddIntent(LanguageUnderstandingModel model, string intentName, string intentId)
        {
            recoImpl.AddIntent(model.modelImpl, intentName, intentId);
        }

        /// <summary>
        /// Adds all intents from the specified Language Understanding Model.
        /// </summary>
        /// <param name="model">The language understanding model from Language Understanding service.</param>
        /// <param name="intentId">A custom string id to be returned in the IntentRecognitionResult's IntentId property.</param>
        public void AddAllIntents(LanguageUnderstandingModel model, string intentId)
        {
            recoImpl.AddAllIntents(model.modelImpl, intentId);
        }

        /// <summary>
        /// Adds all intents from the specified Language Understanding Model.
        /// </summary>
        /// <param name="model">The language understanding model from Language Understanding service.</param>
        public void AddAllIntents(LanguageUnderstandingModel model)
        {
            recoImpl.AddAllIntents(model.modelImpl);
        }

        protected override void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                try
                {
                    recoImpl.SetRecognizingCallback(null, GCHandle.ToIntPtr(gch));
                    recoImpl.SetRecognizedCallback(null, GCHandle.ToIntPtr(gch));
                    recoImpl.SetCanceledCallback(null, GCHandle.ToIntPtr(gch));
                    recoImpl.SetSessionStartedCallback(null, GCHandle.ToIntPtr(gch));
                    recoImpl.SetSessionStoppedCallback(null, GCHandle.ToIntPtr(gch));
                    recoImpl.SetSpeechStartDetectedCallback(null, GCHandle.ToIntPtr(gch));
                    recoImpl.SetSpeechEndDetectedCallback(null, GCHandle.ToIntPtr(gch));
                }
                catch (ApplicationException e)
                {
                    Internal.SpxExceptionThrower.LogError(e.Message);
                }

                recoImpl?.Dispose();

                recognizingCallbackDelegate = null;
                recognizedCallbackDelegate = null;
                canceledCallbackDelegate = null;

                base.Dispose(disposing);
            }
        }

        private new readonly Internal.IntentRecognizer recoImpl;
        private Internal.CallbackFunctionDelegate recognizingCallbackDelegate;
        private Internal.CallbackFunctionDelegate recognizedCallbackDelegate;
        private Internal.CallbackFunctionDelegate canceledCallbackDelegate;

        private readonly Audio.AudioConfig audioConfig;

        // Defines a private methods to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_Recognizing(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                EventHandler<IntentRecognitionEventArgs> eventHandle;
                IntentRecognizer recognizer = (IntentRecognizer)GCHandle.FromIntPtr(pvContext).Target;
                lock (recognizer.recognizerLock)
                {
                    if (recognizer.isDisposing) return;
                    eventHandle = recognizer.Recognizing;
                }
                var eventArgs = new Internal.IntentRecognitionEventArgs(hevent);
                var resultEventArg = new IntentRecognitionEventArgs(eventArgs);
                eventHandle?.Invoke(recognizer, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                Internal.SpxExceptionThrower.LogError(Internal.SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_Recognized(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                EventHandler<IntentRecognitionEventArgs> eventHandle;
                IntentRecognizer recognizer = (IntentRecognizer)GCHandle.FromIntPtr(pvContext).Target;
                lock (recognizer.recognizerLock)
                {
                    if (recognizer.isDisposing) return;
                    eventHandle = recognizer.Recognized;
                }
                var eventArgs = new Internal.IntentRecognitionEventArgs(hevent);
                var resultEventArg = new IntentRecognitionEventArgs(eventArgs);
                eventHandle?.Invoke(recognizer, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                Internal.SpxExceptionThrower.LogError(Internal.SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_Canceled(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                EventHandler<IntentRecognitionCanceledEventArgs> eventHandle;
                IntentRecognizer recognizer = (IntentRecognizer)GCHandle.FromIntPtr(pvContext).Target;
                lock (recognizer.recognizerLock)
                {
                    if (recognizer.isDisposing) return;
                    eventHandle = recognizer.Canceled;
                }
                var eventArgs = new Internal.IntentRecognitionCanceledEventArgs(hevent);
                var resultEventArg = new IntentRecognitionCanceledEventArgs(eventArgs);
                eventHandle?.Invoke(recognizer, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                Internal.SpxExceptionThrower.LogError(Internal.SpxError.InvalidHandle);
            }
        }
    }
}
