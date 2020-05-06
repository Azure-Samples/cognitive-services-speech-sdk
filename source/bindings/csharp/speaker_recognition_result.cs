//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Contains detailed information about result of a speaker recognition operation.
    /// Added in 1.12.0
    /// </summary>
    public class SpeakerRecognitionResult
    {
        internal SpeakerRecognitionResult(IntPtr resultHandlePtr)
        {
            ThrowIfNull(resultHandlePtr);
            resultHandle = new InteropSafeHandle(resultHandlePtr, Internal.RecognitionResult.recognizer_result_handle_release);
            this.ResultId = SpxFactory.GetDataFromHandleUsingDelegate(Internal.RecognitionResult.result_get_result_id, resultHandle, maxCharCount);

            ResultReason resultReason = ResultReason.NoMatch;
            ThrowIfFail(Internal.RecognitionResult.result_get_reason(resultHandle, ref resultReason));
            this.Reason = resultReason;

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.RecognitionResult.result_get_property_bag(resultHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);

            ProfileId = Properties.GetProperty("speakerrecognition.profileid", "");
            NumberFormatInfo provider = CultureInfo.CurrentCulture.NumberFormat;
            Score = Convert.ToDouble(Properties.GetProperty("speakerrecognition.score", "0.0"), provider);
        }

        /// <summary>
        /// Specifies the result identifier.
        /// </summary>
        public string ResultId { get; }

        /// <summary>
        /// Specifies status of speaker recognition result.
        /// </summary>
        public ResultReason Reason { get; }

        /// <summary>
        /// Presents the recognized profile id.
        /// </summary>
        public string ProfileId { get; }

        /// <summary>
        /// Presents the similarity score of the recognized speaker.
        /// The score is a float number indicating the similarity between input audio and targeted voice profile.This number is between 0 and 1. A higher number means higher similarity.
        /// </summary>
        public double Score { get; }

        /// <summary>
        /// Contains properties of the results.
        /// </summary>
        public PropertyCollection Properties { get; private set; }

        /// <summary>
        /// Returns a string that represents the speaker recognition result.
        /// </summary>
        /// <returns>A string that represents the speaker recognition result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"ResultId:{0} Reason:{1} Recognized ProfileId:<{2}>. Json:{3}",
                ResultId, Reason, ProfileId, Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult));
        }

        internal InteropSafeHandle resultHandle;
        internal const Int32 maxCharCount = 2048;
    }

    /// <summary>
    /// Contains detailed information about why a result was canceled.
    /// Added in version 1.12.0
    /// </summary>
    public class SpeakerRecognitionCancellationDetails
    {
        /// <summary>
        /// Contains the detailed information about why a speaker recognition result was canceled.
        /// </summary>
        /// <param name="result">The result that was canceled.</param>
        /// <returns>The CancellationDetails object being created.</returns>
        public static SpeakerRecognitionCancellationDetails FromResult(SpeakerRecognitionResult result)
        {
            return new SpeakerRecognitionCancellationDetails(result);
        }

        internal SpeakerRecognitionCancellationDetails(SpeakerRecognitionResult result)
        {
            ThrowIfNull(result);
            speakerRecognitionResult = result;
            ThrowIfNull(speakerRecognitionResult.resultHandle, "Invalid result handle.");

            CancellationReason reason;
            ThrowIfFail(Internal.CancellationDetails.result_get_reason_canceled(speakerRecognitionResult.resultHandle, out reason));
            this.Reason = reason;

            CancellationErrorCode errorCode;
            ThrowIfFail(Internal.CancellationDetails.result_get_canceled_error_code(speakerRecognitionResult.resultHandle, out errorCode));
            this.ErrorCode = errorCode;

            this.ErrorDetails = speakerRecognitionResult.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonErrorDetails);
        }

        /// <summary>
        /// The reason the recognition was canceled.
        /// </summary>
        public CancellationReason Reason { get; private set; }

        /// <summary>
        /// The error code in case of an unsuccessful recognition (<see cref="Reason"/> is set to Error).
        /// If Reason is not Error, ErrorCode returns NoError.
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

        private SpeakerRecognitionResult speakerRecognitionResult = null;
    }
}
