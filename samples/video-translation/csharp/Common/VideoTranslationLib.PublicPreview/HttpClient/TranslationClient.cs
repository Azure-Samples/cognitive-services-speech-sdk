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
using Microsoft.SpeechServices.DataContracts;
using Newtonsoft.Json;
using System;
using System.IO;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

public class TranslationClient<TDeploymentEnvironment, TIteration, TIterationInput> : HttpClientBase<TDeploymentEnvironment>
    where TDeploymentEnvironment : Enum
    where TIteration : Iteration<TIterationInput>
    where TIterationInput : IterationInput
{
    public TranslationClient(HttpClientConfigBase<TDeploymentEnvironment> config)
        : base(config)
    {
    }

    public override string ControllerName => "Translations";


    public async Task<IFlurlResponse> DeleteTranslationAsync(string translationId)
    {
        ArgumentException.ThrowIfNullOrEmpty(translationId);

        var url = BuildRequestBase()
            .AppendPathSegment(translationId);

        return await RequestWithRetryAsync(async () =>
        {
            return await url
                .DeleteAsync()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<string> GetTranslationStringAsync(string translationId)
    {
        var response = await GetTranslationResponseAsync(translationId).ConfigureAwait(false);
        return await response.GetStringAsync().ConfigureAwait(false);
    }

    public async Task<Translation<TIteration, TIterationInput>> GetTranslationAsync(string translationId)
    {
        var response = await GetTranslationResponseAsync(translationId).ConfigureAwait(false);

        // Not exist.
        if (response == null)
        {
            return null;
        }

        return await response.GetJsonAsync<Translation<TIteration, TIterationInput>>().ConfigureAwait(false);
    }

    public async Task<IFlurlResponse> GetTranslationResponseAsync(string translationId)
    {
        var url = BuildRequestBase();

        url = url.AppendPathSegment(translationId.ToString());

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

    public async Task<PaginatedResources<Translation<TIteration, TIterationInput>>> GetTranslationsAsync()
    {
        var url = BuildRequestBase();

        return await RequestWithRetryAsync(async () =>
        {
            // var responseJson = await url.GetStringAsync().ConfigureAwait(false);
            return await url.GetAsync()
                .ReceiveJson<PaginatedResources<Translation<TIteration, TIterationInput>>>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<PaginatedResources<TIteration>> GetIterationsAsync(string translationId)
    {
        var url = BuildRequestBase();

        return await RequestWithRetryAsync(async () =>
        {
            // var responseJson = await url.GetStringAsync().ConfigureAwait(false);
            return await url
                .AppendPathSegment(translationId)
                .AppendPathSegment("iterations")
                .GetAsync()
                .ReceiveJson<PaginatedResources<TIteration>>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<TIteration> GetIterationAsync(string translationId, string iterationId)
    {
        var url = BuildRequestBase();

        return await RequestWithRetryAsync(async () =>
        {
            // var responseJson = await url.GetStringAsync().ConfigureAwait(false);
            return await url
                .AppendPathSegment(translationId)
                .AppendPathSegment("iterations")
                .AppendPathSegment(iterationId)
                .GetAsync()
                .ReceiveJson<TIteration>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<(Translation<TIteration, TIterationInput> translation, TIteration iteration)> CreateTranslationAndIterationAndWaitUntilTerminatedAsync(
        Translation<TIteration, TIterationInput> translation,
        TIteration iteration)
    {
        var transaltionResponse = await CreateTranslationAndWaitUntilTerminatedAsync(
            translation: translation).ConfigureAwait(false);
        ArgumentNullException.ThrowIfNull(transaltionResponse);

        Console.WriteLine("Created translation:");
        Console.WriteLine(JsonConvert.SerializeObject(
            transaltionResponse,
            Formatting.Indented,
            CustomContractResolver.WriterSettings));

        var iterationClient = new IterationClient<TDeploymentEnvironment>(this.Config);
        var iterationResponse = await iterationClient.CreateIterationAndWaitUntilTerminatedAsync(
            translationId: transaltionResponse.Id,
            iteration: iteration,
            additionalHeaders: null).ConfigureAwait(false);

        return (transaltionResponse, iterationResponse);
    }

    public async Task<Translation<TIteration, TIterationInput>> CreateTranslationAndWaitUntilTerminatedAsync(
        Translation<TIteration, TIterationInput> translation)
    {
        Console.WriteLine($"Creating translation {translation.Id} :");

        var operationId = Guid.NewGuid().ToString();
        var (responseTranslation, createTranslationResponseHeaders) = await CreateTranslationAsync(
            translation: translation,
            operationId: operationId).ConfigureAwait(false);
        ArgumentNullException.ThrowIfNull(responseTranslation);

        if (!createTranslationResponseHeaders.TryGetFirst(CommonConst.Http.Headers.OperationLocation, out var operationLocation) ||
            string.IsNullOrEmpty(operationLocation))
        {
            throw new InvalidDataException($"Missing header {CommonConst.Http.Headers.OperationLocation} in headers");
        }

        var operationClient = new OperationClient<TDeploymentEnvironment>(this.Config);

        await operationClient.QueryOperationUntilTerminateAsync(new Uri(operationLocation)).ConfigureAwait(false);

        return await GetTranslationAsync(
            translationId: responseTranslation.Id).ConfigureAwait(false);
    }

    public async Task<(Translation<TIteration, TIterationInput> translation, IReadOnlyNameValueList<string> headers)> CreateTranslationAsync(
        Translation<TIteration, TIterationInput> translation,
        string operationId)
    {
        ArgumentNullException.ThrowIfNull(translation);
        var responseTask = CreateTranslationWithResponseAsync(
            translation: translation,
            operationId: operationId);
        var response = await responseTask.ConfigureAwait(false);
        var translationResponse = await response.GetJsonAsync<Translation<TIteration, TIterationInput>>()
            .ConfigureAwait(false);
        return (translationResponse, response.Headers);
    }

    public async Task<(string responseString, IReadOnlyNameValueList<string> headers)> CreateTranslationWithStringResponseAsync(
        Translation<TIteration, TIterationInput> translation,
        string operationId)
    {
        ArgumentNullException.ThrowIfNull(translation);
        var responseTask = CreateTranslationWithResponseAsync(
            translation: translation,
            operationId: operationId);
        var response = await responseTask.ConfigureAwait(false);
        var translationResponse = await response.GetStringAsync()
            .ConfigureAwait(false);
        return (translationResponse, response.Headers);
    }

    private async Task<IFlurlResponse> CreateTranslationWithResponseAsync(
        Translation<TIteration, TIterationInput> translation,
        string operationId)
    {
        ArgumentNullException.ThrowIfNull(translation);
        ArgumentException.ThrowIfNullOrEmpty(translation.Id);
        ArgumentException.ThrowIfNullOrEmpty(operationId);

        var url = BuildRequestBase()
            .AppendPathSegment(translation.Id)
            .WithHeader(CommonConst.Http.Headers.OperationId, operationId);

        return await RequestWithRetryAsync(async () =>
        {
            return await url
                .PutJsonAsync(translation)
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }
}
