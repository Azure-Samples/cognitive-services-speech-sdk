// <copyright file="TranscriptionAPIRequest.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Collections.Generic;
    using System.Threading.Tasks;
    using Microsoft.Extensions.Logging;

    public static class TranscriptionAPIRequest
    {
        private const string Description = "AcceleratorTranscription";

        public static async Task<Uri> PostRequest(Subscription subscription, string fileName, string sas, string locale, Dictionary<string, string> properties, IEnumerable<Guid> modelList, ILogger logger)
        {
            if (subscription == null)
            {
                throw new ArgumentNullException(nameof(subscription));
            }

            var client = new BatchClient(subscription.SubscriptionKey, subscription.LocationUri.AbsoluteUri, logger);
            var transcriptionLocation = await client.PostTranscriptionAsync(fileName, Description, locale, properties, new Uri(sas), modelList).ConfigureAwait(false);

            return transcriptionLocation;
        }
    }
}
