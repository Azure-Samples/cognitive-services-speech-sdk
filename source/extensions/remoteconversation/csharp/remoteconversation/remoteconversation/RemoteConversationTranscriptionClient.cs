//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Azure;
using Azure.Core;
using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace Microsoft.CognitiveServices.Speech.RemoteConversation
{
    /// <summary>
    /// Perform remote conversation transcription for an already started conversation.
    /// Added in version 1.13.0
    /// </summary>
    public class RemoteConversationTranscriptionClient
    {
        /// <summary>
        /// Constructor to create RemoteConversationTranscriptionClient
        /// </summary>
        /// <param name="config">The speech config object.</param>
        public RemoteConversationTranscriptionClient(SpeechConfig config)
        {
            m_config = config;
            string key = config.GetProperty(PropertyId.SpeechServiceConnection_Key);
            string region = config.GetProperty(PropertyId.SpeechServiceConnection_Region);
            string token = config.GetProperty(PropertyId.SpeechServiceAuthorization_Token);
            string endpoint = config.GetProperty(PropertyId.SpeechServiceConnection_Endpoint);

            if (endpoint != null && endpoint.Length != 0)
            {
                m_speechEndpoint = endpoint;
            }
            else
            {
                if (region == null || region.Length == 0)
                {
                    throw new ApplicationException("Must have a valid speech region or endpoint");
                }
                else
                {
                    m_speechEndpoint = m_hostNamePrefix + region + m_hostNameSuffix;
                }
            }

            if ((token == null || token.Length == 0) && (key == null || key.Length == 0))
            {
                throw new ApplicationException("Must have a valid authorization token or subscription key");
            }

            if (token != null && token.Length != 0)
            {
                m_pipeline = new RemoteConversationPipeline(m_speechEndpoint, "Authorization", token);
            }
            else
            {
                m_pipeline = new RemoteConversationPipeline(m_speechEndpoint, "Ocp-Apim-Subscription-Key", key);
            }

        }

        /// <summary>
        /// Delete Remote Conversation Transcription
        /// </summary>
        /// <param name="id">The meeting id to be deleted.</param>
        public Response DeleteRemoteTranscription(string id)
        {
            var res = m_pipeline.SendRequest(RequestMethod.Delete, id, () => new RemoteConversationDeleteTranscriptionModel(), CancellationToken.None);
            return res.GetRawResponse();
        }

        /// <summary>
        /// Delete Remote Conversation Transcription async
        /// </summary>
        /// <param name="id">The meeting id to be deleted.</param>
        public async Task<Response> DeleteRemoteTranscriptionAsync(string id)
        {
            var res = await m_pipeline.SendRequestAsync(RequestMethod.Delete, id, () => new RemoteConversationDeleteTranscriptionModel(), CancellationToken.None);
            return res.GetRawResponse();
        }

        internal virtual Response<RemoteConversationTranscriptionModel> GetTranscriptionResults(string id, CancellationToken cancellationToken = default)
        {
            Response<RemoteConversationTranscriptionModel> model = m_pipeline.SendRequest(RequestMethod.Get, id, () => new RemoteConversationTranscriptionModel(), cancellationToken);
            return Response.FromValue(model.Value, model.GetRawResponse());
        }

        internal virtual async Task<Response<RemoteConversationTranscriptionModel>> GetTranscriptionResultsAsync(string id, CancellationToken cancellationToken = default)
        {
            Response<RemoteConversationTranscriptionModel> model =  await m_pipeline.SendRequestAsync(RequestMethod.Get, id, () => new RemoteConversationTranscriptionModel(), cancellationToken);
            return Response.FromValue(model.Value, model.GetRawResponse());
        }

        private SpeechConfig m_config = null;
        private RemoteConversationPipeline m_pipeline;
        private string m_hostNameSuffix = ".cts.speech.microsoft.com/api/v1/";
        private string m_hostNamePrefix = "https://transcribe.";
        private string m_speechEndpoint = null;
    }
}


