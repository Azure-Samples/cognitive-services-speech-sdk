//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Util;

using Flurl;
using Flurl.Http;
using Flurl.Http.Configuration;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public;
using Microsoft.SpeechServices.CustomVoice.TtsLib.TtsUtil;
using Newtonsoft.Json;
using Polly;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

public abstract class HttpClientBase<TDeploymentEnvironment>
    where TDeploymentEnvironment : Enum
{
    public HttpClientBase(HttpClientConfigBase<TDeploymentEnvironment> config)
    {
        this.Config = config;
    }

    protected HttpClientConfigBase<TDeploymentEnvironment> Config { get; set; }

    public abstract string ControllerName { get; }

    public async Task<IFlurlResponse> DeleteByIdAsync(
        string id,
        IReadOnlyDictionary<string, string> queryParams = null)
    {
        var url = this.BuildRequestBase();

        url = url.AppendPathSegment(id);

        if (queryParams != null)
        {
            foreach (var (name, value) in queryParams)
            {
                url = url.SetQueryParam(name, value);
            }
        }

        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
            .DeleteAsync()
            .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<string> QueryByIdResponseStringAsync(
        Guid id,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
    {
        var url = this.BuildRequestBase(additionalHeaders: additionalHeaders)
            .AppendPathSegment(id.ToString());

        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
            .GetAsync()
            .ReceiveString()
            .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    protected async Task<T> QueryByIdAsync<T>(
        Guid id,
        IReadOnlyDictionary<string, string> additionalHeaders = null)
    {
        var url = this.BuildRequestBase(additionalHeaders: additionalHeaders)
            .AppendPathSegment(id.ToString());

        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
                .GetAsync()
                .ReceiveJson<T>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    protected IFlurlRequest BuildRequestBase(
        IReadOnlyDictionary<string, string> additionalHeaders = null)
    {
        var url = this.Config.RootUrl
            .AppendPathSegment(this.ControllerName)
            .SetQueryParam("api-version", this.Config.ApiVersion)
            .WithHeader("Ocp-Apim-Subscription-Key", this.Config.SubscriptionKey);
        if (additionalHeaders != null)
        {
            foreach (var additionalHeader in additionalHeaders)
            {
                url.WithHeader(additionalHeader.Key, additionalHeader.Value);
            }
        }

        // Default json serializer will serialize enum to number, which will cause API parse DTO failure:
        //  "Error converting value 0 to type 'Microsoft.SpeechServices.Common.Client.OneApiState'. Path 'Status', line 1, position 56."
        url.Settings.JsonSerializer = new NewtonsoftJsonSerializer(CustomContractResolver.WriterSettings);

        return url;
    }

    public async Task<T> QueryTaskByIdUntilTerminatedAsync<T>(
        Guid id,
        IReadOnlyDictionary<string, string> additionalHeaders = null,
        bool printFirstQueryResult = false,
        TimeSpan? timeout = null)
        where T : StatefulResourceBase
    {
        var startTime = DateTime.Now;
        OneApiState? state = null;
        var firstTimePrinted = false;

        while (DateTime.Now - startTime < (timeout ?? TimeSpan.FromHours(3)))
        {
            var translation = await this.QueryByIdAsync<T>(id, additionalHeaders).ConfigureAwait(false);
            if (translation == null)
            {
                return null;
            }

            var runPrinted = false;
            if (printFirstQueryResult && !firstTimePrinted)
            {
                runPrinted = true;
                firstTimePrinted = true;
                ConsoleMaskSasHelper.WriteLineMaskSas(JsonConvert.SerializeObject(
                    translation,
                    Formatting.Indented,
                    CustomContractResolver.WriterSettings));
            }

            if (new[] { OneApiState.Failed, OneApiState.Succeeded }.Contains(translation.Status))
            {
                Console.WriteLine();
                Console.WriteLine();
                Console.WriteLine($"Task completed with state: {translation.Status.AsString()}");
                if (!runPrinted)
                {
                    ConsoleMaskSasHelper.WriteLineMaskSas(JsonConvert.SerializeObject(
                        translation,
                        Formatting.Indented,
                        CustomContractResolver.WriterSettings));
                }

                return translation;
            }
            else
            {
                await Task.Delay(TimeSpan.FromSeconds(3)).ConfigureAwait(false);
                if (state == null || state != translation.Status)
                {
                    Console.WriteLine();
                    Console.WriteLine();
                    Console.WriteLine($"Task {translation.Status.AsString()}:");
                    state = translation.Status;
                }
                else
                {
                    Console.Write(".");
                }
            }
        }

        Console.WriteLine();
        Console.WriteLine();
        Console.WriteLine($"Task run timeout after {(DateTime.Now - startTime).TotalMinutes.ToString("0.00", CultureInfo.InvariantCulture)} mins");
        return null;
    }

    public async Task<TResponse> RequestWithRetryAsync<TResponse>(Func<Task<TResponse>> requestAsyncFunc)
    {
        var policy = BuildRetryPolicy();

        return await policy.ExecuteAsync(async () =>
        {
            return await ExceptionHelper.PrintHandleExceptionAsync(async () =>
            {
                return await requestAsyncFunc().ConfigureAwait(false);
            });
        });
    }

    public static Polly.Retry.AsyncRetryPolicy BuildRetryPolicy()
    {
        var retryPolicy = Policy
           .Handle<FlurlHttpException>(IsTransientError)
           .WaitAndRetryAsync(5, retryAttempt =>
           {
               var nextAttemptIn = TimeSpan.FromSeconds(5 * Math.Pow(2, retryAttempt));
               Console.WriteLine($"Retry attempt {retryAttempt} to make request. Next try on {nextAttemptIn.TotalSeconds} seconds.");
               return nextAttemptIn;
           });

        return retryPolicy;
    }

    protected static bool IsTransientError(FlurlHttpException exception)
    {
        int[] httpStatusCodesWorthRetrying =
        {
                (int)HttpStatusCode.RequestTimeout, // 408
                (int)HttpStatusCode.BadGateway, // 502
                (int)HttpStatusCode.ServiceUnavailable, // 503
                (int)HttpStatusCode.GatewayTimeout, // 504
                (int)HttpStatusCode.TooManyRequests, // 429
            };

        Console.WriteLine($"Flurl exception status code: {exception.StatusCode}");
        return exception.StatusCode.HasValue && httpStatusCodesWorthRetrying.Contains(exception.StatusCode.Value);
    }
}
