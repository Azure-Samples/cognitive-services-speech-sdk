//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib;

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Microsoft.SpeechServices.CommonLib.Enums;
using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using Newtonsoft.Json.Serialization;

public class CustomContractResolver : CamelCasePropertyNamesContractResolver
{
    public static readonly CustomContractResolver ReaderContractResolver = new CustomContractResolver();
    public static readonly CustomContractResolver WriterContractResolver = new CustomContractResolver();

    public static JsonSerializerSettings WriterSettings { get; } = new JsonSerializerSettings
    {
        ContractResolver = WriterContractResolver,
        ConstructorHandling = ConstructorHandling.AllowNonPublicDefaultConstructor,
        Converters = new List<JsonConverter> { new StringEnumConverter() { AllowIntegerValues = false } },
        DateFormatString = "yyyy-MM-ddTHH\\:mm\\:ss.fffZ",
        NullValueHandling = NullValueHandling.Ignore,
        Formatting = Formatting.Indented,
        ReferenceLoopHandling = ReferenceLoopHandling.Ignore
    };

    public static JsonSerializerSettings ReaderSettings { get; } = new JsonSerializerSettings
    {
        ContractResolver = ReaderContractResolver,
        ConstructorHandling = ConstructorHandling.AllowNonPublicDefaultConstructor,
        Converters = new List<JsonConverter> { new StringEnumConverter() { AllowIntegerValues = true } },
        Formatting = Formatting.Indented
    };

    public static string GetResolvedPropertyName(PropertyInfo property)
    {
        ArgumentNullException.ThrowIfNull(property);

        string propertyName;
        var jsonAttribute = property.GetCustomAttributes(typeof(JsonPropertyAttribute)).Cast<JsonPropertyAttribute>().FirstOrDefault();
        if (jsonAttribute != null && !string.IsNullOrWhiteSpace(jsonAttribute.PropertyName))
        {
            propertyName = jsonAttribute.PropertyName;
        }
        else
        {
            propertyName = ReaderContractResolver.GetResolvedPropertyName(property.Name);
        }

        return propertyName;
    }

    protected override JsonProperty CreateProperty(MemberInfo member, MemberSerialization memberSerialization)
    {
        var property = base.CreateProperty(member, memberSerialization);

        if (!property.Writable)
        {
            var propertyInfo = member as PropertyInfo;
            if (propertyInfo != null)
            {
                property.Writable = propertyInfo.CanWrite;
            }
        }

        const string createdDateTime = "CreatedDateTime";
        const string lastActionDateTime = "LastActionDateTime";
        const string status = "Status";
        const string timeToLive = "TimeToLive";
        const string duration = "Duration";
        const string customProperties = "CustomProperties";

        if (property.PropertyType == typeof(DateTime) && property.PropertyName == this.ResolvePropertyName(createdDateTime))
        {
            property.ShouldSerialize =
                instance =>
                {
                    var value = (DateTime)instance.GetType().GetProperty(createdDateTime).GetValue(instance);
                    return value != default(DateTime);
                };
        }
        else if (property.PropertyType == typeof(DateTime) && property.PropertyName == this.ResolvePropertyName(lastActionDateTime))
        {
            property.ShouldSerialize =
                instance =>
                {
                    var value = (DateTime)instance.GetType().GetProperty(lastActionDateTime).GetValue(instance);
                    return value != default(DateTime);
                };
        }
        else if (property.PropertyType == typeof(OneApiState) && property.PropertyName == this.ResolvePropertyName(status))
        {
            property.ShouldSerialize =
                instance =>
                {
                    var value = (OneApiState)instance.GetType().GetProperty(status).GetValue(instance);
                    return value != default(OneApiState);
                };
        }
        else if (property.PropertyType == typeof(TimeSpan) && property.PropertyName == this.ResolvePropertyName(timeToLive))
        {
            property.ShouldSerialize =
                instance =>
                {
                    var value = (TimeSpan)instance.GetType().GetProperty(timeToLive).GetValue(instance);
                    return value != TimeSpan.Zero;
                };
        }
        else if (property.PropertyType == typeof(TimeSpan) && property.PropertyName == this.ResolvePropertyName(duration))
        {
            property.ShouldSerialize =
                instance =>
                {
                    var value = (TimeSpan)instance.GetType().GetProperty(duration).GetValue(instance);
                    return value != TimeSpan.Zero;
                };
        }
        else if (property.PropertyType == typeof(IReadOnlyDictionary<string, string>) && property.PropertyName == this.ResolvePropertyName(customProperties))
        {
            property.ShouldSerialize =
                instance =>
                {
                    var value = (IReadOnlyDictionary<string, string>)instance.GetType().GetProperty(customProperties).GetValue(instance);
                    return value != null && value.Count > 0;
                };
        }

        return property;
    }

    // do not javascriptify (camel case) dictionary keys. This would e.g. change
    // key in artifact properties.
    protected override string ResolveDictionaryKey(string dictionaryKey)
    {
        return dictionaryKey;
    }
}
