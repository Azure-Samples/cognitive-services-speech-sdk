//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib;

using Newtonsoft.Json.Converters;
using Newtonsoft.Json.Serialization;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;

public static class CommonPublicConst
{
    public static class ArgumentDescription
    {
        public const string Region = "specify speech resource region.";

        public const string SubscriptionKey = "Specify speech resource key for authentication by key, then don't need specify customDomainName.";

        public const string CustomDomainName = "Specify custom domain name part of the endpoint: https://[customDomainName].cognitiveservices.azure.com/, then don't need specify subscriptionKey.";

        public const string ManagedIdentityClientId = "Specify managed identity client ID, only optional available when specify custom domain.";

        public const string ApiVersion = "Specify API version.";
    }

    public static class Json
    {
        // Default json serializer will serialize enum to number, which will cause API parse DTO failure:
        //  "Error converting value 0 to type 'Microsoft.SpeechServices.Common.Client.OneApiState'. Path 'Status', line 1, position 56."
        public static JsonSerializerSettings WriterSettings { get; } = new JsonSerializerSettings
        {
            ContractResolver = new CamelCasePropertyNamesContractResolver(),
            Converters = new List<JsonConverter> { new StringEnumConverter() { AllowIntegerValues = false } },
            DateFormatString = "yyyy-MM-ddTHH\\:mm\\:ss.fffZ",
            NullValueHandling = NullValueHandling.Ignore,
            Formatting = Formatting.Indented,
        };

        public static JsonSerializerSettings ReaderSettings { get; } = new JsonSerializerSettings
        {
            ContractResolver = new CamelCasePropertyNamesContractResolver(),
            Converters = new List<JsonConverter> { new StringEnumConverter() { AllowIntegerValues = true } },
            Formatting = Formatting.Indented
        };
    }

    public static class EventHub
    {
        public static class HeaderNames
        {
            public const string EventKind = "MS-VideoTranslation-Event";
        }
    }

    public static class ExistCodes
    {
        public const int NoError = 0;
        public const int GenericError = -1;
    }

    public static class ApiVersions
    {
        public const string ApiVersion30beta1 = "v3.0-beta1";
        public const string ApiVersion20230401Preview = "2023-04-01-preview";
        public const string ApiVersion20230701Preview = "2023-07-01-preview";
        public const string ApiVersion20240520Preview = "2024-05-20-preview";
        public const string ApiVersion20250520 = "2025-05-20";
        public const string ApiVersion20260101Preview = "2026-01-01-preview";
        public const string ApiVersion20260301 = "2026-03-01";
    }

    public static class Http
    {
        public readonly static TimeSpan UploadFileTimeout = TimeSpan.FromMinutes(10);

        public static readonly TimeSpan OperationQueryDuration = TimeSpan.FromSeconds(3);

        public static readonly TimeSpan LongRunOperationTaskExpiredDuration = TimeSpan.FromHours(3);

        public static class ParameterNames
        {
            public const string ApiVersion = "api-version";
        }

        public static class Headers
        {
            public const string SubscriptionKey = "Ocp-Apim-Subscription-Key";
            public const string OperationId = "Operation-Id";
            public const string OperationLocation = "Operation-Location";

            public readonly static IEnumerable<string> OperationHeaders = new[]
            {
                OperationId,
                OperationLocation,
            };
        }
    }
}
