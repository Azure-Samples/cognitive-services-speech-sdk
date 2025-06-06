//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Util;
using Flurl;
using Flurl.Http;
using Flurl.Http.Configuration;
using Microsoft.SpeechServices.CommonLib.Public.Enums;
using Microsoft.SpeechServices.CommonLib.Public.Interface;
using Microsoft.SpeechServices.CommonLib.TtsUtil;
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

public abstract class HttpClientBase
{
    public HttpClientBase(HttpClientConfigBase config)
    {
        this.BaseConfig = config;
        this.Logger = new PublicAppLogger();
    }

    protected HttpClientConfigBase BaseConfig { get; set; }

    protected HttpSpeechClientConfigBase SpeechConfig
    {
        get
        {
            if (this.BaseConfig is HttpSpeechClientConfigBase speechConfig)
            {
                return speechConfig;
            }

            return null;
        }
    }

    public abstract string ControllerName { get; }

    public IAppLogger Logger { get; private set; }

    public virtual bool IsVersionInSegment => false;

    public async Task<IFlurlRequest> AuthenticateAsync(Flurl.Url request)
    {
        var speechConfig = this.SpeechConfig;
        if (speechConfig != null)
        {
            if (!string.IsNullOrEmpty(speechConfig.SubscriptionKey))
            {
                return request.WithHeader(
                    CommonPublicConst.Http.Headers.SubscriptionKey,
                    speechConfig.SubscriptionKey);
            }
            else if (string.IsNullOrEmpty(speechConfig.CustomDomainName))
            {
                // OAuth only avaible when custom domain enabled.
                throw new NotSupportedException($"Please provide either key or custom domain name");
            }
        }

        if (this.BaseConfig.UseOAuth)
        {
            var token = await this.BaseConfig.AcquireOAuthTokenAsync().ConfigureAwait(false);
            return request.WithOAuthBearerToken(token);
        }
        else
        {
            throw new NotSupportedException("Not supported auth");
        }
    }

    public async Task<IFlurlResponse> DeleteByIdAsync(
        string id,
        IReadOnlyDictionary<string, string> queryParams = null)
    {
        var url = await this.BuildRequestBaseAsync().ConfigureAwait(false);

        url = url.AppendPathSegment(id);

        if (queryParams != null)
        {
            foreach (var (name, value) in queryParams)
            {
                url = url.SetQueryParam(name, value);
            }
        }

        this.Logger?.LogDebug(url.Url.ToString());
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
        var url = await this.BuildRequestBaseAsync(
            additionalHeaders: additionalHeaders).ConfigureAwait(false);
        url = url.AppendPathSegment(id.ToString());

        this.Logger?.LogDebug(url.Url.ToString());
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
        var url = await this.BuildRequestBaseAsync(
            additionalHeaders: additionalHeaders).ConfigureAwait(false);
        url = url.AppendPathSegment(id.ToString());

        this.Logger?.LogDebug(url.Url.ToString());
        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
                .GetAsync()
                .ReceiveJson<T>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    protected async Task<IFlurlRequest> BuildBackendPathVersionRequestBaseAsync(
        IReadOnlyDictionary<string, string> additionalHeaders = null)
    {
        var url = this.BaseConfig.RootUrl
            .AppendPathSegment(this.ControllerName);
        var request = await this.AuthenticateAsync(url).ConfigureAwait(false);
        if (additionalHeaders != null)
        {
            foreach (var additionalHeader in additionalHeaders)
            {
                request = request.WithHeader(additionalHeader.Key, additionalHeader.Value);
            }
        }

        // Default json serializer will serialize enum to number, which will cause API parse DTO failure:
        //  "Error converting value 0 to type 'Microsoft.SpeechServices.Common.Client.OneApiState'. Path 'Status', line 1, position 56."
        request.Settings.JsonSerializer = new NewtonsoftJsonSerializer(CommonPublicConst.Json.WriterSettings);

        return request;
    }

    protected async Task<IFlurlRequest> BuildRequestBaseAsync(
        IReadOnlyDictionary<string, string> additionalHeaders = null)
    {
        var url = this.BaseConfig.RootUrl
            .AppendPathSegment(this.ControllerName);
        var request = await this.AuthenticateAsync(url).ConfigureAwait(false);
        if (additionalHeaders != null)
        {
            foreach (var additionalHeader in additionalHeaders)
            {
                request = request.WithHeader(additionalHeader.Key, additionalHeader.Value);
            }
        }

        // Default json serializer will serialize enum to number, which will cause API parse DTO failure:
        //  "Error converting value 0 to type 'Microsoft.SpeechServices.Common.Client.OneApiState'. Path 'Status', line 1, position 56."
        request.Settings.JsonSerializer = new NewtonsoftJsonSerializer(CommonPublicConst.Json.WriterSettings);

        return request;
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

        while (DateTime.Now - startTime < (timeout ?? CommonPublicConst.Http.LongRunOperationTaskExpiredDuration))
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
                this.Logger?.LogLine(JsonConvert.SerializeObject(
                    translation,
                    Formatting.Indented,
                    CommonPublicConst.Json.WriterSettings));
            }

            if (new[] { OneApiState.Failed, OneApiState.Succeeded }.Contains(translation.Status))
            {
                this.Logger?.LogLine();
                this.Logger?.LogLine();
                this.Logger?.LogLine($"Task completed with state: {translation.Status}");
                if (!runPrinted)
                {
                    this.Logger?.LogLine(JsonConvert.SerializeObject(
                        translation,
                        Formatting.Indented,
                        CommonPublicConst.Json.WriterSettings));
                }

                return translation;
            }
            else
            {
                await Task.Delay(TimeSpan.FromSeconds(3)).ConfigureAwait(false);
                if (state == null || state != translation.Status)
                {
                    this.Logger?.LogLine();
                    this.Logger?.LogLine();
                    this.Logger?.LogLine($"Task {translation.Status}:");
                    state = translation.Status;
                }
                else
                {
                    Console.Write(".");
                }
            }
        }

        this.Logger?.LogLine();
        this.Logger?.LogLine();
        this.Logger?.LogLine($"Task run timeout after {(DateTime.Now - startTime).TotalMinutes.ToString("0.00", CultureInfo.InvariantCulture)} mins");
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

    public Polly.Retry.AsyncRetryPolicy BuildRetryPolicy()
    {
        var retryPolicy = Policy
           .Handle<FlurlHttpException>(IsTransientError)
           .WaitAndRetryAsync(5, retryAttempt =>
           {
               var nextAttemptIn = TimeSpan.FromSeconds(5 * Math.Pow(2, retryAttempt));
               this.Logger?.LogLine($"Retry attempt {retryAttempt} to make request. Next try on {nextAttemptIn.TotalSeconds} seconds.");
               return nextAttemptIn;
           });

        return retryPolicy;
    }

    protected bool IsTransientError(FlurlHttpException exception)
    {
        int[] httpStatusCodesWorthRetrying =
        {
            (int)HttpStatusCode.RequestTimeout, // 408
            (int)HttpStatusCode.BadGateway, // 502
            (int)HttpStatusCode.ServiceUnavailable, // 503
            (int)HttpStatusCode.GatewayTimeout, // 504
            (int)HttpStatusCode.TooManyRequests, // 429
        };

        this.Logger?.LogLine($"Flurl exception status code: {exception.StatusCode}");
        return exception.StatusCode.HasValue && httpStatusCodesWorthRetrying.Contains(exception.StatusCode.Value);
    }
}
