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
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;
using Microsoft.SpeechServices.CustomVoice;
using Microsoft.SpeechServices.DataContracts;
using Microsoft.VisualBasic;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

public class IterationClient<TDeploymentEnvironment> : HttpClientBase<TDeploymentEnvironment>
    where TDeploymentEnvironment : Enum
{
    public IterationClient(HttpClientConfigBase<TDeploymentEnvironment> config)
        : base(config)
    {
    }

    public override string ControllerName => "translations";

    public async Task<(TIteration iteration, IReadOnlyNameValueList<string> headers)> GetIterationAsync<TIteration>(
        string translationId,
        string iterationId,
        IReadOnlyDictionary<string, string> additionalHeaders)
        where TIteration : Iteration
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

    public async Task<PaginatedResources<Iteration>> QueryIterationsAsync(string translationId)
    {
        var url = BuildRequestBase()
            .AppendPathSegment(translationId);

        return await RequestWithRetryAsync(async () =>
        {
            // var responseJson = await url.GetStringAsync().ConfigureAwait(false);
            return await url.GetAsync()
                .ReceiveJson<PaginatedResources<Iteration>>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<TIteration> CreateIterationAndWaitUntilTerminatedAsync<TIteration>(
        string translationId,
        TIteration iteration,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
        where TIteration : Iteration
    {
        var operationId = Guid.NewGuid().ToString();

        Console.WriteLine($"Creating iteration {iteration.Id} for translation {translationId} :");
        var (responseIteration, headers) = await CreateIterationAsync(
            translationId: translationId,
            iteration: iteration,
            operationId: operationId,
            additionalHeaders: additionalHeaders).ConfigureAwait(false);
        ArgumentNullException.ThrowIfNull(responseIteration);

        if (!headers.TryGetFirst(CommonConst.Http.Headers.OperationLocation, out var operationLocation) ||
            string.IsNullOrEmpty(operationLocation))
        {
            throw new InvalidDataException($"Missing header {CommonConst.Http.Headers.OperationLocation} in headers");
        }

        var operationClient = new OperationClient<TDeploymentEnvironment>(this.Config);

        await operationClient.QueryOperationUntilTerminateAsync(new Uri(operationLocation)).ConfigureAwait(false);

        (responseIteration, headers) = await GetIterationAsync<TIteration>(
            translationId: translationId,
            iterationId: responseIteration.Id,
            additionalHeaders: additionalHeaders).ConfigureAwait(false);
        return responseIteration;
    }

    public async Task<(TIteration iteration, IReadOnlyNameValueList<string> headers)> CreateIterationAsync<TIteration>(
        string translationId,
        TIteration iteration,
        string operationId,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
        where TIteration : Iteration
    {
        ArgumentNullException.ThrowIfNull(iteration);

        var responseTask = CreateIterationWithResponseAsync(
            translationId: translationId,
            iteration: iteration,
            operationId: operationId,
            additionalHeaders: additionalHeaders);
        var response = await responseTask.ConfigureAwait(false);
        var iterationResponse = await response.GetJsonAsync<TIteration>()
            .ConfigureAwait(false);
        return (iterationResponse, response.Headers);
    }

    public async Task<(string iterationResponseString, IReadOnlyNameValueList<string> headers)> CreateIterationWithStringResponseAsync(
        string translationId,
        Iteration iteration,
        string operationId,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
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

    private async Task<IFlurlResponse> CreateIterationWithResponseAsync(
        string translationId,
        Iteration iteration,
        string operationId,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
    {
        ArgumentNullException.ThrowIfNull(iteration);
        ArgumentException.ThrowIfNullOrEmpty(translationId);
        ArgumentException.ThrowIfNullOrEmpty(iteration.Id);
        ArgumentException.ThrowIfNullOrEmpty(operationId);

        var url = BuildRequestBase(additionalHeaders: additionalHeaders)
            .AppendPathSegment(translationId)
            .AppendPathSegment("iterations")
            .AppendPathSegment(iteration.Id)
            .WithHeader(CommonConst.Http.Headers.OperationId, operationId);

        return await RequestWithRetryAsync(async () =>
        {
            return await url
                .PutJsonAsync(iteration)
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }
}
