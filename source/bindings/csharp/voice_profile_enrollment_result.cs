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
    /// Contains the result of enrolling a voice profile.
    /// Added in version 1.12.0
    /// </summary>
    public class VoiceProfileEnrollmentResult
    {
        internal VoiceProfileEnrollmentResult(IntPtr resultHandlePtr)
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

            ProfileId = Properties.GetProperty("enrollment.profileId", "");
            NumberFormatInfo provider = CultureInfo.CurrentCulture.NumberFormat;
            EnrollmentsCount = Convert.ToInt32(Properties.GetProperty("enrollment.enrollmentsCount", "0"), provider);
            EnrollmentsLength = TimeSpan.FromTicks((long)Convert.ToUInt64(Properties.GetProperty("enrollment.enrollmentsLength", "0"), provider));
            EnrollmentsSpeechLength = TimeSpan.FromTicks((long)Convert.ToUInt64(Properties.GetProperty("enrollment.enrollmentsSpeechLength", "0"), provider));
            if(Int32.TryParse(Properties.GetProperty("enrollment.remainingEnrollmentsCount"), out int count))
            {
                RemainingEnrollmentsCount = count;
            }
            if(UInt64.TryParse(Properties.GetProperty("enrollment.remainingEnrollmentsSpeechLength"), out ulong length))
            {
                RemainingEnrollmentsSpeechLength = TimeSpan.FromTicks((long)length);
            }            
            AudioLength = TimeSpan.FromTicks((long)Convert.ToUInt64(Properties.GetProperty("enrollment.audioLength", "0"), provider));
            AudioSpeechLength = TimeSpan.FromTicks((long)Convert.ToUInt64(Properties.GetProperty("enrollment.audioSpeechLength", "0"), provider));
        }

        /// <summary>
        /// Specifies the result identifier.
        /// </summary>
        public string ResultId { get; }

        /// <summary>
        /// Specifies reason of the enrollment result.
        /// </summary>
        public ResultReason Reason { get; }

        /// <summary>
        /// Specifies profile id in the enrollment result.
        /// </summary>
        public string ProfileId { get; private set; }

        /// <summary>
        /// Number of enrollment audios accepted for this profile.
        /// </summary>
        public Int32 EnrollmentsCount { get; private set; }

        /// <summary>
        /// The total length of enrollment audios accepted for this profile in hundred nanoseconds.
        /// </summary>
        public TimeSpan EnrollmentsLength { get; private set; }

        /// <summary>
        /// Number of enrollment audios needed to complete profile enrollment.
        /// </summary>
        public Int32? RemainingEnrollmentsCount { get; private set; }

        /// <summary>
        /// The amount of pure speech (which is the amount of audio after removing silence and non-speech segments) needed to complete profile enrollment in hundred nanoseconds.
        /// </summary>
        public TimeSpan? RemainingEnrollmentsSpeechLength { get; private set; }

        /// <summary>
        /// The summation of pure speech(which is the amount of audio after removing silence and non - speech segments) across all profile enrollments in hundred nanoseconds.
        /// </summary>
        public TimeSpan EnrollmentsSpeechLength { get; private set; }

        /// <summary>
        /// This enrollment audio length in hundred nanoseconds.
        /// </summary>
        public TimeSpan AudioLength { get; private set; }

        /// <summary>
        /// This enrollment audio pure speech(which is the amount of audio after removing silence and non - speech segments) length in hundred nanoseconds.
        /// </summary>
        public TimeSpan AudioSpeechLength { get; private set; }

        /// <summary>
        /// Contains properties of the results.
        /// </summary>
        public PropertyCollection Properties { get; private set; }

        /// <summary>
        /// Returns a string that represents the enrollment result.
        /// </summary>
        /// <returns>A string that represents the enrollment result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"ResultId:{0} Reason:{1} Recognized Json:{2}",
                ResultId, Reason, Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult));
        }

        internal InteropSafeHandle resultHandle;
        internal const Int32 maxCharCount = 2048;
    }

    /// <summary>
    /// Represents the cancellation details of an enrollment result.
    /// Added in version 1.12.0.
    /// </summary>
    public class VoiceProfileEnrollmentCancellationDetails
    {
        /// <summary>
        /// Create an object that represents the details of a canceled enrollment result.
        /// </summary>
        /// <param name="result">a voice profile enrollment result object.</param>
        /// <returns>a voice profile enrollment cancellation details object.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1062", Justification = "Null is OK, checked in constructor.")]
        public static VoiceProfileEnrollmentCancellationDetails FromResult(VoiceProfileEnrollmentResult result)
        {
            return new VoiceProfileEnrollmentCancellationDetails(result);
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        internal VoiceProfileEnrollmentCancellationDetails(VoiceProfileEnrollmentResult result)
        {
            ThrowIfNull(result);
            voiceProfileEnrollmentResult = result;
            ThrowIfNull(voiceProfileEnrollmentResult.resultHandle, "Invalid result handle.");

            CancellationReason reason;
            ThrowIfFail(Internal.CancellationDetails.result_get_reason_canceled(voiceProfileEnrollmentResult.resultHandle, out reason));
            this.Reason = reason;

            CancellationErrorCode errorCode;
            ThrowIfFail(Internal.CancellationDetails.result_get_canceled_error_code(voiceProfileEnrollmentResult.resultHandle, out errorCode));
            this.ErrorCode = errorCode;

            this.ErrorDetails = voiceProfileEnrollmentResult.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonErrorDetails);
        }

        /// <summary>
        /// The reason the enrollment was canceled.
        /// </summary>
        public CancellationReason Reason { get; private set; }

        /// <summary>
        /// The error code in case of an unsuccessful enrollment (<see cref="Reason"/> is set to Error).
        /// </summary>
        public CancellationErrorCode ErrorCode { get; private set; }

        /// <summary>
        /// The error message in case of an unsuccessful enrollment (<see cref="Reason"/> is set to Error).
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

        private VoiceProfileEnrollmentResult voiceProfileEnrollmentResult = null;
    }
}
