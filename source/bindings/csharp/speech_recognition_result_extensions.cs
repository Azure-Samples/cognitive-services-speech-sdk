//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Runtime.Serialization.Json;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Extension methods for speech recognition result
    /// </summary>
    public static class SpeechRecognitionResultExtensions
    {
        /// <summary>
        /// Returns best possible recognitions for the result if the recognizer
        /// was created with detailed output format.
        /// </summary>
        /// <param name="result">Recognition result.</param>
        /// <returns>A collection of best recognitions.</returns>
        public static IEnumerable<DetailedSpeechRecognitionResult> Best(this SpeechRecognitionResult result)
        {
            var json = result.Properties.Get(SpeechPropertyId.SpeechServiceResponse_Json);
            using (var stream = new MemoryStream())
            using (var writer = new StreamWriter(stream))
            {
                writer.Write(json);
                writer.Flush();
                stream.Position = 0;

                var ser = new DataContractJsonSerializer(typeof(DetailedSpeechRecognitionResultCollection));
                var best = ((DetailedSpeechRecognitionResultCollection)ser.ReadObject(stream)).NBest;
                if (best == null)
                    return new List<DetailedSpeechRecognitionResult>();
                else
                    return best;
            }
        }
    }
}
