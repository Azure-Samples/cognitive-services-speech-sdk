//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Util;

using Flurl;
using Flurl.Http;
using Microsoft.SpeechServices.CommonLib.Attributes;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.CommonLib.Extensions;
using Microsoft.SpeechServices.CustomVoice.TtsLib.TtsUtil;
using Microsoft.SpeechServices.DataContracts.Deprecated;
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
    protected const string WatermarkDetectionsControllerName = "WatermarkDetections";
    protected const string Version20230401Preview = "2023-04-01-preview";
    private static string apiVersion = string.Empty;

    public HttpClientBase(TDeploymentEnvironment environment, string subKey)
    {
        this.Environment = environment;
        this.SubscriptionKey = subKey;
    }

    public static string ApiVersion
    {
        get
        {
            if (!string.IsNullOrEmpty(apiVersion))
            {
                return apiVersion;
            }

            return Version20230401Preview;
        }

        set
        {
            apiVersion = value;
        }
    }

    public virtual string RouteBase => "texttospeech";

    public abstract string ControllerName { get; }

    public TDeploymentEnvironment Environment { get; set; }

    public string SubscriptionKey { get; set; }

    public Uri BaseUrl
    {
        get
        {
            return this.Environment.GetAttributeOfType<DeploymentEnvironmentAttribute>()?.GetApiBaseUrl();
        }
    }

    public async Task<IFlurlResponse> DeleteByIdAsync(
        Guid id,
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
        var url = this.BuildRequestBase(additionalHeaders)
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
        var url = this.BuildRequestBase(additionalHeaders)
            .AppendPathSegment(id.ToString());

        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
                .GetAsync()
                .ReceiveJson<T>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    protected IFlurlRequest BuildRequestBase(IReadOnlyDictionary<string, string> additionalHeaders = null)
    {
        var url = this.BaseUrl
            .AppendPathSegment("api")
            .AppendPathSegment(RouteBase)
            .AppendPathSegment(this.ControllerName)
            .SetQueryParam("api-version", ApiVersion)
            .WithHeader("Ocp-Apim-Subscription-Key", this.SubscriptionKey);
        if (additionalHeaders != null)
        {
            foreach (var additionalHeader in additionalHeaders)
            {
                url.WithHeader(additionalHeader.Key, additionalHeader.Value);
            }
        }

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
