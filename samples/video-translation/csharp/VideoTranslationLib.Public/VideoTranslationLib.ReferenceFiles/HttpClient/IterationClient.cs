//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace VideoTranslationPublicPreviewLib.HttpClient;

using Flurl;
using Flurl.Http;
using Flurl.Util;
using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.CommonLib.Util;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public;
using Microsoft.SpeechServices.DataContracts;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Threading.Tasks;

public class IterationClient : HttpClientBase
{
    public IterationClient(HttpClientConfigBase config)
        : base(config)
    {
    }

    public override string ControllerName => "translations";

    public async Task<(TIteration iteration, IReadOnlyNameValueList<string> headers)> GetIterationAsync<TIteration>(
        string translationId,
        string iterationId,
        IReadOnlyDictionary<string, string> additionalHeaders)
    {
        var responseTask = GetIterationWithResponseAsync(
            translationId: translationId,
            iterationId: iterationId,
            additionalHeaders: additionalHeaders);
        var response = await responseTask.ConfigureAwait(false);
        var iteration = await response.GetJsonAsync<TIteration>().ConfigureAwait(false);
        return (iteration, response.Headers);
    }

    public async Task<(string iterationStringResponse, IReadOnlyNameValueList<string> headers)> GetIterationStringResponseAsync(
        string translationId,
        string iterationId,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
    {
        var responseTask = GetIterationWithResponseAsync(
            translationId: translationId,
            iterationId: iterationId,
            additionalHeaders: additionalHeaders);
        var response = await responseTask.ConfigureAwait(false);
        var responseString = await response.GetStringAsync().ConfigureAwait(false);
        return (responseString, response.Headers);
    }

    public async Task<IFlurlResponse> GetIterationWithResponseAsync(
        string translationId,
        string iterationId,
        IReadOnlyDictionary<string, string> additionalHeaders)
    {
        ArgumentException.ThrowIfNullOrEmpty(translationId);
        ArgumentException.ThrowIfNullOrEmpty(iterationId);

        var url = BuildRequestBase(additionalHeaders: additionalHeaders)
            .AppendPathSegment(translationId)
            .AppendPathSegment("iterations")
            .AppendPathSegment(iterationId);

        return await RequestWithRetryAsync(async () =>
        {
            try
            {
                return await url
                    .GetAsync()
                    .ConfigureAwait(false);
            }
            catch (FlurlHttpException ex)
            {
                if (ex.StatusCode == (int)HttpStatusCode.NotFound)
                {
                    return null;
                }

                Console.Write($"Response failed with error: {await ex.GetResponseStringAsync().ConfigureAwait(false)}");
                throw;
            }
        }).ConfigureAwait(false);
    }

    public async Task<PaginatedResources<TIteration>> QueryIterationsAsync<TIteration>(string translationId)
    {
        var url = BuildRequestBase()
            .AppendPathSegment(translationId);

        return await RequestWithRetryAsync(async () =>
        {
            // var responseJson = await url.GetStringAsync().ConfigureAwait(false);
            return await url.GetAsync()
                .ReceiveJson<PaginatedResources<TIteration>>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<TIteration> CreateIterationAndWaitUntilTerminatedAsync<TIteration>(
        string translationId,
        TIteration iteration,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
        where TIteration : StatefulResourceBase
    {
        var operationId = Guid.NewGuid().ToString();

        Console.WriteLine($"Creating iteration {iteration.Id} for translation {translationId} :");
        var (iterationResponse, headers) = await CreateIterationAsync(
            translationId: translationId,
            iteration: iteration,
            operationId: operationId,
            additionalHeaders: additionalHeaders).ConfigureAwait(false);
        ArgumentNullException.ThrowIfNull(iterationResponse);
        Console.WriteLine("Creating iteration:");
        Console.WriteLine(JsonConvert.SerializeObject(
            iterationResponse,
            Formatting.Indented,
            CommonPublicConst.Json.WriterSettings));

        if (!headers.TryGetFirst(CommonPublicConst.Http.Headers.OperationLocation, out var operationLocation) ||
            string.IsNullOrEmpty(operationLocation))
        {
            throw new InvalidDataException($"Missing header {CommonPublicConst.Http.Headers.OperationLocation} in headers");
        }

        var operationClient = new OperationClient(this.Config);

        await operationClient.QueryOperationUntilTerminateAsync(new Uri(operationLocation)).ConfigureAwait(false);

        (iterationResponse, headers) = await GetIterationAsync<TIteration>(
            translationId: translationId,
            iterationId: iterationResponse.Id,
            additionalHeaders: additionalHeaders).ConfigureAwait(false);

        Console.WriteLine("Created iteration:");
        Console.WriteLine(JsonConvert.SerializeObject(
            iterationResponse,
            Formatting.Indented,
            CommonPublicConst.Json.WriterSettings));

        return iterationResponse;
    }

    public async Task<(TIteration iteration, IReadOnlyNameValueList<string> headers)> CreateIterationAsync<TIteration>(
        string translationId,
        TIteration iteration,
        string operationId,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
        where TIteration : StatefulResourceBase
    {
        ArgumentNullException.ThrowIfNull(iteration);

        var responseTask = CreateIterationWithResponseAsync(
            translationId: translationId,
            iteration: iteration,
            operationId: operationId,
            additionalHeaders: additionalHeaders);
        var response = await responseTask.ConfigureAwait(false);
        var responseString = await response.GetStringAsync()
            .ConfigureAwait(false);
        var iterationResponse = await response.GetJsonAsync<TIteration>()
            .ConfigureAwait(false);
        return (iterationResponse, response.Headers);
    }

    public async Task<(string iterationResponseString, IReadOnlyNameValueList<string> headers)> CreateIterationWithStringResponseAsync<TIteration>(
        string translationId,
        TIteration iteration,
        string operationId,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
        where TIteration : StatefulResourceBase
    {
        var responseTask = CreateIterationWithResponseAsync(
            translationId: translationId,
            iteration: iteration,
            operationId: operationId,
            additionalHeaders: additionalHeaders);
        var response = await responseTask.ConfigureAwait(false);
        var responseString = await response.GetStringAsync()
            .ConfigureAwait(false);
        return (responseString, response.Headers);
    }

    private async Task<IFlurlResponse> CreateIterationWithResponseAsync<TIteration>(
        string translationId,
        TIteration iteration,
        string operationId,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
        where TIteration : StatefulResourceBase
    {
        ArgumentNullException.ThrowIfNull(iteration);
        ArgumentException.ThrowIfNullOrEmpty(translationId);
        ArgumentException.ThrowIfNullOrEmpty(iteration.Id);
        ArgumentException.ThrowIfNullOrEmpty(operationId);

        var url = BuildRequestBase(additionalHeaders: additionalHeaders)
            .AppendPathSegment(translationId)
            .AppendPathSegment("iterations")
            .AppendPathSegment(iteration.Id)
            .WithHeader(CommonPublicConst.Http.Headers.OperationId, operationId);

        return await RequestWithRetryAsync(async () =>
        {
            return await url
                .PutJsonAsync(iteration)
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }
}
