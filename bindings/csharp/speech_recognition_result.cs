//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.Diagnostics;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines result of speech recognition.
    /// </summary>
    public class SpeechRecognitionResult
    {
        internal SpeechRecognitionResult(Internal.RecognitionResult result)
        {
            Trace.Assert((int)SpeechRecognitionStatus.Recognized == (int)Internal.Reason.Recognized);
            Trace.Assert((int)SpeechRecognitionStatus.IntermediateResult == (int)Internal.Reason.IntermediateResult);
            Trace.Assert((int)SpeechRecognitionStatus.NoMatch == (int)Internal.Reason.NoMatch);
            Trace.Assert((int)SpeechRecognitionStatus.Canceled == (int)Internal.Reason.Canceled);
            Trace.Assert((int)SpeechRecognitionStatus.OtherRecognizer == (int)Internal.Reason.OtherRecognizer);

            this.ResultId = result.ResultId;
            this.RecognizedText = result.Text;
            this.RecognitionStatus = (SpeechRecognitionStatus)((int)result.Reason);
            this.properties = result.Properties;
        }

        /// <summary>
        /// Specifies the result identifier.
        /// </summary>
        public string ResultId { get; }

        /// <summary>
        /// Specifies status of speech recognition result.
        /// </summary>
        public SpeechRecognitionStatus RecognitionStatus { get; }

        /// <summary>
        /// Presents the recognized text in the result.
        /// </summary>
        public string RecognizedText { get; }

        /// <summary>
        /// In case of an unsuccessful recognition, provides a brief description of an occurred error.
        /// This field is only filled-out if the recognition status (<see cref="RecognitionStatus"/>) is set to Canceled.
        /// </summary>
        public string RecognitionFailureReason { get { return properties.Get(Internal.ResultProperty.ErrorDetails).GetString(); } }

        /// <summary>
        /// A string containing Json serialized recognition result as it was received from the service.
        /// </summary>
        public string Json { get { return properties.Get(Internal.ResultProperty.Json).GetString(); } }

        /// <summary>
        /// Returns a string that represents the speech recognition result.
        /// </summary>
        /// <returns>A string that represents the speech recognition result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"ResultId:{0} Status:{1} Recognized text:<{2}>.", ResultId, RecognitionStatus, RecognizedText);
        }

        private Internal.ResultPropertyValueCollection properties;
    }
}
