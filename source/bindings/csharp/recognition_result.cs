//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Diagnostics.Contracts;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Contains detailed information about result of a recognition operation.
    /// </summary>
    public class RecognitionResult
    {
        internal RecognitionResult(IntPtr resultHandlePtr)
        {
            ThrowIfNull(resultHandlePtr);
            resultHandle = new InteropSafeHandle(resultHandlePtr, Internal.RecognitionResult.recognizer_result_handle_release);
            this.ResultId = SpxFactory.GetDataFromHandleUsingDelegate(Internal.RecognitionResult.result_get_result_id, resultHandle, maxCharCount);
            this.Text = SpxFactory.GetDataFromHandleUsingDelegate(Internal.RecognitionResult.result_get_text, resultHandle, maxCharCount);

            ResultReason resultReason = ResultReason.NoMatch;
            ThrowIfFail(Internal.RecognitionResult.result_get_reason(resultHandle, ref resultReason));
            this.Reason = resultReason;

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.RecognitionResult.result_get_property_bag(resultHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        /// <summary>
        /// Specifies the result identifier.
        /// </summary>
        public string ResultId { get; }

        /// <summary>
        /// Specifies status of speech recognition result.
        /// </summary>
        public ResultReason Reason { get; }

        /// <summary>
        /// Presents the recognized text in the result.
        /// </summary>
        public string Text { get; }

        /// <summary>
        /// Duration of the recognized speech.
        /// </summary>
        public TimeSpan Duration => TimeSpan.FromTicks((long)GetDuration());

        /// <summary>
        /// Offset of the recognized speech in ticks. A single tick represents one hundred nanoseconds or one ten-millionth of a second.
        /// </summary>
        public long OffsetInTicks => (long) GetOffset();
            
        /// <summary>
        /// Contains properties of the results.
        /// </summary>
        public PropertyCollection Properties { get; private set; }

        /// <summary>
        /// Returns a string that represents the speech recognition result.
        /// </summary>
        /// <returns>A string that represents the speech recognition result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"ResultId:{0} Reason:{1} Recognized text:<{2}>. Json:{3}",
                ResultId, Reason, Text, Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult));
        }

        private ulong GetDuration()
        {
            ThrowIfNull(resultHandle);
            UInt64 duration = 0;
            ThrowIfFail(Internal.RecognitionResult.result_get_duration(resultHandle, ref duration));
            return duration;
        }

        private ulong GetOffset()
        {
            ThrowIfNull(resultHandle);
            UInt64 offset = 0;
            ThrowIfFail(Internal.RecognitionResult.result_get_offset(resultHandle, ref offset));
            return offset;
        }

        internal InteropSafeHandle resultHandle;
        internal const Int32 maxCharCount = 1024;
    }

    /// <summary>
    /// Contains detailed information about why a result was canceled.
    /// </summary>
    public class CancellationDetails
    {
        /// <summary>
        /// Creates an instance of CancellationDetails object for the canceled SpeechRecognitionResult.
        /// </summary>
        /// <param name="result">The result that was canceled.</param>
        /// <returns>The CancellationDetails object being created.</returns>
        public static CancellationDetails FromResult(RecognitionResult result)
        {
            return new CancellationDetails(result);
        }

        internal CancellationDetails(RecognitionResult result)
        {
            ThrowIfNull(result);
            recognitionResult = result;
            ThrowIfNull(recognitionResult.resultHandle, "Invalid result handle.");

            CancellationReason reason;
            ThrowIfFail(Internal.CancellationDetails.result_get_reason_canceled(recognitionResult.resultHandle, out reason));
            this.Reason = reason;

            CancellationErrorCode errorCode;
            ThrowIfFail(Internal.CancellationDetails.result_get_canceled_error_code(recognitionResult.resultHandle, out errorCode));
            this.ErrorCode = errorCode;

            this.ErrorDetails = recognitionResult.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonErrorDetails);
        }

        /// <summary>
        /// The reason the recognition was canceled.
        /// </summary>
        public CancellationReason Reason { get; private set; }

        /// <summary>
        /// The error code in case of an unsuccessful recognition (<see cref="Reason"/> is set to Error).
        /// If Reason is not Error, ErrorCode returns NoError.
        /// Added in version 1.1.0.
        /// </summary>
        public CancellationErrorCode ErrorCode { get; private set; }

        /// <summary>
        /// The error message in case of an unsuccessful recognition (<see cref="Reason"/> is set to Error).
        /// </summary>
        public string ErrorDetails { get; private set; }

        /// <summary>
        /// Returns a string that represents the cancellation details.
        /// </summary>
        /// <returns>A string that represents the cancellation details.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"Reason:{0} ErrorDetails:<{1}>", Reason, ErrorDetails);
        }

        private RecognitionResult recognitionResult = null;
    }

    /// <summary>
    /// Contains detailed information for NoMatch recognition results.
    /// </summary>
    public class NoMatchDetails
    {
        /// <summary>
        /// Creates an instance of NoMatchDetails object for NoMatch RecognitionResults.
        /// </summary>
        /// <param name="result">The recognition result that was not recognized.</param>
        /// <returns>The NoMatchDetails object being created.</returns>
        public static NoMatchDetails FromResult(RecognitionResult result)
        {
            return new NoMatchDetails(result);
        }

        internal NoMatchDetails(RecognitionResult result)
        {
            ThrowIfNull(result);
            recognitionResult = result;
            ThrowIfNull(recognitionResult.resultHandle, "Invalid result handle.");
            NoMatchReason reason = NoMatchReason.NotRecognized;
            ThrowIfFail(Internal.NoMatchDetails.result_get_no_match_reason(recognitionResult.resultHandle, ref reason));
            this.Reason = reason;
        }

        /// <summary>
        /// The reason the result was not recognized.
        /// </summary>
        public NoMatchReason Reason { get; private set; }

        /// <summary>
        /// Returns a string that represents the cancellation details.
        /// </summary>
        /// <returns>A string that represents the cancellation details.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "NoMatchReason:{0}", Reason);
        }

        private RecognitionResult recognitionResult = null;
    }
}
