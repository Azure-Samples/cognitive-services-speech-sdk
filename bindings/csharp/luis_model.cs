//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
using System;

namespace Carbon.Recognition.Intent
{
    /// <summary>
    /// Represents LUIS model used for intent recognition.
    /// </summary>
    public sealed class LuisModel : IDisposable
    { 
        /// <summary>
        /// Creates a LUIS model using the specified endpoint.
        /// </summary>
        /// <param name="uri">A string that represents the endpoint of the LUIS model.</param>
        /// <returns>The LUIS model being created.</returns>
        public static LuisModel From(string uri)
        {
            return new LuisModel(Carbon.Internal.LuisModel.From(uri));
        }

        /// <summary>
        /// Creates a LUIS model using subscription key and application id.
        /// </summary>
        /// <param name="subscriptionKey">A string that represents the subscription key.</param>
        /// <param name="appId">A string that represents the application id.</param>
        /// <returns>The LUIS model being created.</returns>
        public static LuisModel From(string subscriptionKey, string appId)
        {
            return new LuisModel(Carbon.Internal.LuisModel.From(subscriptionKey, appId));
        }

        /// <summary>
        /// Creates a LUIS model using hostname, subscription key and application id.
        /// </summary>
        /// <param name="hostName">A string that represents the host name of the LUIS model.</param>
        /// <param name="subscriptionKey">A string that represents the subscription key.</param>
        /// <param name="appId">A string that represents the application id.</param>
        /// <returns>The LUIS model being created.</returns>
        public static LuisModel From(string hostName, string subscriptionKey, string appId)
        {
            return new LuisModel(Carbon.Internal.LuisModel.From(hostName, subscriptionKey, appId));
        }

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


        internal LuisModel(Carbon.Internal.LuisModel model)
        {
            modelImpl = model;
        }

        internal Carbon.Internal.LuisModel modelImpl { get; }
    }

    
}
