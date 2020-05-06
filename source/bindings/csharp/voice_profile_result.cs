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
    /// Contains the result of processing a voice profile.
    /// Added in version 1.12.0
    /// </summary>
    public class VoiceProfileResult
    {
        internal VoiceProfileResult(IntPtr resultHandlePtr)
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
        }

        /// <summary>
        /// Specifies the result identifier.
        /// </summary>
        public string ResultId { get; }

        /// <summary>
        /// Specifies the reason of voice profile result.
        /// </summary>
        public ResultReason Reason { get; }

        /// <summary>
        /// Contains properties of the results.
        /// </summary>
        public PropertyCollection Properties { get; private set; }

        /// <summary>
        /// Returns a string that represents the voice profile result.
        /// </summary>
        /// <returns>A string that represents the voice profile result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"ResultId:{0} Reason:{1} Recognized Json:{2}",
                ResultId, Reason, Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult));
        }

        internal InteropSafeHandle resultHandle;
        internal const Int32 maxCharCount = 2048;
    }

    /// <summary>
    /// Contains detailed information about why a voice profile action was canceled.
    /// </summary>
    public class VoiceProfileCancellationDetails
    {
        /// <summary>
        /// Creates an instance of VoiceProfileCancellationDetails object for the canceled VoiceProfileResult.
        /// </summary>
        /// <param name="result">The result that was canceled.</param>
        /// <returns>The CancellationDetails object being created.</returns>
        public static VoiceProfileCancellationDetails FromResult(VoiceProfileResult result)
        {
            return new VoiceProfileCancellationDetails(result);
        }

        internal VoiceProfileCancellationDetails(VoiceProfileResult result)
        {
            ThrowIfNull(result);
            voiceProfileResult = result;
            ThrowIfNull(voiceProfileResult.resultHandle, "Invalid result handle.");

            CancellationReason reason;
            ThrowIfFail(Internal.CancellationDetails.result_get_reason_canceled(voiceProfileResult.resultHandle, out reason));
            this.Reason = reason;

            CancellationErrorCode errorCode;
            ThrowIfFail(Internal.CancellationDetails.result_get_canceled_error_code(voiceProfileResult.resultHandle, out errorCode));
            this.ErrorCode = errorCode;

            this.ErrorDetails = voiceProfileResult.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonErrorDetails);
        }

        /// <summary>
        /// The reason the voice profile action was canceled.
        /// </summary>
        public CancellationReason Reason { get; private set; }

        /// <summary>
        /// The error code in case of an unsuccessful voice profile action (<see cref="Reason"/> is set to Error).
        /// </summary>
        public CancellationErrorCode ErrorCode { get; private set; }

        /// <summary>
        /// The error message in case of an unsuccessful voice profile action (<see cref="Reason"/> is set to Error).
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

        private VoiceProfileResult voiceProfileResult = null;
    }
}
