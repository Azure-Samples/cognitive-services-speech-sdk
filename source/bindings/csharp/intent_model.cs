//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Intent
{
    /// <summary>
    /// Represents language understanding model used for intent recognition.
    /// </summary>
    public sealed class LanguageUnderstandingModel
    { 
        /// <summary>
        /// Creates an language understanding model using the specified endpoint.
        /// </summary>
        /// <param name="uri">A string that represents the endpoint of the language understanding model.</param>
        /// <returns>The language understanding model being created.</returns>
        public static LanguageUnderstandingModel FromEndpoint(string uri)
        {
            IntPtr luModel = IntPtr.Zero;
            ThrowIfFail(Internal.LanguageUnderstandingModel.language_understanding_model_create_from_uri(out luModel, uri));
            return new LanguageUnderstandingModel(luModel);
        }

        /// <summary>
        /// Creates an language understanding model using the application id of Language Understanding service.
        /// </summary>
        /// <param name="appId">A string that represents the application id of Language Understanding service.</param>
        /// <returns>The language understanding model being created.</returns>
        public static LanguageUnderstandingModel FromAppId(string appId)
        {
            IntPtr luModel = IntPtr.Zero;
            ThrowIfFail(Internal.LanguageUnderstandingModel.language_understanding_model_create_from_app_id(out luModel, appId));
            return new LanguageUnderstandingModel(luModel);
        }

        /// <summary>
        /// Creates an language understanding model using hostname, subscription key and application id of Language Understanding service.
        /// </summary>
        /// <param name="subscriptionKey">A string that represents the subscription key of Language Understanding service.</param>
        /// <param name="appId">A string that represents the application id of Language Understanding service.</param>
        /// <param name="region">A string that represents the region of the Language Understanding service (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>The language understanding model being created.</returns>
        public static LanguageUnderstandingModel FromSubscription(string subscriptionKey, string appId, string region)
        {
            IntPtr luModel = IntPtr.Zero;
            ThrowIfFail(Internal.LanguageUnderstandingModel.language_understanding_model_create_from_subscription(out luModel, subscriptionKey, appId, region));
            return new LanguageUnderstandingModel(luModel);
        }

        private LanguageUnderstandingModel(IntPtr luPtr)
        {
            luHandle = new InteropSafeHandle(luPtr, Internal.LanguageUnderstandingModel.language_understanding_model__handle_release);
        }

        internal InteropSafeHandle luHandle;
    }
}
