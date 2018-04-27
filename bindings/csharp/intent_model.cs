//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;

namespace Microsoft.CognitiveServices.Speech.Intent
{
    /// <summary>
    /// Represents language understanding model used for intent recognition.
    /// </summary>
    public sealed class LanguageUnderstandingModel : IDisposable
    { 
        /// <summary>
        /// Creates an language understanding model using the specified endpoint.
        /// </summary>
        /// <param name="uri">A string that represents the endpoint of the language understanding model.</param>
        /// <returns>The language understanding model being created.</returns>
        public static LanguageUnderstandingModel From(string uri)
        {
            return new LanguageUnderstandingModel(Microsoft.CognitiveServices.Speech.Internal.LanguageUnderstandingModel.From(uri));
        }

        /// <summary>
        /// Creates an language understanding model using subscription key and application id of Language Understanding service.
        /// </summary>
        /// <param name="subscriptionKey">A string that represents the subscription key.</param>
        /// <param name="appId">A string that represents the application id of Language Understanding service.</param>
        /// <returns>The language understanding model being created.</returns>
        public static LanguageUnderstandingModel From(string subscriptionKey, string appId)
        {
            return new LanguageUnderstandingModel(Microsoft.CognitiveServices.Speech.Internal.LanguageUnderstandingModel.From(subscriptionKey, appId));
        }

        /// <summary>
        /// Creates an language understanding model using hostname, subscription key and application id of Language Understanding service.
        /// </summary>
        /// <param name="hostName">A string that represents the host name of of Language Understanding service.</param>
        /// <param name="subscriptionKey">A string that represents the subscription key of Language Understanding service.</param>
        /// <param name="appId">A string that represents the application id of Language Understanding service.</param>
        /// <returns>The language understanding model being created.</returns>
        public static LanguageUnderstandingModel From(string hostName, string subscriptionKey, string appId)
        {
            return new LanguageUnderstandingModel(Microsoft.CognitiveServices.Speech.Internal.LanguageUnderstandingModel.From(hostName, subscriptionKey, appId));
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            if (disposed)
            {
                return;
            }

            modelImpl.Dispose();
            disposed = true;
        }

        private bool disposed = false;


        internal LanguageUnderstandingModel(Microsoft.CognitiveServices.Speech.Internal.LanguageUnderstandingModel model)
        {
            modelImpl = model;
        }

        internal Microsoft.CognitiveServices.Speech.Internal.LanguageUnderstandingModel modelImpl { get; }
    }

    
}
