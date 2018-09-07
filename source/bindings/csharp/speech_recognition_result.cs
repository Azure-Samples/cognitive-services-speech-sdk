//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
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
            Trace.Assert((int)RecognitionStatus.Recognized == (int)Internal.Reason.Recognized);
            Trace.Assert((int)RecognitionStatus.IntermediateResult == (int)Internal.Reason.IntermediateResult);
            Trace.Assert((int)RecognitionStatus.NoMatch == (int)Internal.Reason.NoMatch);
            Trace.Assert((int)RecognitionStatus.InitialSilenceTimeout == (int)Internal.Reason.InitialSilenceTimeout);
            Trace.Assert((int)RecognitionStatus.InitialBabbleTimeout == (int)Internal.Reason.InitialBabbleTimeout);
            Trace.Assert((int)RecognitionStatus.Canceled == (int)Internal.Reason.Canceled);

            resultImpl = result;
            this.ResultId = result.ResultId;
            this.Text = result.Text;
            this.RecognitionStatus = (RecognitionStatus)((int)result.Reason);
            Properties = new ResultPropertiesImpl(result.Properties);
        }

        /// <summary>
        /// Specifies the result identifier.
        /// </summary>
        public string ResultId { get; }

        /// <summary>
        /// Specifies status of speech recognition result.
        /// </summary>
        public RecognitionStatus RecognitionStatus { get; }

        /// <summary>
        /// Presents the recognized text in the result.
        /// </summary>
        public string Text { get; }

        /// <summary>
        /// Duration of the recognized speech.
        /// </summary>
        public TimeSpan Duration => TimeSpan.FromTicks((long)this.resultImpl.Duration());

        /// <summary>
        /// Offset of the recognized speech in ticks. A single tick represents one hundred nanoseconds or one ten-millionth of a second.
        /// </summary>
        public long OffsetInTicks => (long)this.resultImpl.Offset();

        /// <summary>
        /// In case of an unsuccessful recognition, provides a brief description of an occurred error.
        /// This field is only filled-out if the recognition status (<see cref="RecognitionStatus"/>) is set to Canceled.
        /// </summary>
        public string RecognitionFailureReason { get { return Properties.Get(ResultPropertyKind.ErrorDetails); } }

        /// <summary>
        /// Contains properties of the results.
        /// </summary>
        public IResultProperties Properties;

        /// <summary>
        /// Returns a string that represents the speech recognition result.
        /// </summary>
        /// <returns>A string that represents the speech recognition result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"ResultId:{0} Status:{1} Recognized text:<{2}>. Json:{3}", 
                ResultId, RecognitionStatus, Text, Properties.Get(ResultPropertyKind.Json));
        }

        // Hold the reference.
        private Internal.RecognitionResult resultImpl;

        private class ResultPropertiesImpl : IResultProperties
        {
            private Internal.ResultPropertyCollection resultPropertyImpl;

            public ResultPropertiesImpl(Internal.ResultPropertyCollection internalProperty)
            {
                resultPropertyImpl = internalProperty; 
            }
            
            public string Get(ResultPropertyKind propertyKind)
            {
                return Get(propertyKind, string.Empty);
            }

            public string Get(string propertyName)
            {
                return Get(propertyName, string.Empty);
            }

            public string Get(ResultPropertyKind propertyKind, string defaultValue)
            {
                return resultPropertyImpl.GetProperty((Internal.SpeechPropertyId)propertyKind, defaultValue);
            }

            public string Get(string propertyName, string defaultValue)
            {
                return resultPropertyImpl.GetProperty(propertyName, defaultValue);
            }

        }
    }
}
