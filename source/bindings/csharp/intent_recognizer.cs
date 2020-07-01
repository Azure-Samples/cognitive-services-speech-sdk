//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Intent
{
    /// <summary>
    /// Perform intent recognition on the speech input. It returns both recognized text and recognized intent.
    /// </summary>
    public sealed class IntentRecognizer : Recognizer
    {
        private event EventHandler<IntentRecognitionEventArgs> _Recognizing;
        private event EventHandler<IntentRecognitionEventArgs> _Recognized;
        private event EventHandler<IntentRecognitionCanceledEventArgs> _Canceled;


        /// <summary>
        /// The event <see cref="Recognizing"/> signals that an intermediate recognition result is received.
        /// </summary>
        public event EventHandler<IntentRecognitionEventArgs> Recognizing
        {
            add
            {
                if (this._Recognizing == null)
                {
                    ThrowIfFail(Internal.Recognizer.recognizer_recognizing_set_callback(recoHandle, recognizingCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Recognizing += value;
            }
            remove
            {
                this._Recognizing -= value;
                if (this._Recognizing == null)
                {
                    LogErrorIfFail(Internal.Recognizer.recognizer_recognizing_set_callback(recoHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// The event <see cref="Recognized"/> signals that a final recognition result is received.
        /// </summary>
        public event EventHandler<IntentRecognitionEventArgs> Recognized
        {
            add
            {
                if (this._Recognized == null)
                {
                    ThrowIfFail(Internal.Recognizer.recognizer_recognized_set_callback(recoHandle, recognizedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Recognized += value;
            }
            remove
            {
                this._Recognized -= value;
                if (this._Recognized == null)
                {
                    LogErrorIfFail(Internal.Recognizer.recognizer_recognized_set_callback(recoHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// The event <see cref="Canceled"/> signals that the intent recognition was canceled.
        /// </summary>
        public event EventHandler<IntentRecognitionCanceledEventArgs> Canceled
        {
            add
            {
                if (this._Canceled == null)
                {
                    ThrowIfFail(Internal.Recognizer.recognizer_canceled_set_callback(recoHandle, canceledCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Canceled += value;
            }
            remove
            {
                this._Canceled -= value;
                if (this._Canceled == null)
                {
                    LogErrorIfFail(Internal.Recognizer.recognizer_canceled_set_callback(recoHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// Creates a new instance of IntentRecognizer.
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        public IntentRecognizer(SpeechConfig speechConfig)
            : this(FromConfig(SpxFactory.recognizer_create_intent_recognizer_from_config, speechConfig))
        {
        }

        /// <summary>
        /// Creates a new instance of IntentRecognizer.
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        /// <param name="audioConfig">Audio configuration</param>
        public IntentRecognizer(SpeechConfig speechConfig, Audio.AudioConfig audioConfig)
            : this(FromConfig(SpxFactory.recognizer_create_intent_recognizer_from_config, speechConfig, audioConfig))
        {
            this.audioInputKeepAlive = audioConfig;
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        internal IntentRecognizer(InteropSafeHandle recoHandle) : base(recoHandle)
        {
            recognizingCallbackDelegate = FireEvent_Recognizing;
            recognizedCallbackDelegate = FireEvent_Recognized;
            canceledCallbackDelegate = FireEvent_Canceled;

            ThrowIfNull(recoHandle, "Invalid recognizer handle");

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.Recognizer.recognizer_get_property_bag(recoHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
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
                return Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token, "");
            }

            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException(nameof(value));
                }

                Properties.SetProperty(PropertyId.SpeechServiceAuthorization_Token, value);
            }
        }

        /// <summary>
        /// Gets the collection of properties and their values defined for this <see cref="IntentRecognizer"/>.
        /// Note: The property collection is only valid until the recognizer owning this Properties is disposed or finalized.
        /// </summary>
        public PropertyCollection Properties { get; internal set; }

        /// <summary>
        /// Starts speech recognition with intent recognition, and returns after a single utterance is recognized. The end of a
        /// single utterance is determined by listening for silence at the end or until a maximum of 15
        /// seconds of audio is processed.  The task returns the recognition text as result.
        /// Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
        /// shot recognition like command or query.
        /// For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
        /// </summary>
        /// <returns>A task representing the recognition operation. The task returns a value of <see cref="IntentRecognitionResult"/></returns>
        public Task<IntentRecognitionResult> RecognizeOnceAsync()
        {
            return Task.Run(() =>
            {
                IntentRecognitionResult result = null;
                base.DoAsyncRecognitionAction(() => result = new IntentRecognitionResult(RecognizeOnce()));
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
                base.DoAsyncRecognitionAction(StartContinuousRecognition);
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
                base.DoAsyncRecognitionAction(StopContinuousRecognition);
            });
        }

        /// <summary>
        /// Starts speech recognition on a continuous audio stream with keyword spotting, until StopKeywordRecognitionAsync() is called.
        /// User must subscribe to events to receive recognition results.
        /// </summary>
        /// Note: Keyword spotting (KWS) functionality might work with any microphone type, official KWS support, however, is currently limited to the microphone arrays found in the Azure Kinect DK hardware or the Speech Devices SDK.
        /// <param name="model">The keyword recognition model that specifies the keyword to be recognized.</param>
        /// <returns>A task representing the asynchronous operation that starts the recognition.</returns>
        public Task StartKeywordRecognitionAsync(KeywordRecognitionModel model)
        {
            return Task.Run(() =>
            {
                base.DoAsyncRecognitionAction(() => StartKeywordRecognition(model));
            });
        }

        /// <summary>
        /// Stops continuous speech recognition with keyword spotting.
        /// </summary>
        /// Note: Keyword spotting (KWS) functionality might work with any microphone type, official KWS support, however, is currently limited to the microphone arrays found in the Azure Kinect DK hardware or the Speech Devices SDK.
        /// <returns>A task representing the asynchronous operation that stops the recognition.</returns>
        public Task StopKeywordRecognitionAsync()
        {
            return Task.Run(() =>
            {
                base.DoAsyncRecognitionAction(StopKeywordRecognition);
            });
        }

        /// <summary>
        /// Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
        /// </summary>
        /// <param name="simplePhrase">The phrase corresponding to the intent.</param>
        /// <remarks>Once recognized, the IntentRecognitionResult's IntentId property will match the simplePhrase specified here.</remarks>
        public void AddIntent(string simplePhrase)
        {
            var intentTrigger = IntentTrigger.From(simplePhrase);
            AddIntent(intentTrigger, simplePhrase);
        }

        /// <summary>
        /// Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
        /// </summary>
        /// <param name="simplePhrase">The phrase corresponding to the intent.</param>
        /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
        /// <remarks>Once recognized, the result's intent id will match the id supplied here.</remarks>
        public void AddIntent(string simplePhrase, string intentId)
        {
            var intentTrigger = IntentTrigger.From(simplePhrase);
            AddIntent(intentTrigger, intentId);
        }

        /// <summary>
        /// Adds a single intent by name from the specified Language Understanding Model.
        /// </summary>
        /// <param name="model">The language understanding model containing the intent.</param>
        /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
        /// <remarks>Once recognized, the IntentRecognitionResult's IntentId property will contain the intentName specified here.</remarks>
        public void AddIntent(LanguageUnderstandingModel model, string intentName)
        {
            ThrowIfNull(model);
            var intentTrigger = IntentTrigger.From(model, intentName);
            AddIntent(intentTrigger, intentName);
        }

        /// <summary>
        /// Adds a single intent by name from the specified Language Understanding Model.
        /// </summary>
        /// <param name="model">The language understanding model containing the intent.</param>
        /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
        /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
        public void AddIntent(LanguageUnderstandingModel model, string intentName, string intentId)
        {
            ThrowIfNull(model);
            var intentTrigger = IntentTrigger.From(model, intentName);
            AddIntent(intentTrigger, intentId);
        }

        /// <summary>
        /// Adds all intents from the specified Language Understanding Model.
        /// </summary>
        /// <param name="model">The language understanding model from Language Understanding service.</param>
        /// <param name="intentId">A custom string id to be returned in the IntentRecognitionResult's IntentId property.</param>
        public void AddAllIntents(LanguageUnderstandingModel model, string intentId)
        {
            ThrowIfNull(model);
            var intentTrigger = IntentTrigger.From(model);
            AddIntent(intentTrigger, intentId);
        }

        /// <summary>
        /// Adds all intents from the specified Language Understanding Model.
        /// </summary>
        /// <param name="model">The language understanding model from Language Understanding service.</param>
        public void AddAllIntents(LanguageUnderstandingModel model)
        {
            ThrowIfNull(model);
            var intentTrigger = IntentTrigger.From(model);
            AddIntent(intentTrigger, "");
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        private void AddIntent(IntentTrigger intentTrigger, string intentId)
        {
            ThrowIfNull(recoHandle, "Invalid recognizer handle");
            IntPtr intentIdPtr = Utf8StringMarshaler.MarshalManagedToNative(intentId);
            try
            {
                ThrowIfFail(Internal.Recognizer.intent_recognizer_add_intent(recoHandle, intentIdPtr, intentTrigger.triggerHandle));
            }
            finally
            {
                Marshal.FreeHGlobal(intentIdPtr);
            }
            GC.KeepAlive(intentTrigger);
        }

        ~IntentRecognizer()
        {
            isDisposing = true;
            Dispose(false);
        }

        /// <summary>
        /// This method performs cleanup of resources.
        /// The Boolean parameter <paramref name="disposing"/> indicates whether the method is called from <see cref="IDisposable.Dispose"/> (if <paramref name="disposing"/> is true) or from the finalizer (if <paramref name="disposing"/> is false).
        /// Derived classes should override this method to dispose resource if needed.
        /// </summary>
        /// <param name="disposing">Flag to request disposal.</param>
        /// <returns></returns>
        protected override void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (recoHandle != null)
            {
                LogErrorIfFail(Internal.Recognizer.recognizer_recognizing_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_recognized_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_canceled_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_session_started_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_session_stopped_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_speech_start_detected_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_speech_end_detected_set_callback(recoHandle, null, IntPtr.Zero));
            }

            // Dispose of managed resources
            if (disposing)
            {
                recoHandle?.Dispose();
                // This will make Properties unaccessible.
                Properties?.Close();
            }

            recognizingCallbackDelegate = null;
            recognizedCallbackDelegate = null;
            canceledCallbackDelegate = null;
            audioInputKeepAlive = null;
            base.Dispose(disposing);
        }

        private CallbackFunctionDelegate recognizingCallbackDelegate;
        private CallbackFunctionDelegate recognizedCallbackDelegate;
        private CallbackFunctionDelegate canceledCallbackDelegate;

        private Audio.AudioConfig audioInputKeepAlive;

        // Defines private methods to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.
        [MonoPInvokeCallback(typeof(CallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_Recognizing(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var recognizer = InteropSafeHandle.GetObjectFromWeakHandle<IntentRecognizer>(pvContext);
                if (recognizer == null || recognizer.isDisposing)
                {
                    return;
                }
                var resultEventArg = new IntentRecognitionEventArgs(hevent);
                recognizer._Recognizing?.Invoke(recognizer, resultEventArg);
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }

        [MonoPInvokeCallback(typeof(CallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_Recognized(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var recognizer = InteropSafeHandle.GetObjectFromWeakHandle<IntentRecognizer>(pvContext);
                if (recognizer == null || recognizer.isDisposing)
                {
                    return;
                }
                var resultEventArg = new IntentRecognitionEventArgs(hevent);
                recognizer._Recognized?.Invoke(recognizer, resultEventArg);
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }

        [MonoPInvokeCallback(typeof(CallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_Canceled(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var recognizer = InteropSafeHandle.GetObjectFromWeakHandle<IntentRecognizer>(pvContext);
                if (recognizer == null || recognizer.isDisposing)
                {
                    return;
                }
                var resultEventArg = new IntentRecognitionCanceledEventArgs(hevent);
                recognizer._Canceled?.Invoke(recognizer, resultEventArg);
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }
    }
}
