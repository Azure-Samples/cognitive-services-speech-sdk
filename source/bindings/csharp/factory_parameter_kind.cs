//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Diagnostics;

namespace Microsoft.CognitiveServices.Speech
{

    /// <summary>
    /// Defines kind of factory parameters
    /// </summary>
    public enum FactoryParameterKind
    {
        Region = Internal.SpeechPropertyId.SpeechServiceConnection_Region,
        SubscriptionKey = Internal.SpeechPropertyId.SpeechServiceConnection_Key,
        AuthorizationToken = Internal.SpeechPropertyId.SpeechServiceAuthorization_Token,
        Endpoint = Internal.SpeechPropertyId.SpeechServiceConnection_Endpoint
    }

    /// <summary>
    /// Defines name of factory parameters. 
    /// </summary>
    public class FactoryParameterNames
    {
        /// <summary>
        /// The name of parameter for getting/setting region.
        /// </summary>
        public const string Region = "SPEECH-Region";

        /// <summary>
        /// The name of parameter for getting/setting subscription key.
        /// </summary>
        public const string SubscriptionKey = "SPEECH-SubscriptionKey";

        /// <summary>
        /// The name of parameter for getting/setting authorization token.
        /// </summary>
        public const string AuthorizationToken = "SPEECH-AuthToken";

        /// <summary>
        /// The name of parameter for getting/setting endpoint.
        /// </summary>
        public const string Endpoint = "SPEECH-Endpoint";

    }

}
