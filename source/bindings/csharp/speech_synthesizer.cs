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
using System.Text;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Performs speech synthesis to speaker, file, or other audio output streams, and gets synthesized audio as result.
    /// Updated in version 1.7.0
    /// </summary>
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Usage", "CA2213", MessageId = "streamKeepAlive", Justification = "The audio stream is sometimes not owned, and should not be disposed.")]
    public sealed class SpeechSynthesizer : IDisposable
    {
        private event EventHandler<SpeechSynthesisEventArgs> _SynthesisStarted;
        private event EventHandler<SpeechSynthesisEventArgs> _Synthesizing;
        private event EventHandler<SpeechSynthesisEventArgs> _SynthesisCompleted;
        private event EventHandler<SpeechSynthesisEventArgs> _SynthesisCanceled;
        private event EventHandler<SpeechSynthesisWordBoundaryEventArgs> _WordBoundary;

        /// <summary>
        /// The event <see cref="SynthesisStarted"/> signals that the speech synthesis has started.
        /// </summary>
        public event EventHandler<SpeechSynthesisEventArgs> SynthesisStarted
        {
            add
            {
                if (this._SynthesisStarted == null)
                {
                    ThrowIfFail(Internal.Synthesizer.synthesizer_started_set_callback(synthHandle, synthesisStartedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._SynthesisStarted += value;
            }
            remove
            {
                this._SynthesisStarted -= value;
                if (this._SynthesisStarted == null)
                {
                    LogErrorIfFail(Internal.Synthesizer.synthesizer_started_set_callback(synthHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// The event <see cref="Synthesizing"/> signals that the speech synthesis is on going.
        /// </summary>
        public event EventHandler<SpeechSynthesisEventArgs> Synthesizing
        {
            add
            {
                if (this._Synthesizing == null)
                {
                    ThrowIfFail(Internal.Synthesizer.synthesizer_synthesizing_set_callback(synthHandle, synthesizingCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Synthesizing += value;
            }
            remove
            {
                this._Synthesizing -= value;
                if (this._Synthesizing == null)
                {
                    LogErrorIfFail(Internal.Synthesizer.synthesizer_synthesizing_set_callback(synthHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// The event <see cref="SynthesisCompleted"/> signals that the speech synthesis has completed.
        /// </summary>
        public event EventHandler<SpeechSynthesisEventArgs> SynthesisCompleted
        {
            add
            {
                if (this._SynthesisCompleted == null)
                {
                    ThrowIfFail(Internal.Synthesizer.synthesizer_completed_set_callback(synthHandle, synthesisCompletedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._SynthesisCompleted += value;
            }
            remove
            {
                this._SynthesisCompleted -= value;
                if (this._SynthesisCompleted == null)
                {
                    LogErrorIfFail(Internal.Synthesizer.synthesizer_completed_set_callback(synthHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// The event <see cref="SynthesisCanceled"/> signals that the speech synthesis was canceled.
        /// </summary>
        public event EventHandler<SpeechSynthesisEventArgs> SynthesisCanceled
        {
            add
            {
                if (this._SynthesisCanceled == null)
                {
                    ThrowIfFail(Internal.Synthesizer.synthesizer_canceled_set_callback(synthHandle, synthesisCanceledCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._SynthesisCanceled += value;
            }
            remove
            {
                this._SynthesisCanceled -= value;
                if (this._SynthesisCanceled == null)
                {
                    LogErrorIfFail(Internal.Synthesizer.synthesizer_canceled_set_callback(synthHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// The event <see cref="WordBoundary"/> signals that a word boundary was received.
        /// Added in version 1.7.0
        /// </summary>
        public event EventHandler<SpeechSynthesisWordBoundaryEventArgs> WordBoundary
        {
            add
            {
                if (this._WordBoundary == null)
                {
                    ThrowIfFail(Internal.Synthesizer.synthesizer_word_boundary_set_callback(synthHandle, wordBoundaryCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._WordBoundary += value;
            }
            remove
            {
                this._WordBoundary -= value;
                if (this._WordBoundary == null)
                {
                    LogErrorIfFail(Internal.Synthesizer.synthesizer_word_boundary_set_callback(synthHandle, null, IntPtr.Zero));
                }
            }
        }

        internal InteropSafeHandle synthHandle;

        private TtsCallbackFunctionDelegate synthesisStartedCallbackDelegate;
        private TtsCallbackFunctionDelegate synthesizingCallbackDelegate;
        private TtsCallbackFunctionDelegate synthesisCompletedCallbackDelegate;
        private TtsCallbackFunctionDelegate synthesisCanceledCallbackDelegate;
        private TtsCallbackFunctionDelegate wordBoundaryCallbackDelegate;

        private GCHandle gch;
        private object synthesizerLock = new object();
        private int activeAsyncSynthesisCounter = 0;

        private IDisposable streamKeepAlive = null;

        private volatile bool disposed = false;
        private volatile bool isDisposing = false;

        /// <summary>
        /// Creates a new instance of SpeechSynthesizer.
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        public SpeechSynthesizer(SpeechConfig speechConfig) : this(FromConfig(speechConfig))
        {
        }

        /// <summary>
        /// Creates a new instance of SpeechSynthesizer.
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        /// <param name="audioConfig">Audio configuration</param>
        public SpeechSynthesizer(SpeechConfig speechConfig, Audio.AudioConfig audioConfig)
            : this(FromConfig(speechConfig, audioConfig))
        {
            streamKeepAlive = audioConfig?.MoveStreamOwnerShip();
        }

        /// <summary>
        /// Creates a new instance of SpeechSynthesizer.
        /// Added in 1.13.0
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        /// <param name="autoDetectSourceLanguageConfig">The auto detect source language config</param>
        /// <param name="audioConfig">Audio configuration</param>
        public SpeechSynthesizer(SpeechConfig speechConfig, AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig, Audio.AudioConfig audioConfig)
            : this(FromConfig(speechConfig, autoDetectSourceLanguageConfig, audioConfig))
        {
            streamKeepAlive = audioConfig?.MoveStreamOwnerShip();
        }

        internal SpeechSynthesizer(InteropSafeHandle synthHandle)
        {
            ThrowIfNull(synthHandle, "Invalid synthesizer handle");

            synthesisStartedCallbackDelegate = FireEvent_SynthesisStarted;
            synthesizingCallbackDelegate = FireEvent_Synthesizing;
            synthesisCompletedCallbackDelegate = FireEvent_SynthesisCompleted;
            synthesisCanceledCallbackDelegate = FireEvent_SynthesisCanceled;
            wordBoundaryCallbackDelegate = FireEvent_WordBoundary;

            this.synthHandle = synthHandle;
            gch = GCHandle.Alloc(this, GCHandleType.Weak);

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.Synthesizer.synthesizer_get_property_bag(synthHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        internal static InteropSafeHandle FromConfig(SpeechConfig speechConfig)
        {
            if (speechConfig == null)
            {
                throw new ArgumentNullException(nameof(speechConfig));
            }

            Audio.AudioConfig audioConfig = Audio.AudioConfig.FromDefaultSpeakerOutput(); // Use default speaker as default audio output

            IntPtr synthHandlePtr = IntPtr.Zero;
            ThrowIfFail(SpxFactory.synthesizer_create_speech_synthesizer_from_config(out synthHandlePtr, speechConfig.configHandle, audioConfig.configHandle));
            InteropSafeHandle synthHandle = new InteropSafeHandle(synthHandlePtr, Internal.Synthesizer.synthesizer_handle_release);
            GC.KeepAlive(speechConfig);
            GC.KeepAlive(audioConfig);

            return synthHandle;
        }

        internal static InteropSafeHandle FromConfig(SpeechConfig speechConfig, Audio.AudioConfig audioConfig)
        {
            if (speechConfig == null)
            {
                throw new ArgumentNullException(nameof(speechConfig));
            }

            InteropSafeHandle audioConfigHandle;
            if (audioConfig == null)
            {
                audioConfigHandle = new InteropSafeHandle(IntPtr.Zero, null);
            }
            else
            {
                audioConfigHandle = audioConfig.configHandle;
            }

            IntPtr synthHandlePtr = IntPtr.Zero;
            ThrowIfFail(SpxFactory.synthesizer_create_speech_synthesizer_from_config(out synthHandlePtr, speechConfig.configHandle, audioConfigHandle));
            InteropSafeHandle synthHandle = new InteropSafeHandle(synthHandlePtr, Internal.Synthesizer.synthesizer_handle_release);
            GC.KeepAlive(speechConfig);
            GC.KeepAlive(audioConfig);

            return synthHandle;
        }

        internal static InteropSafeHandle FromConfig(SpeechConfig speechConfig, AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig, Audio.AudioConfig audioConfig)
        {
            if (speechConfig == null)
            {
                throw new ArgumentNullException(nameof(speechConfig));
            }

            if (autoDetectSourceLanguageConfig == null)
            {
                throw new ArgumentNullException(nameof(autoDetectSourceLanguageConfig));
            }

            InteropSafeHandle audioConfigHandle;
            if (audioConfig == null)
            {
                audioConfigHandle = new InteropSafeHandle(IntPtr.Zero, null);
            }
            else
            {
                audioConfigHandle = audioConfig.configHandle;
            }

            IntPtr synthHandlePtr = IntPtr.Zero;
            ThrowIfFail(SpxFactory.synthesizer_create_speech_synthesizer_from_auto_detect_source_lang_config(out synthHandlePtr, speechConfig.configHandle, autoDetectSourceLanguageConfig.configHandle, audioConfigHandle));
            InteropSafeHandle synthHandle = new InteropSafeHandle(synthHandlePtr, Internal.Synthesizer.synthesizer_handle_release);
            GC.KeepAlive(speechConfig);
            GC.KeepAlive(autoDetectSourceLanguageConfig);
            GC.KeepAlive(audioConfig);

            return synthHandle;
        }

        /// <summary>
        /// The collection of properties and their values defined for this <see cref="SpeechSynthesizer"/>.
        /// Note: The property collection is only valid until the SpeechSynthesizer owning this Properties is disposed or finalized.
        /// </summary>
        public PropertyCollection Properties { get; internal set; }

        /// <summary>
        /// Gets/sets authorization token used to communicate with the service.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expires, the caller needs to refresh it by calling this setter with a new valid token.
        /// Otherwise, the synthesizer will encounter errors while speech synthesis.
        /// Added in version 1.7.0
        /// </summary>
        public string AuthorizationToken
        {
            get
            {
                return Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token);
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
        /// Execute the speech synthesis on plain text, asynchronously.
        /// </summary>
        /// <param name="text">The plain text for synthesis.</param>
        /// <returns>A task representing the synthesis operation. The task returns a value of SpeechSynthesisResult.</returns>
        public Task<SpeechSynthesisResult> SpeakTextAsync(string text)
        {
            return Task.Run(() =>
            {
                SpeechSynthesisResult result = null;
                DoAsyncSynthesisAction(() => {
                    ThrowIfNull(synthHandle, "Invalid synthesizer handle");

                    IntPtr textPtr = Utf8StringMarshaler.MarshalManagedToNative(text);
                    uint textLen = (uint)(Encoding.UTF8.GetBytes(text).Length);

                    try
                    {
                        IntPtr hresult = IntPtr.Zero;
                        ThrowIfFail(Internal.Synthesizer.synthesizer_speak_text(synthHandle, textPtr, textLen, out hresult));
                        result = new SpeechSynthesisResult(hresult);
                    }
                    finally
                    {
                        Marshal.FreeHGlobal(textPtr);
                    }
                });

                return result;
            });
        }

        /// <summary>
        /// Execute the speech synthesis on SSML, asynchronously.
        /// </summary>
        /// <param name="ssml">The SSML for synthesis.</param>
        /// <returns>A task representing the synthesis operation. The task returns a value of SpeechSynthesisResult.</returns>
        public Task<SpeechSynthesisResult> SpeakSsmlAsync(string ssml)
        {
            return Task.Run(() =>
            {
                SpeechSynthesisResult result = null;
                DoAsyncSynthesisAction(() => {
                    ThrowIfNull(synthHandle, "Invalid synthesizer handle");

                    IntPtr ssmlPtr = Utf8StringMarshaler.MarshalManagedToNative(ssml);
                    uint ssmlLen = (uint)(Encoding.UTF8.GetBytes(ssml).Length);

                    try
                    {
                        IntPtr hresult = IntPtr.Zero;
                        ThrowIfFail(Internal.Synthesizer.synthesizer_speak_ssml(synthHandle, ssmlPtr, ssmlLen, out hresult));
                        result = new SpeechSynthesisResult(hresult);
                    }
                    finally
                    {
                        Marshal.FreeHGlobal(ssmlPtr);
                    }
                });

                return result;
            });
        }

        /// <summary>
        /// Start the speech synthesis on plain text, asynchronously.
        /// </summary>
        /// <param name="text">The plain text for synthesis.</param>
        /// <returns>A task representing the synthesis operation. The task returns a value of SpeechSynthesisResult.</returns>
        public Task<SpeechSynthesisResult> StartSpeakingTextAsync(string text)
        {
            return Task.Run(() =>
            {
                SpeechSynthesisResult result = null;
                DoAsyncSynthesisAction(() => {
                    ThrowIfNull(synthHandle, "Invalid synthesizer handle");

                    IntPtr textPtr = Utf8StringMarshaler.MarshalManagedToNative(text);
                    uint textLen = (uint)(Encoding.UTF8.GetBytes(text).Length);

                    try
                    {
                        IntPtr hresult = IntPtr.Zero;
                        ThrowIfFail(Internal.Synthesizer.synthesizer_start_speaking_text(synthHandle, textPtr, textLen, out hresult));
                        result = new SpeechSynthesisResult(hresult);
                    }
                    finally
                    {
                        Marshal.FreeHGlobal(textPtr);
                    }
                });

                return result;
            });
        }

        /// <summary>
        /// Start the speech synthesis on SSML, asynchronously.
        /// </summary>
        /// <param name="ssml">The SSML for synthesis.</param>
        /// <returns>A task representing the synthesis operation. The task returns a value of SpeechSynthesisResult.</returns>
        public Task<SpeechSynthesisResult> StartSpeakingSsmlAsync(string ssml)
        {
            return Task.Run(() =>
            {
                SpeechSynthesisResult result = null;
                DoAsyncSynthesisAction(() => {
                    ThrowIfNull(synthHandle, "Invalid synthesizer handle");

                    IntPtr ssmlPtr = Utf8StringMarshaler.MarshalManagedToNative(ssml);
                    uint ssmlLen = (uint)(Encoding.UTF8.GetBytes(ssml).Length);

                    try
                    {
                        IntPtr hresult = IntPtr.Zero;
                        ThrowIfFail(Internal.Synthesizer.synthesizer_start_speaking_ssml(synthHandle, ssmlPtr, ssmlLen, out hresult));
                        result = new SpeechSynthesisResult(hresult);
                    }
                    finally
                    {
                        Marshal.FreeHGlobal(ssmlPtr);
                    }
                });

                return result;
            });
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            try
            {
                isDisposing = true;
                lock (synthesizerLock)
                {
                    if (activeAsyncSynthesisCounter != 0)
                    {
                        throw new InvalidOperationException("Cannot dispose a synthesizer while async synthesis is running. Await async synthesis to avoid unexpected disposals.");
                    }
                }
            }
            finally
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }
        }

        ~SpeechSynthesizer()
        {
            isDisposing = true;
            Dispose(false);
        }

        private void DoAsyncSynthesisAction(Action synthImplAction)
        {
            lock (synthesizerLock)
            {
                activeAsyncSynthesisCounter++;
            }
            if (disposed || isDisposing)
            {
                throw new ObjectDisposedException(this.GetType().Name);
            }
            try
            {
                synthImplAction();
            }
            finally
            {
                lock (synthesizerLock)
                {
                    activeAsyncSynthesisCounter--;
                }
            }
        }

        private void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                // This will make Properties unaccessible.
                Properties.Close();
            }

            if (synthHandle != null)
            {
                LogErrorIfFail(Internal.Synthesizer.synthesizer_started_set_callback(synthHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Synthesizer.synthesizer_synthesizing_set_callback(synthHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Synthesizer.synthesizer_completed_set_callback(synthHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Synthesizer.synthesizer_canceled_set_callback(synthHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Synthesizer.synthesizer_word_boundary_set_callback(synthHandle, null, IntPtr.Zero));
                synthHandle.Dispose();
            }

            synthesisStartedCallbackDelegate = null;
            synthesizingCallbackDelegate = null;
            synthesisCompletedCallbackDelegate = null;
            synthesisCanceledCallbackDelegate = null;
            wordBoundaryCallbackDelegate = null;

            streamKeepAlive?.Dispose();  // The Close() callback in stream is called during the disposing of synthHandle

            if (gch.IsAllocated)
            {
                gch.Free();
            }

            disposed = true;
        }

        // Defines private methods to raise a C# event for speech synthesis result when a corresponding callback is invoked by the native layer.
        [MonoPInvokeCallback(typeof(TtsCallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_SynthesisStarted(IntPtr hsynth, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var synthesizer = InteropSafeHandle.GetObjectFromWeakHandle<SpeechSynthesizer>(pvContext);
                if (synthesizer == null || synthesizer.isDisposing)
                {
                    return;
                }

                using (var resultEventArg = new SpeechSynthesisEventArgs(hevent))
                {
                    synthesizer._SynthesisStarted?.Invoke(synthesizer, resultEventArg);
                }
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }

        [MonoPInvokeCallback(typeof(TtsCallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_Synthesizing(IntPtr hsynth, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var synthesizer = InteropSafeHandle.GetObjectFromWeakHandle<SpeechSynthesizer>(pvContext);
                if (synthesizer == null || synthesizer.isDisposing)
                {
                    return;
                }

                using (var resultEventArg = new SpeechSynthesisEventArgs(hevent))
                {
                    synthesizer._Synthesizing?.Invoke(synthesizer, resultEventArg);
                }
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }

        [MonoPInvokeCallback(typeof(TtsCallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_SynthesisCompleted(IntPtr hsynth, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var synthesizer = InteropSafeHandle.GetObjectFromWeakHandle<SpeechSynthesizer>(pvContext);
                if (synthesizer == null || synthesizer.isDisposing)
                {
                    return;
                }

                using (var resultEventArg = new SpeechSynthesisEventArgs(hevent))
                {
                    synthesizer._SynthesisCompleted?.Invoke(synthesizer, resultEventArg);
                }
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }

        [MonoPInvokeCallback(typeof(TtsCallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_SynthesisCanceled(IntPtr hsynth, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var synthesizer = InteropSafeHandle.GetObjectFromWeakHandle<SpeechSynthesizer>(pvContext);
                if (synthesizer == null || synthesizer.isDisposing)
                {
                    return;
                }

                using (var resultEventArg = new SpeechSynthesisEventArgs(hevent))
                {
                    synthesizer._SynthesisCanceled?.Invoke(synthesizer, resultEventArg);
                }
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }

        [MonoPInvokeCallback(typeof(TtsCallbackFunctionDelegate))]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "All exceptions are catched and logged inside callback handlers")]
        private static void FireEvent_WordBoundary(IntPtr hsynth, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var synthesizer = InteropSafeHandle.GetObjectFromWeakHandle<SpeechSynthesizer>(pvContext);
                if (synthesizer == null || synthesizer.isDisposing)
                {
                    return;
                }

                using (var wordBoundaryEventArg = new SpeechSynthesisWordBoundaryEventArgs(hevent))
                {
                    synthesizer._WordBoundary?.Invoke(synthesizer, wordBoundaryEventArg);
                }
            }
            catch (Exception e)
            {
                LogError(e.Message);
            }
        }
    }
}
