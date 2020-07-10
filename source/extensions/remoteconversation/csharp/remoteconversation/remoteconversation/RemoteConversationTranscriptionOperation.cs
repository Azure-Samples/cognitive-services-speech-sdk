//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Azure;
using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Transcription;
using System.Text.Json;

namespace Microsoft.CognitiveServices.Speech.RemoteConversation
{
    /// <summary>
    /// Operation class which represents a long running operaton inside the service.
    /// This is extended from Azure Core Operation https://github.com/Azure/azure-sdk-for-net/blob/master/sdk/core/Azure.Core/src/Operation%7BT%7D.cs
    /// Added in version 1.13.0
    /// <typeparam name="RemoteConversationTranscriptionResult">The final result of the long-running operation.</typeparam>
    /// </summary>
    public class RemoteConversationTranscriptionOperation : Operation<RemoteConversationTranscriptionResult>
    {
        /// <summary>
        /// Constructor to create long running operation for RemoteConversationTranscription
        /// </summary>
        /// <param name="id">Meeting id for the long running transcription in the service.</param>
        /// <param name="config">The speech config object.</param>
        public RemoteConversationTranscriptionOperation(string id, RemoteConversationTranscriptionClient client)
        {
            Id = id;
            m_client = client;
        }
        
        /// <summary>
        /// Gets an ID representing the operation that can be used to poll for
        /// the status of the long-running operation.
        /// </summary>
        public override string Id { get; }

        /// <summary>
        /// Returns true if the long-running operation completed successfully and has produced final result (accessible by Value property).
        /// </summary>
        public override bool HasValue => m_result != null;

        /// <summary>
        /// Final result of the long-running operation.
        /// </summary>
        /// <remarks>
        /// This property can be accessed only after the operation completes successfully (HasValue is true).
        /// </remarks>
        public override RemoteConversationTranscriptionResult Value
        {
            get
            {
                if (m_resultModel is null || m_resultModel.ProcessingStatus == RemoteTaskStatusModel.Deleted)
                {
                    throw new InvalidOperationException("The operation was deleted so no value is available.");
                }

                if (m_resultModel.ProcessingStatus == RemoteTaskStatusModel.Failed)
                {
                    throw new OperationCanceledException("The operation was failed so no value is available.");
                }

                m_result = new RemoteConversationTranscriptionResult();
                m_result.Id = m_resultModel.Id;
                foreach(var item in m_resultModel.Transcriptions)
                {
                    RemoteConversationTranscriptionPropertyCollectionImpl collection = new RemoteConversationTranscriptionPropertyCollectionImpl();
                    var jsonString = JsonSerializer.Serialize<RemoteTranscriptionModel>(item);
                    collection.SetProperty("RESULT-Json", jsonString);
                    ConversationTranscriptionResult result = new RemoteConversationTranscriptionResultImpl(item.Text,
                        item.ResultId,
                        item.UserId,
                        item.reason,
                        item.OffsetInTicks,
                        item.DurationInTicks,
                        collection
                        );
                    m_result.AddTranscription(result);

                }

                return m_result;
            }
        }

        /// <summary>
        /// Returns true if the long-running operation completed.
        /// </summary>
        public override bool HasCompleted => m_completed;

        /// <summary>
        /// The last HTTP response received from the server.
        /// </summary>
        /// <remarks>
        /// The last response returned from the server during the lifecycle of this instance.
        /// An instance of Operation<typeparamref name="RemoteConversationTranscriptionResult"/> sends requests to a server in UpdateStatusAsync, UpdateStatus, and other methods.
        /// Responses from these requests can be accessed using GetRawResponse.
        /// </remarks>
        public override Response GetRawResponse() => m_response;

        /// <summary>
        /// Calls the server to get updated status of the long-running operation.
        /// </summary>
        /// <param name="cancellationToken">A <see cref="CancellationToken"/> used for the service call.</param>
        /// <returns>The HTTP response received from the server.</returns>
        /// <remarks>
        /// This operation will update the value returned from GetRawResponse and might update HasCompleted, HasValue, and Value.
        /// </remarks>
        public override Response UpdateStatus(CancellationToken cancellationToken = default)
        {
            if (!m_completed)
            {
                Response<RemoteConversationTranscriptionModel> pollResponse = m_client.GetTranscriptionResults(Id, cancellationToken);

                if(pollResponse != null)
                {
                    m_response = pollResponse.GetRawResponse();

                    m_resultModel = pollResponse;

                    if (m_resultModel.ProcessingStatus == RemoteTaskStatusModel.Completed
                        || m_resultModel.ProcessingStatus == RemoteTaskStatusModel.Deleted
                        || m_resultModel.ProcessingStatus == RemoteTaskStatusModel.Failed)
                    {
                        m_completed = true;
                    }
                }
            }

            return GetRawResponse();
        }

        /// <summary>
        /// Calls the server to get updated status of the long-running operation.
        /// </summary>
        /// <param name="cancellationToken">A <see cref="CancellationToken"/> used for the service call.</param>
        /// <returns>The HTTP response received from the server.</returns>
        /// <remarks>
        /// This operation will update the value returned from GetRawResponse and might update HasCompleted, HasValue, and Value.
        /// </remarks>
        public override async ValueTask<Response> UpdateStatusAsync(CancellationToken cancellationToken = default)
        {
            if (!m_completed)
            {
                Response<RemoteConversationTranscriptionModel> pollResponse = await m_client.GetTranscriptionResultsAsync(Id, cancellationToken).ConfigureAwait(false);

                if (pollResponse != null)
                {
                    m_response = pollResponse.GetRawResponse();

                    m_resultModel = pollResponse;

                    if (m_resultModel.ProcessingStatus == RemoteTaskStatusModel.Completed
                        || m_resultModel.ProcessingStatus == RemoteTaskStatusModel.Deleted
                        || m_resultModel.ProcessingStatus == RemoteTaskStatusModel.Failed)
                    {
                        m_completed = true;
                    }
                    m_resultModel.Id = Id;
                }
            }
            return GetRawResponse();
        }

        /// <summary>
        /// Periodically calls every 10 seconds to the service till the long-running operation completes.
        /// </summary>
        /// <param name="cancellationToken">A <see cref="CancellationToken"/> used for the periodical service calls.</param>
        /// <returns>The last HTTP response received from the server.</returns>
        /// <remarks>
        /// This method will periodically call UpdateStatusAsync till HasCompleted is true, then return the final result of the operation.
        /// </remarks>
        public override ValueTask<Response<RemoteConversationTranscriptionResult>> WaitForCompletionAsync(CancellationToken cancellationToken = default) =>
            DefaultWaitForCompletionAsync(cancellationToken);
        
        /// <summary>
        /// Periodically calls the server till the long-running operation completes.
        /// </summary>
        /// <param name="pollingInterval">
        /// The interval between status requests to the server.
        /// The interval can change based on information returned from the server.
        /// For example, the server might communicate to the client that there is not reason to poll for status change sooner than some time.
        /// </param>
        /// <param name="cancellationToken">A <see cref="CancellationToken"/> used for the periodical service calls.</param>
        /// <returns>The last HTTP response received from the server.</returns>
        /// <remarks>
        /// This method will periodically call UpdateStatusAsync till HasCompleted is true, then return the final result of the operation.
        /// </remarks>
        public override ValueTask<Response<RemoteConversationTranscriptionResult>> WaitForCompletionAsync(TimeSpan pollingInterval, CancellationToken cancellationToken = default) =>
            DefaultWaitForCompletionAsync(pollingInterval, cancellationToken);

        private ValueTask<Response<RemoteConversationTranscriptionResult>> DefaultWaitForCompletionAsync(CancellationToken cancellationToken)
        {
            return WaitForCompletionAsync(m_defaultPollingInterval, cancellationToken);
        }

        private async ValueTask<Response<RemoteConversationTranscriptionResult>> DefaultWaitForCompletionAsync(TimeSpan pollingInterval, CancellationToken cancellationToken)
        {
            while (true)
            {
                await UpdateStatusAsync(cancellationToken).ConfigureAwait(false);
                if (HasCompleted)
                {
                    return Response.FromValue(Value, GetRawResponse());
                }

                await Task.Delay(pollingInterval, cancellationToken).ConfigureAwait(false);
            }
        }

        private const int DEFAULT_POLLING_INTERVAL_SECONDS = 10;
        private RemoteConversationTranscriptionModel m_resultModel { get; set; }
        private Response m_response;
        private RemoteConversationTranscriptionResult m_result;
        private bool m_completed = false;
        private RemoteConversationTranscriptionClient m_client;
        private static TimeSpan m_defaultPollingInterval { get; } = TimeSpan.FromSeconds(DEFAULT_POLLING_INTERVAL_SECONDS);
    }
}


