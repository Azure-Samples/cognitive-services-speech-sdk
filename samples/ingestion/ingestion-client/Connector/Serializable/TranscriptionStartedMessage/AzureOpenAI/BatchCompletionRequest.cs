// <copyright file="BatchCompletionRequest.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TranscriptionStartedMessage.AzureOpenAI
{
    public class BatchCompletionRequest
    {
        public BatchCompletionRequest(string operationLocation)
        {
            this.OperationLocation = operationLocation;
        }

        public string OperationLocation { get; set; }
    }
}
