//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.SpeechServices.CommonLib.HttpClient;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public;
using Newtonsoft.Json;
using System;
using System.IO;
using System.Threading.Tasks;

namespace Microsoft.SpeechServices.CommonLib.Util;

public abstract class CurlHttpStatefulClientBase<TDto> : CurlHttpClientBase<TDto>
    where TDto : StatefulResourceBase
{
    public CurlHttpStatefulClientBase(HttpClientConfigBase config)
        : base(config)
    {
    }


    protected async Task<TDto> CreateDtoAndWaitUntilTerminatedAsync(
        TDto dto)
    {
        Console.WriteLine($"Creating resource {dto.Id} :");
        Console.WriteLine(JsonConvert.SerializeObject(dto, Formatting.Indented, CommonPublicConst.Json.WriterSettings));

        var operationId = Guid.NewGuid().ToString();
        var (response, createResponseHeaders) = await CreateDtoWithOperationAsync(
            dto: dto,
            operationId: operationId).ConfigureAwait(false);
        ArgumentNullException.ThrowIfNull(response);

        if (!createResponseHeaders.TryGetFirst(CommonPublicConst.Http.Headers.OperationLocation, out var operationLocation) ||
            string.IsNullOrEmpty(operationLocation))
        {
            throw new InvalidDataException($"Missing header {CommonPublicConst.Http.Headers.OperationLocation} in headers");
        }

        var operationClient = new OperationClient(this.SpeechConfig);

        await operationClient.QueryOperationUntilTerminateAsync(new Uri(operationLocation)).ConfigureAwait(false);

        return await GetTypedDtoAsync(
            translationId: response.Id).ConfigureAwait(false);
    }
}
