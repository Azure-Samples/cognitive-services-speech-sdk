using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Microsoft.AspNetCore.Http;

namespace Azure.AI.Test.Common.Servers.Adapters.NetCore
{
    /// <summary>
    /// Query parameters adapter for .Net core implementation
    /// </summary>
    public class HttpQueryParamsAdapter : IHttpQueryParameters
    {
        private IQueryCollection _query;

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="query">The query collection to wrap</param>
        public HttpQueryParamsAdapter(IQueryCollection query)
        {
            _query = query ?? throw new ArgumentNullException(nameof(query));
        }

        /// <inheritdoc />
        public int Count => _query.Count;

        /// <inheritdoc />
        public IEnumerable<string> Keys => _query.Keys;

        /// <inheritdoc />
        public IEnumerable<string> GetValues(string name)
            => _query[name];

        /// <inheritdoc />
        public IEnumerator<KeyValuePair<string, IEnumerable<string>>> GetEnumerator()
            => _query.Select(kvp => new KeyValuePair<string, IEnumerable<string>>(kvp.Key, kvp.Value)).GetEnumerator();

        /// <inheritdoc />
        IEnumerator IEnumerable.GetEnumerator() => GetEnumerator();
    }
}
