//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Diagnostics.Contracts;
using System.Globalization;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Contains detailed information about result of a speech synthesis operation.
    /// Added in version 1.4.0
    /// </summary>
    public class SpeechSynthesisResult : IDisposable
    {
        internal SpeechSynthesisResult(IntPtr resultHandlePtr)
        {
            ThrowIfNull(resultHandlePtr);
            resultHandle = new InteropSafeHandle(resultHandlePtr, Internal.SynthesisResult.synthesizer_result_handle_release);

            this.ResultId = SpxFactory.GetDataFromHandleUsingDelegate(Internal.SynthesisResult.synth_result_get_result_id, resultHandle, maxCharCount);

            ResultReason resultReason;
            ThrowIfFail(Internal.SynthesisResult.synth_result_get_reason(resultHandle, out resultReason));
            this.Reason = resultReason;

            this.AudioData = GetAudioData();

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SynthesisResult.synth_result_get_property_bag(resultHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        /// <summary>
        /// Specifies unique ID of speech synthesis result.
        /// </summary>
        public string ResultId { get; }

        /// <summary>
        /// Specifies status of speech synthesis result.
        /// </summary>
        public ResultReason Reason { get; }

        /// <summary>
        /// Presents the synthesized audio in the result.
        /// </summary>
        public byte[] AudioData { get; }

        /// <summary>
        /// Contains properties of the results.
        /// </summary>
        public PropertyCollection Properties { get; private set; }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// This method performs cleanup of resources.
        /// The Boolean parameter <paramref name="disposing"/> indicates whether the method is called from <see cref="IDisposable.Dispose"/> (if <paramref name="disposing"/> is true) or from the finalizer (if <paramref name="disposing"/> is false).
        /// Derived classes should override this method to dispose resource if needed.
        /// </summary>
        /// <param name="disposing">Flag to request disposal.</param>
        /// <returns></returns>
        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                // free managed resources
                Properties?.Close();
                resultHandle?.Dispose();
            }
            // free native resources if there are any.
            disposed = true;
        }

        private byte[] GetAudioData()
        {
            uint audioLength;
            ThrowIfFail(Internal.SynthesisResult.synth_result_get_audio_length(resultHandle, out audioLength));

            audioLength = audioLength % int.MaxValue;
            IntPtr nativeBuffer = Marshal.AllocHGlobal((int)audioLength);

            try
            {
                uint filledSize;
                ThrowIfFail(Internal.SynthesisResult.synth_result_get_audio_data(resultHandle, nativeBuffer, audioLength, out filledSize));

                filledSize = filledSize % int.MaxValue;
                byte[] buffer = new byte[filledSize];
                Marshal.Copy(nativeBuffer, buffer, 0, (int)filledSize);

                return buffer;
            }
            finally
            {
                Marshal.FreeHGlobal(nativeBuffer);
            }
        }

        internal InteropSafeHandle resultHandle;
        internal const Int32 maxCharCount = 1024;
        internal volatile bool disposed = false;
    }

    /// <summary>
    /// Contains detailed information about why a speech synthesis result was canceled.
    /// Added in version 1.4.0
    /// </summary>
    public class SpeechSynthesisCancellationDetails
    {
        /// <summary>
        /// Creates an instance of SpeechSynthesisCancellationDetails object for the canceled SpeechSynthesisResult.
        /// </summary>
        /// <param name="result">The result that was canceled.</param>
        /// <returns>The CancellationDetails object being created.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1062", Justification = "validated in internal constructor")]
         public static SpeechSynthesisCancellationDetails FromResult(SpeechSynthesisResult result)
        {
            return new SpeechSynthesisCancellationDetails(result);
        }

        /// <summary>
        /// Creates an instance of SpeechSynthesisCancellationDetails object for the canceled AudioDataStream.
        /// </summary>
        /// <param name="stream">The stream that was canceled</param>
        /// <returns>The CancellationDetails object being created.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1062", Justification = "validated in internal constructor")]
        public static SpeechSynthesisCancellationDetails FromStream(AudioDataStream stream)
        {
            return new SpeechSynthesisCancellationDetails(stream);
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        internal SpeechSynthesisCancellationDetails(SpeechSynthesisResult result)
        {
            ThrowIfNull(result);
            ThrowIfNull(result.resultHandle, "Invalid result handle.");

            CancellationReason reason;
            ThrowIfFail(Internal.SynthesisResult.synth_result_get_reason_canceled(result.resultHandle, out reason));
            this.Reason = reason;

            CancellationErrorCode errorCode;
            ThrowIfFail(Internal.SynthesisResult.synth_result_get_canceled_error_code(result.resultHandle, out errorCode));
            this.ErrorCode = errorCode;

            this.ErrorDetails = result.Properties.GetProperty(PropertyId.CancellationDetails_ReasonDetailedText);
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        internal SpeechSynthesisCancellationDetails(AudioDataStream stream)
        {
            ThrowIfNull(stream);
            ThrowIfNull(stream.streamHandle, "Invalid result handle.");

            CancellationReason reason;
            ThrowIfFail(Internal.AudioDataStream.audio_data_stream_get_reason_canceled(stream.streamHandle, out reason));
            this.Reason = reason;

            CancellationErrorCode errorCode;
            ThrowIfFail(Internal.AudioDataStream.audio_data_stream_get_canceled_error_code(stream.streamHandle, out errorCode));
            this.ErrorCode = errorCode;

            this.ErrorDetails = stream.Properties.GetProperty(PropertyId.CancellationDetails_ReasonDetailedText);
        }

        /// <summary>
        /// The reason the synthesis was canceled.
        /// </summary>
        public CancellationReason Reason { get; private set; }

        /// <summary>
        /// The error code in case of an unsuccessful synthesis (<see cref="Reason"/> is set to Error).
        /// If Reason is not Error, ErrorCode returns NoError.
        /// </summary>
        public CancellationErrorCode ErrorCode { get; private set; }

        /// <summary>
        /// The error message in case of an unsuccessful synthesis (<see cref="Reason"/> is set to Error).
        /// </summary>
        public string ErrorDetails { get; private set; }

        /// <summary>
        /// Returns a string that represents the cancellation details.
        /// </summary>
        /// <returns>A string that represents the cancellation details.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "Reason:{0} ErrorCode:{1} ErrorDetails:<{2}>", Reason, ErrorCode, ErrorDetails);
        }
    }
}
