using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;

namespace Azure.AI.Test.Common.Servers.Adapters
{
    /// <summary>
    /// The parsed query parameters of a HTTP request
    /// </summary>
    public interface IHttpQueryParameters : IEnumerable<KeyValuePair<string, IEnumerable<string>>>
    {
        /// <summary>
        /// Gets the number of query parameter keys
        /// </summary>
        int Count { get; }

        /// <summary>
        /// Gets all the query parameter names
        /// </summary>
        IEnumerable<string> Keys { get; }

        /// <summary>
        /// Gets the values for a particular query string
        /// </summary>
        /// <param name="name">The name of the query parameter</param>
        /// <returns>All the values for that query parameter. If the query parameter was not present, you will get back
        /// an empty enumerable. If the query parameter did not have a value set, you will get back an enumerable with
        /// a single empty string
        /// empty string</returns>
        IEnumerable<string> GetValues(string name);
    }

    /// <summary>
    /// Extension methods for HTTP query string
    /// </summary>
    public static class QueryParameterExtensions
    {
        /// <summary>
        /// Gets the single string value for the specified key, or returns the default value
        /// </summary>
        /// <param name="query">The query string collection to read from</param>
        /// <param name="key">The key retrieve</param>
        /// <param name="default">(Optional) The default value to return if the key was not set or is invalid</param>
        /// <returns>The string value, or the default value specified</returns>
        public static string GetStringOrDefault(this IHttpQueryParameters query, string key, string @default = null)
        {
            string strValue;

            if (query != null
                && key != null
                && (strValue = query.GetValues(key)?.FirstOrDefault(v => !string.IsNullOrWhiteSpace(v))) != null)
            {
                return strValue;
            }
            else
            {
                return @default;
            }
        }

        /// <summary>
        /// Gets the integer value for the specified key
        /// </summary>
        /// <param name="query">The query string collection to read from</param>
        /// <param name="key">The key retrieve</param>
        /// <param name="default">(Optional) The default value to return if the key was not set or is invalid</param>
        /// <returns>The integer value, or the default value specified</returns>
        public static int GetIntOrDefault(this IHttpQueryParameters query, string key, int @default = 0)
        {
            string strValue = GetStringOrDefault(query, key, null);
            if (int.TryParse(strValue, NumberStyles.Integer, CultureInfo.InvariantCulture, out int intValue))
            {
                return intValue;
            }
            else
            {
                return @default;
            }
        }

        /// <summary>
        /// Gets the enum value for the specified key
        /// </summary>
        /// <typeparam name="TEnum">The type of the enum to retrieve</typeparam>
        /// <param name="query">The query string collection to read from</param>
        /// <param name="key">The key retrieve</param>
        /// <param name="default">(Optional) The default value to return if the key was not set or is invalid</param>
        /// <returns>The enum value, or the default value specified</returns>
        public static TEnum GetEnumOrDefault<TEnum>(this IHttpQueryParameters query, string key, TEnum @default = default)
            where TEnum : struct
        {
            if (!typeof(TEnum).IsEnum)
            {
                throw new NotSupportedException("The type specified is not an enum: " + typeof(TEnum).AssemblyQualifiedName);
            }

            string strValue = GetStringOrDefault(query, key, null);
            if (Enum.TryParse<TEnum>(strValue, true, out var enumValue))
            {
                return enumValue;
            }

            return @default;
        }

        /// <summary>
        /// Gets the absolute URI for the specified key
        /// </summary>
        /// <param name="query">The query string collection to read from</param>
        /// <param name="key">The key retrieve</param>
        /// <param name="default">(Optional) The default value to return if the key was not set or is invalid</param>
        /// <returns>The URI value, or the default value specified</returns>
        public static Uri GetUriOrDefault(this IHttpQueryParameters query, string key, Uri @default = null)
            => GetUriOrDefault(query, key, UriKind.Absolute, @default);

        /// <summary>
        /// Gets the URI for the specified key
        /// </summary>
        /// <param name="query">The query string collection to read from</param>
        /// <param name="key">The key retrieve</param>
        /// <param name="uriKind">The type of the URI we are expecting</param>
        /// <param name="default">(Optional) The default value to return if the key was not set or is invalid</param>
        /// <returns>The URI value, or the default value specified</returns>
        public static Uri GetUriOrDefault(this IHttpQueryParameters query, string key, UriKind uriKind, Uri @default = null)
        {
            string strValue = GetStringOrDefault(query, key, null);
            if (Uri.TryCreate(strValue, uriKind, out var uri))
            {
                return uri;
            }

            return @default;
        }

        /// <summary>
        /// Gets the value for the specified key attempting to do any supported conversions from a string
        /// </summary>
        /// <typeparam name="TValue">The type of the value</typeparam>
        /// <param name="query">The query string collection to read from</param>
        /// <param name="key">The key retrieve</param>
        /// <param name="default">(Optional) The default value to return if the key was not set or is invalid</param>
        /// <returns>The parsed value, or the default value specified</returns>
        public static TValue GetValueOrDefault<TValue>(this IHttpQueryParameters query, string key, TValue @default = default)
        {
            var converter = TypeDescriptor.GetConverter(typeof(TValue));
            if (converter?.CanConvertFrom(typeof(string)) != true)
            {
                throw new NotSupportedException("Can't convert from a string to " + typeof(TValue).AssemblyQualifiedName);
            }

            var strValue = GetStringOrDefault(query, key, null);
            if (strValue == null)
            {
                return @default;
            }
            else
            {
                return (TValue)converter.ConvertFromString(strValue);
            }
        }
    }
}
