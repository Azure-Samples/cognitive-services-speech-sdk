//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.Serialization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Collection of best recognitions.
    /// </summary>
    [DataContract]
    internal sealed class DetailedSpeechRecognitionResultCollection
    {
        /// <summary>
        /// Enumerable of alternative interpretations of the same speech recognition result.
        /// </summary>
        [DataMember]
        public IEnumerable<DetailedSpeechRecognitionResult> NBest { get; set; }
    }

    /// <summary>
    /// Detailed recognition result.
    /// </summary>
    [DataContract]
    public sealed class DetailedSpeechRecognitionResult
    {
        internal DetailedSpeechRecognitionResult()
        { }

        internal DetailedSpeechRecognitionResult(double confidence, string text, string lexicalForm, string normalizedForm)
        {
            this.Confidence = confidence;
            this.Text = text;
            this.LexicalForm = lexicalForm;
            this.NormalizedForm = normalizedForm;
            this.MaskedNormalizedForm = normalizedForm;
        }

        /// <summary>
        /// Confidence of recognition.
        /// </summary>
        [DataMember]
        public double Confidence { get; private set; }

        /// <summary>
        /// Recognized text.
        /// </summary>
        [DataMember(Name = "Display")]
        public string Text { get; private set; }

        /// <summary>
        /// Raw lexical form.
        /// </summary>
        [DataMember(Name = "Lexical")]
        public string LexicalForm { get; private set; }

        /// <summary>
        /// Normalized form.
        /// </summary>
        [DataMember(Name = "ITN")]
        public string NormalizedForm { get; private set; }

        /// <summary>
        /// Normalized form with masked profanity.
        /// </summary>
        [DataMember(Name = "MaskedITN")]
        public string MaskedNormalizedForm { get; private set; }
    }
}
