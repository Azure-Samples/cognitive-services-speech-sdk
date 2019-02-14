//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Text;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;

namespace Microsoft.CognitiveServices.Speech.Intent
{
    /// <summary>
    /// Defines result of intent recognition.
    /// </summary>
    public sealed class IntentRecognitionResult : RecognitionResult
    {
        internal IntentRecognitionResult(IntPtr resultPtr)
            : base(resultPtr)
        {
            IntentId = SpxFactory.GetDataFromHandleUsingDelegate(Internal.RecognitionResult.intent_result_get_intent_id, resultHandle, maxCharCount);
        }

        /// <summary>
        /// A string that represents the intent identifier being recognized.
        /// </summary>
        public string IntentId { get; }

        /// <summary>
        /// Returns a string that represents the intent recognition result.
        /// </summary>
        /// <returns>A string that represents the intent recognition result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "ResultId:{0} Reason:{1} IntentId:<{2}> Recognized text:<{3}> Recognized Json:{4}. LanguageUnderstandingJson:{5}",
                ResultId, Reason, IntentId, Text, Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult), Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult));
        }

    }
}
