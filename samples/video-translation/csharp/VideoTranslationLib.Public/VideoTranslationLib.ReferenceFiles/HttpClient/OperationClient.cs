//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace VideoTranslationPublicPreviewLib.HttpClient;

using Flurl;
using Flurl.Http;
using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.CommonLib.Util;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;
using System;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

public class OperationClient : HttpClientBase
{
    public OperationClient(HttpClientConfigBase config)
        : base(config)
    {
    }

    public override string ControllerName => "operations";

    public async Task QueryOperationUntilTerminateAsync(
        Uri operationLocation)
    {
        var operation = await this.GetOperationAsync(operationLocation).ConfigureAwait(false);
        ArgumentNullException.ThrowIfNull(operation);
        Console.WriteLine($"Querying operation: {operationLocation}:");
        var lastStatus = operation.Status;
        Console.WriteLine(operation.Status);
        while (new[]
        {
            OperationStatus.NotStarted,
            OperationStatus.Running,
        }.Contains(operation.Status))
        {
            operation = await this.GetOperationAsync(operationLocation).ConfigureAwait(false);
            ArgumentNullException.ThrowIfNull(operation);
            if (operation.Status != lastStatus)
            {
                Console.WriteLine();
                Console.WriteLine(operation.Status);
                lastStatus = operation.Status;
            }

            Console.Write(".");
            await Task.Delay(CommonPublicConst.Http.OperationQueryDuration).ConfigureAwait(false);
        }

        Console.WriteLine();
    }

    public async Task<string> GetOperationStringAsync(Uri operationLocation)
    {
        var url = operationLocation
            .SetQueryParam(CommonPublicConst.Http.ParameterNames.ApiVersion, this.Config.ApiVersion)
            .WithHeader(CommonPublicConst.Http.Headers.SubscriptionKey, this.Config.SubscriptionKey);

        var response = await GetOperationWithResponseAsync(operationLocation).ConfigureAwait(false);
        return await response.GetStringAsync().ConfigureAwait(false);
    }

    public async Task<Operation> GetOperationAsync(Uri operationLocation)
    {
        var url = operationLocation
            .SetQueryParam(CommonPublicConst.Http.ParameterNames.ApiVersion, this.Config.ApiVersion)
            .WithHeader(CommonPublicConst.Http.Headers.SubscriptionKey, this.Config.SubscriptionKey);

        var response = await GetOperationWithResponseAsync(operationLocation).ConfigureAwait(false);
        return await response.GetJsonAsync<Operation>().ConfigureAwait(false);
    }

    public async Task<IFlurlResponse> GetOperationWithResponseAsync(Uri operationLocation)
    {
        var url = operationLocation
            .SetQueryParam(CommonPublicConst.Http.ParameterNames.ApiVersion, this.Config.ApiVersion)
            .WithHeader(CommonPublicConst.Http.Headers.SubscriptionKey, this.Config.SubscriptionKey);

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
}
