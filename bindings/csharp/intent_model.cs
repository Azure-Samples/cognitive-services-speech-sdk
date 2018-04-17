//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
using System;

namespace Microsoft.CognitiveServices.Speech.Recognition.Intent
{
    /// <summary>
    /// Represents intent model used for intent recognition.
    /// </summary>
    public sealed class IntentModel : IDisposable
    { 
        /// <summary>
        /// Creates an intent model using the specified endpoint.
        /// </summary>
        /// <param name="uri">A string that represents the endpoint of the intent model.</param>
        /// <returns>The intent model being created.</returns>
        public static IntentModel From(string uri)
        {
            return new IntentModel(Microsoft.CognitiveServices.Speech.Internal.LuisModel.From(uri));
        }

        /// <summary>
        /// Creates an intent model using subscription key and application id of Language Understanding service.
        /// </summary>
        /// <param name="subscriptionKey">A string that represents the subscription key.</param>
        /// <param name="appId">A string that represents the application id of Language Understanding service.</param>
        /// <returns>The intent model being created.</returns>
        public static IntentModel From(string subscriptionKey, string appId)
        {
            return new IntentModel(Microsoft.CognitiveServices.Speech.Internal.LuisModel.From(subscriptionKey, appId));
        }

        /// <summary>
        /// Creates an intent model using hostname, subscription key and application id of Language Understanding service.
        /// </summary>
        /// <param name="hostName">A string that represents the host name of of Language Understanding service.</param>
        /// <param name="subscriptionKey">A string that represents the subscription key of Language Understanding service.</param>
        /// <param name="appId">A string that represents the application id of Language Understanding service.</param>
        /// <returns>The intent model being created.</returns>
        public static IntentModel From(string hostName, string subscriptionKey, string appId)
        {
            return new IntentModel(Microsoft.CognitiveServices.Speech.Internal.LuisModel.From(hostName, subscriptionKey, appId));
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


        internal IntentModel(Microsoft.CognitiveServices.Speech.Internal.LuisModel model)
        {
            modelImpl = model;
        }

        internal Microsoft.CognitiveServices.Speech.Internal.LuisModel modelImpl { get; }
    }

    
}
