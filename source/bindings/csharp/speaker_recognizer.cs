//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;
using System.Threading;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Perform speaker recognition.
    /// Added in version 1.12.0
    /// </summary>
    public sealed class SpeakerRecognizer : DisposableBase
    {
        /// <summary>
        /// Creates a speaker recognizer.
        /// </summary>
        /// <param name="speechConfig">The speech configuration to use.</param>
        /// <param name="audioConfig">The audio config to use.</param>
        /// <returns></returns>
        ///
        public SpeakerRecognizer(SpeechConfig speechConfig, Audio.AudioConfig audioConfig)
        {
            if (audioConfig == null) throw new ArgumentNullException(nameof(audioConfig));
            if (speechConfig == null) throw new ArgumentException(nameof(speechConfig));

            IntPtr handlePtr = Internal.SpeakerRecognition.SPXHANDLE_INVALID;

            ThrowIfFail(SpxFactory.recognizer_create_speaker_recognizer_from_config(out handlePtr, speechConfig.configHandle, audioConfig.configHandle));

            InteropSafeHandle handle = new InteropSafeHandle(handlePtr, Internal.SpeakerRecognition.speaker_recognizer_release_handle);
            GC.KeepAlive(speechConfig);

            this.speakerRecognizerHandle = handle;

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SpeakerRecognition.speaker_recognizer_get_property_bag(speakerRecognizerHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);

            audioInputKeepAlive = audioConfig;
        }

        /// <summary>
        /// Gets the collection of properties and their values defined for this <see cref="SpeakerRecognizer"/>.
        /// </summary>
        public PropertyCollection Properties { get; internal set; }

        /// <summary>
        /// Verify the speaker in the speaker verification model.
        /// </summary>
        /// <param name="model">A SpeakerVerificationModel.</param>
        /// <returns>An asynchronous operation representing verifying the speaker in the model.</returns>
        public Task<SpeakerRecognitionResult> RecognizeOnceAsync(SpeakerVerificationModel model)
        {
            return RunAsync((h) =>
            {
                IntPtr resultHandle = IntPtr.Zero;

                Internal.SpeakerRecognition.speaker_recognizer_verify(h, model.modelHandle, out resultHandle);
                return new SpeakerRecognitionResult(resultHandle);
            });
        }

        /// <summary>
        /// Identify the speakers in the speaker identification Model.
        /// </summary>
        /// <param name="model">A speaker identification model.</param>
        /// <returns>An asynchronous operation representing identifying the speakers in the model.</returns>
        public Task<SpeakerRecognitionResult> RecognizeOnceAsync(SpeakerIdentificationModel model)
        {
            return RunAsync((h) =>
            {
                IntPtr resultHandle = IntPtr.Zero;

                Internal.SpeakerRecognition.speaker_recognizer_identify(h, model.modelHandle, out resultHandle);
                return new SpeakerRecognitionResult(resultHandle);
            });
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="disposeManaged"></param>
        /// <returns></returns>
        override protected void Dispose(bool disposeManaged)
        {
            if (disposeManaged)
            {
                // the C# standard guarantees that reads of 32 bit values always atomic
                if (activeAsyncCounter != 0)
                {
                    throw new InvalidOperationException("Cannot dispose a speaker recognizer while an async action is running. Await async actions to avoid unexpected disposals.");
                }

                // This will make Properties unaccessible.
                Properties.Close();

                if (speakerRecognizerHandle != null)
                {
                    speakerRecognizerHandle.Dispose();
                }
            }

            audioInputKeepAlive = null;
        }

        private Task<SpeakerRecognitionResult> RunAsync(Func<InteropSafeHandle, SpeakerRecognitionResult> method)
        {
            return Task.Run(() =>
            {
                CheckDisposed();
                ThrowIfNull(speakerRecognizerHandle);
                SpeakerRecognitionResult result;
                try
                {
                    Interlocked.Increment(ref activeAsyncCounter);
                    result = method(speakerRecognizerHandle);
                }
                finally
                {
                    Interlocked.Decrement(ref activeAsyncCounter);
                }
                return result;
            });
        }

        internal InteropSafeHandle speakerRecognizerHandle;
        private int activeAsyncCounter = 0;
        private Audio.AudioConfig audioInputKeepAlive;
    }
}
