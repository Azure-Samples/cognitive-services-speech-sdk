//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Azure;
using Azure.Core;
using Azure.Core.Pipeline;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.RemoteConversation
{
    internal class RemoteConversationPipeline
    {
        internal RemoteConversationPipeline(string endpoint, string authKey, string authVal)
        {
            m_authorizationKey = authKey;
            m_authorizationValue = authVal;
            m_Uri = new Uri(endpoint);
            m_pipeline = HttpPipelineBuilder.Build(new RemoteConversationClientOptions());
        }

        internal Response<T> CreateResponse<T>(T result, Response response)
            where T : IJsonDeserializable
        {
            result.Deserialize(response.ContentStream);
            return Response.FromValue(result, response);
        }

        internal Request CreateRequest(RequestMethod method, string id)
        {
            Request request = m_pipeline.CreateRequest();

            request.Method = method;
            request.Uri.Reset(m_Uri);
            request.Headers.Add(m_authorizationKey, m_authorizationValue);
            request.Headers.Add("accept", "application/json");
            request.Uri.AppendPath("OfflineTask");
            request.Uri.AppendQuery("id", id);

            return request;
        }

        internal Response<TResult> SendRequest<TResult>(RequestMethod method, string id, Func<TResult> resultFactory, CancellationToken cancellationToken)
            where TResult : IJsonDeserializable
        {
            using (Request request = CreateRequest(method, id))
            {
                Response response = m_pipeline.SendRequest(request, cancellationToken);

                if (method == RequestMethod.Delete)
                {
                    return Response.FromValue(resultFactory(), response);
                }
                else
                {
                    return CreateResponse(resultFactory(), response);
                }
            }


        }
        internal async Task<Response<TResult>> SendRequestAsync<TResult>(RequestMethod method, string id, Func<TResult> resultFactory, CancellationToken cancellationToken)
            where TResult : IJsonDeserializable
        {
            using (Request request = CreateRequest(method, id))
            {
                Response response = await m_pipeline.SendRequestAsync(request, CancellationToken.None).ConfigureAwait(false);

                if (method == RequestMethod.Delete)
                {
                    return Response.FromValue(resultFactory(), response);
                }
                else
                {
                    return CreateResponse(resultFactory(), response);
                }
            }
        }

        private readonly HttpPipeline m_pipeline;
        private Uri m_Uri;
        private string m_authorizationKey;
        private string m_authorizationValue;
    }
}
