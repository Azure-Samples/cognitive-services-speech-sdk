// <copyright file="AzureOpenAIRequests.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TranscriptionStartedMessage.AzureOpenAI
{
    public class AzureOpenAIRequests
    {
        public AzureOpenAIRequests(BatchCompletionRequest batchCompletionRequest)
        {
            this.BatchCompletionRequest = batchCompletionRequest;
        }

        public BatchCompletionRequest BatchCompletionRequest { get; set; }
    }
}
