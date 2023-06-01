// <copyright file="BatchCompletionRequest.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TranscriptionStartedMessage.AzureOpenAI
{
    public class BatchCompletionRequest
    {
        public BatchCompletionRequest(string operationLocation, BatchCompletionRequestStatus status)
        {
            this.OperationLocation = operationLocation;
            this.Status = status;
        }

        public enum BatchCompletionRequestStatus
        {
            None = 0,
            NotRunning,
            Canceled,
            Running,
            Succeeded,
            Failed
        }

        public string OperationLocation { get; set; }

        public BatchCompletionRequestStatus Status { get; set; }

        public static bool IsTerminatedBatchCompletionRequestStatus(BatchCompletionRequestStatus status)
        {
            return status == BatchCompletionRequestStatus.Succeeded || status == BatchCompletionRequestStatus.Failed || status == BatchCompletionRequestStatus.Canceled;
        }
    }
}
