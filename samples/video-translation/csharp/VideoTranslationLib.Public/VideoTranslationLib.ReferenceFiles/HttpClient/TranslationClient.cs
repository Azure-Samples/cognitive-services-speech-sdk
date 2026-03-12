//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace VideoTranslationPublicPreviewLib.HttpClient;

using Flurl;
using Flurl.Http;
using Flurl.Util;
using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.CommonLib.Public.Enums;
using Microsoft.SpeechServices.CommonLib.Util;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20250520;
using Microsoft.SpeechServices.DataContracts;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Threading.Tasks;

public class TranslationClient : CurlHttpStatefulClientBase<Translation>
{
    public TranslationClient(HttpSpeechClientConfigBase config)
        : base(config)
    {
    }

    public override string ControllerName => "translations";


    public async Task<IFlurlResponse> DeleteTranslationAsync(string translationId)
    {
        return await this.DeleteByIdAsync(translationId).ConfigureAwait(false);
    }

    public async Task<string> GetTranslationStringAsync(string translationId)
    {
        return await this.GetDtoResponseStringAsync(translationId).ConfigureAwait(false);
    }

    public async Task<Translation> GetTranslationAsync(string translationId)
    {
        return await this.GetTypedDtoAsync(translationId).ConfigureAwait(false);
    }

    public async Task<PaginatedResources<Translation>> GetTranslationsAsync()
    {
        return await this.ListTypedDtosAsync().ConfigureAwait(false);
    }

    public async Task<PaginatedResources<Iteration>> GetIterationsAsync(string translationId)
    {
        var url = await this.BuildRequestBaseAsync().ConfigureAwait(false);

        return await RequestWithRetryAsync(async () =>
        {
            return await url
                .AppendPathSegment(translationId)
                .AppendPathSegment("iterations")
                .GetAsync()
                .ReceiveJson<PaginatedResources<Iteration>>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<Iteration> GetIterationAsync(string translationId, string iterationId)
    {
        var url = await this.BuildRequestBaseAsync().ConfigureAwait(false);

        return await RequestWithRetryAsync(async () =>
        {
            return await url
                .AppendPathSegment(translationId)
                .AppendPathSegment("iterations")
                .AppendPathSegment(iterationId)
                .GetAsync()
                .ReceiveJson<Iteration>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<(Translation translation, Iteration iteration)> CreateTranslationAndIterationAndWaitUntilTerminatedAsync(
        Translation translation,
        Iteration iteration,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
    {
        var translationResponse = await CreateTranslationAndWaitUntilTerminatedAsync(
            translation: translation).ConfigureAwait(false);
        ArgumentNullException.ThrowIfNull(translationResponse);

        Console.WriteLine("Created translation:");
        Console.WriteLine(JsonConvert.SerializeObject(
            translationResponse,
            Formatting.Indented,
            CommonPublicConst.Json.WriterSettings));

        if (translationResponse.Status != OneApiState.Succeeded)
        {
            throw new InvalidDataException("Failed to create translation.");
        }

        Iteration iterationResponse = null;
        if (!(translation.Input?.AutoCreateFirstIteration ?? false))
        {
            var iterationClient = new IterationClient(this.SpeechConfig);
            iterationResponse = await iterationClient.CreateIterationAndWaitUntilTerminatedAsync(
                translationId: translationResponse.Id,
                iteration: iteration,
                additionalHeaders: additionalHeaders).ConfigureAwait(false);
        }

        return (translationResponse, iterationResponse);
    }

    public async Task<Translation> CreateTranslationAndWaitUntilTerminatedAsync(
        Translation translation)
    {
        return await this.CreateDtoAndWaitUntilTerminatedAsync(translation).ConfigureAwait(false);
    }

    public async Task<(Translation translation, IReadOnlyNameValueList<string> headers)> CreateTranslationAsync(
        Translation translation,
        string operationId)
    {
        return await this.CreateDtoWithOperationAsync(
            dto: translation,
            operationId: operationId).ConfigureAwait(false);
    }

    public async Task<(string responseString, IReadOnlyNameValueList<string> headers)> CreateTranslationWithStringResponseAsync(
        Translation translation,
        string operationId)
    {
        return await this.CreateDtoWithOperationAndStringResponseAsync(
            dto: translation,
            operationId: operationId).ConfigureAwait(false);
    }
}
