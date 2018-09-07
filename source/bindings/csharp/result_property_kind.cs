//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Diagnostics;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines kind of result properties.
    /// </summary>
    public enum ResultPropertyKind
    {
        Json = Internal.SpeechPropertyId.SpeechServiceResponse_Json,
        LanguageUnderstandingJson = Internal.SpeechPropertyId.SpeechServiceResponse_JsonResult,
        ErrorDetails = Internal.SpeechPropertyId.SpeechServiceResponse_JsonErrorDetails
    }

    /// <summary>
    /// Defines name of result property names.
    /// </summary>
    public class ResultPropertyNames
    {
        /// <summary>
        /// The name of property Json.
        /// </summary>
        public const string Json = "RESULT-Json";

        /// <summary>
        /// The name of property LanguageUnderstandingJson.
        /// </summary>
        public const string LanguageUnderstandingJson = "RESULT-LanguageUnderstandingJson";

        /// <summary>
        /// The name of property ErrorDetails.
        /// </summary>
        public const string ErrorDetails = "RESULT-ErrorDetails";

    }
}
