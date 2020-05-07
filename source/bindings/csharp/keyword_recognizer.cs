//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;
using System.Threading;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Recognizer type that is specialized to only handle keyword activation.
    /// <seealso cref="KeywordRecognitionResult" />
    /// <seealso cref="KeywordRecognitionEventArgs" />
    /// <seealso cref="KeywordRecognitionModel" />
    /// <seealso cref="AudioConfig" />
    /// </summary>
    /// <example>
    /// First, the object needs to be instantiated:
    /// <code>
    /// var audioConfig = AudioConfig.FromDefaultMicrophoneInput(); // Or an alternative input
    /// var recognizer = KeywordRecognizer.FromConfig(audioConfig);
    /// </code>
    /// (optional) Then, the events need to be wired in order to receive notifications:
    /// <code>
    /// recognizer.Recognized += (s, e)
    /// {
    ///     // Your logic here...
    /// };
    /// </code>
    /// And finally, recognition needs to be started.
    /// <code>
    /// var keywordModel = KeywordRecognitionModel.FromFile(modelPath);
    /// var result = await recognizer.RecognizeKeywordOnceAsync(keywordModel);
    /// </code>
    /// </example>
    public sealed class KeywordRecognizer: IDisposable
    {
        /// <summary>
        /// GC handle for callbacks for context.
        /// </summary>
        private GCHandle gch;

        private SpeechRecognizer recognizer;

        /// <summary>
        /// Creates a KeywordRecognizer from an <see cref="AudioConfig" />. The config is intended
        /// to define the audio input to be used by the recognizer object.
        /// </summary>
        /// <param name="audioConfig">Defines the audio input to be used by the recognizer.</param>
        /// <returns>A new KeywordRecognizer that will consume audio from the specified input.</returns>
        /// <remarks>
        /// If no audio config is provided, it will be equivalent to calling with a config constructed with
        /// <see cref="Audio.AudioConfig.FromDefaultMicrophoneInput" />
        /// </remarks>
        public KeywordRecognizer(Audio.AudioConfig audioConfig)
        {
            var config = audioConfig == null ? Audio.AudioConfig.FromDefaultMicrophoneInput() : audioConfig;
            recognizer = new SpeechRecognizer(Recognizer.FromConfig(SpxFactory.recognizer_create_keyword_recognizer_from_audio_config, config));
            recognizedCallbackDelegate = FireEvent_Recognized;
            gch = GCHandle.Alloc(this, GCHandleType.Weak);
        }

        /// <summary>
        /// Starts a keyword recognition session. This session will last until the first keyword is recognized. When this happens,
        /// a <see cref="Recognized" /> event will be raised and the session will end. To rearm the keyword, the method needs to be called
        /// again after the event is emitted.
        /// </summary>
        /// <param name="model">The <see cref="KeywordRecognitionModel" /> that describes the keyword we want to detect.</param>
        /// <returns>A future that resolves to a <see cref="KeywordRecognitionResult" /> that resolves once a keyword is detected.</returns>
        /// <remarks>
        /// Note that if no keyword is detected in the input, the task will never resolve (unless <see cref="StopRecognitionAsync" /> is called.
        /// </remarks>
        public Task<KeywordRecognitionResult> RecognizeOnceAsync(KeywordRecognitionModel model)
        {
            return Task.Run(() =>
            {
                KeywordRecognitionResult result = null;
                recognizer.DoAsyncRecognitionAction(() =>
                {
                    IntPtr resultHandle = IntPtr.Zero;
                    ThrowIfNull(recognizer.recoHandle, "Invalid recognizer handle");
                    ThrowIfFail(Internal.Recognizer.recognizer_recognize_keyword_once(recognizer.recoHandle, model.keywordHandle, out resultHandle));
                    result = new KeywordRecognitionResult(resultHandle);
                });
                return result;
            });
        }

        /// <summary>
        /// Stops a currently active keyword recognition session.
        /// </summary>
        /// <returns>A future that resolves when the active session (if any) is stopped.</returns>
        public Task StopRecognitionAsync()
        {
            return recognizer.StopKeywordRecognitionAsync();
        }

        /// <summary>
        /// Signal for events related to the recognition of keywords.
        /// </summary>
        public event EventHandler<KeywordRecognitionEventArgs> Recognized
        {
            add
            {
                if (_Recognized == null)
                {
                    ThrowIfFail(Internal.Recognizer.recognizer_recognized_set_callback(recognizer.recoHandle, recognizedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                _Recognized += value;
            }
            remove
            {
                _Recognized -= value;
                if (_Recognized == null)
                {
                    LogErrorIfFail(Internal.Recognizer.recognizer_recognized_set_callback(recognizer.recoHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// Signal for events relating to the cancellation of an interaction. The event indicates if the reason is a direct cancellation or an error.
        /// </summary>
        public event EventHandler<SpeechRecognitionCanceledEventArgs> Canceled
        {
            add
            {
                recognizer.Canceled += value;
            }
            remove
            {
                recognizer.Canceled -= value;
            }
        }

        /// <summary>
        /// A collection of properties and their values defined for this <see cref="KeywordRecognizer"/>.
        /// </summary>
        public PropertyCollection Properties
        {
            get => recognizer.Properties;
        }

        /// <summary>
        /// isDisposing is a flag used to indicate if object is being disposed.
        /// </summary>
        private volatile bool isDisposing = false;

        /// <summary>
        /// This method performs cleanup of resources.
        /// </summary>
        public void Dispose()
        {
            if (isDisposing)
            {
                return;
            }
            isDisposing = true;
            recognizedCallbackDelegate = null;
            LogErrorIfFail(Internal.Recognizer.recognizer_recognized_set_callback(recognizer.recoHandle, null, IntPtr.Zero));
            recognizer.Dispose();

        }

        private CallbackFunctionDelegate recognizedCallbackDelegate;
        private event EventHandler<KeywordRecognitionEventArgs> _Recognized;

        // Defines private methods to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.
        [MonoPInvokeCallback(typeof(CallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_Recognized(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var recognizer = InteropSafeHandle.GetObjectFromWeakHandle<KeywordRecognizer>(pvContext);
                if (recognizer == null || recognizer.isDisposing)
                {
                    return;
                }
                var resultEventArg = new KeywordRecognitionEventArgs(hevent);
                recognizer._Recognized?.Invoke(recognizer, resultEventArg);
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }
    }
}
