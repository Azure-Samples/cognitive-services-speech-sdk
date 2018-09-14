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
            Trace.Assert((int)ResultReason.NoMatch == (int)Internal.ResultReason.NoMatch);
            Trace.Assert((int)ResultReason.Canceled == (int)Internal.ResultReason.Canceled);
            Trace.Assert((int)ResultReason.RecognizingSpeech == (int)Internal.ResultReason.RecognizingSpeech);
            Trace.Assert((int)ResultReason.RecognizedSpeech == (int)Internal.ResultReason.RecognizedSpeech);
            Trace.Assert((int)ResultReason.RecognizingIntent == (int)Internal.ResultReason.RecognizingIntent);
            Trace.Assert((int)ResultReason.RecognizedIntent == (int)Internal.ResultReason.RecognizedIntent);
            Trace.Assert((int)ResultReason.TranslatingSpeech == (int)Internal.ResultReason.TranslatingSpeech);
            Trace.Assert((int)ResultReason.TranslatedSpeech == (int)Internal.ResultReason.TranslatedSpeech);
            Trace.Assert((int)ResultReason.SynthesizingAudio == (int)Internal.ResultReason.SynthesizingAudio);
            Trace.Assert((int)ResultReason.SynthesizingAudioComplete == (int)Internal.ResultReason.SynthesizingAudioComplete);

            resultImpl = result;
            this.ResultId = result.ResultId;
            this.Text = result.Text;
            this.Reason = (ResultReason)((int)result.Reason);
            Properties = new PropertyCollection(result.Properties);
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
        public TimeSpan Duration => TimeSpan.FromTicks((long)this.resultImpl.Duration());

        /// <summary>
        /// Offset of the recognized speech in ticks. A single tick represents one hundred nanoseconds or one ten-millionth of a second.
        /// </summary>
        public long OffsetInTicks => (long)this.resultImpl.Offset();

        /// <summary>
        /// Contains properties of the results.
        /// </summary>
        public PropertyCollection Properties;

        /// <summary>
        /// Returns a string that represents the speech recognition result.
        /// </summary>
        /// <returns>A string that represents the speech recognition result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"ResultId:{0} Reason:{1} Recognized text:<{2}>. Json:{3}", 
                ResultId, Reason, Text, Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult));
        }

        // Hold the reference.
        internal Internal.RecognitionResult resultImpl { get; }
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
        public static CancellationDetails FromResult(SpeechRecognitionResult result)
        {
            var canceledImpl = Internal.CancellationDetails.FromResult(result.resultImpl);
            return new CancellationDetails(canceledImpl);
        }

        internal CancellationDetails(Internal.CancellationDetails cancellation)
        {
            Trace.Assert((int)CancellationReason.Error == (int)Internal.CancellationReason.Error);
            Trace.Assert((int)CancellationReason.EndOfStream == (int)Internal.CancellationReason.EndOfStream);

            canceledImpl = cancellation;
            this.Reason = (CancellationReason)((int)cancellation.Reason);
            this.ErrorDetails = cancellation.ErrorDetails;
        }

        /// <summary>
        /// The reason the recognition was canceled.
        /// </summary>
        public CancellationReason Reason;

        /// <summary>
        /// In case of an unsuccessful recognition, provides a details of why the occurred error.
        /// This field is only filled-out if the reason canceled (<see cref="Reason"/>) is set to Error.
        /// </summary>
        public string ErrorDetails;

        /// <summary>
        /// Returns a string that represents the cancellation details.
        /// </summary>
        /// <returns>A string that represents the cancellation details.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"Reason:{0} ErrorDetails:<{1}>", Reason, ErrorDetails);
        }

        // Hold the reference.
        private Internal.CancellationDetails canceledImpl;
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
        public static NoMatchDetails FromResult(SpeechRecognitionResult result)
        {
            var noMatchImpl = Internal.NoMatchDetails.FromResult(result.resultImpl);
            return new NoMatchDetails(noMatchImpl);
        }

        internal NoMatchDetails(Internal.NoMatchDetails noMatch)
        {
            Trace.Assert((int)NoMatchReason.NotRecognized == (int)Internal.NoMatchReason.NotRecognized);
            Trace.Assert((int)NoMatchReason.InitialSilenceTimeout == (int)Internal.NoMatchReason.InitialSilenceTimeout);
            Trace.Assert((int)NoMatchReason.InitialBabbleTimeout == (int)Internal.NoMatchReason.InitialBabbleTimeout);

            noMatchImpl = noMatch;
            this.Reason = (NoMatchReason)((int)noMatch.Reason);
        }

        /// <summary>
        /// The reason the result was not recognized.
        /// </summary>
        public NoMatchReason Reason;

        /// <summary>
        /// Returns a string that represents the cancellation details.
        /// </summary>
        /// <returns>A string that represents the cancellation details.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "NoMatchReason:{0}", Reason);
        }

        // Hold the reference.
        private Internal.NoMatchDetails noMatchImpl;
    }
}
