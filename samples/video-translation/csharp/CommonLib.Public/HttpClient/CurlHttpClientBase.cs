//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Flurl;
using Flurl.Http;
using Flurl.Util;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public;
using Microsoft.SpeechServices.DataContracts;
using Newtonsoft.Json;
using System;
using System.Globalization;
using System.Net;
using System.Threading.Tasks;

namespace Microsoft.SpeechServices.CommonLib.Util;

public abstract class CurlHttpClientBase<TDto> : HttpClientBase
    where TDto : ResourceBase
{
    public CurlHttpClientBase(HttpClientConfigBase config)
        : base(config)
    {
    }

    protected async Task<(TDto response, IReadOnlyNameValueList<string> headers)> CreateDtoWithOperationAsync(
        TDto dto,
        string operationId)
    {
        ArgumentNullException.ThrowIfNull(dto);
        var (responseString, headers) = await CreateDtoWithOperationAndStringResponseAsync(
            dto: dto,
            operationId: operationId).ConfigureAwait(false);
        var typedResponse = JsonConvert.DeserializeObject<TDto>(responseString);
        return (typedResponse, headers);
    }

    protected async Task<(string responseString,
        IReadOnlyNameValueList<string> headers)>
    CreateDtoWithOperationAndStringResponseAsync(
        TDto dto,
        string operationId)
    {
        ArgumentNullException.ThrowIfNull(dto);
        var responseTask = CreateDtoWithOperationIdAndResponseAsync(
            dto: dto,
            operationId: operationId);
        var response = await responseTask.ConfigureAwait(false);
        var stringResponse = await response.GetStringAsync()
            .ConfigureAwait(false);
        return (stringResponse, response.Headers);
    }

    protected async Task<PaginatedResources<TDto>> ListTypedDtosAsync(
        int? top = null,
        int? skip = null,
        int? maxPageSize = null)
    {
        var url = await this.BuildRequestBaseAsync().ConfigureAwait(false);
        if (top != null)
        {
            url = url.SetQueryParam("top", top.Value.ToString(CultureInfo.InvariantCulture));
        }

        if (skip != null)
        {
            url = url.SetQueryParam("skip", skip.Value.ToString(CultureInfo.InvariantCulture));
        }

        if (maxPageSize != null)
        {
            // maxpagesize is case sensitive.
            url = url.SetQueryParam("maxpagesize", maxPageSize.Value.ToString(CultureInfo.InvariantCulture));
        }

        Console.WriteLine($"Listing: {url.Url}");
        return await RequestWithRetryAsync(async () =>
        {
            return await url.GetAsync()
                .ReceiveJson<PaginatedResources<TDto>>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    protected async Task<TDto> GetTypedDtoAsync(string translationId)
    {
        var response = await GetDtoResponseAsync(translationId).ConfigureAwait(false);

        // Not exist.
        if (response == null)
        {
            return null;
        }

        return await response.GetJsonAsync<TDto>().ConfigureAwait(false);
    }

    protected async Task<string> GetDtoResponseStringAsync(string id)
    {
        var response = await GetDtoResponseAsync(id).ConfigureAwait(false);
        return await response.GetStringAsync().ConfigureAwait(false);
    }

    protected async Task<IFlurlResponse> GetDtoResponseAsync(string id)
    {
        var url = await this.BuildRequestBaseAsync().ConfigureAwait(false);

        url = url.AppendPathSegment(id);

        Console.WriteLine($"Getting: {url.Url}");
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

    private async Task<IFlurlResponse> CreateDtoWithOperationIdAndResponseAsync(
        TDto dto,
        string operationId)
    {
        ArgumentNullException.ThrowIfNull(dto);
        ArgumentException.ThrowIfNullOrEmpty(dto.Id);
        ArgumentException.ThrowIfNullOrEmpty(operationId);

        var url = await this.BuildRequestBaseAsync().ConfigureAwait(false);
        url = url.AppendPathSegment(dto.Id)
            .WithHeader(CommonPublicConst.Http.Headers.OperationId, operationId);

        return await RequestWithRetryAsync(async () =>
        {
            return await url
                .PutJsonAsync(dto)
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }
}
