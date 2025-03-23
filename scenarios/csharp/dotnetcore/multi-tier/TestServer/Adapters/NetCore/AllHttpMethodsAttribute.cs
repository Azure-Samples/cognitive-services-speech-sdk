using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Reflection;
using Microsoft.AspNetCore.Mvc.Routing;

namespace Azure.AI.Test.Common.Servers.Adapters
{
    public class AllHttpMethodsAttribute : HttpMethodAttribute
    {
        public AllHttpMethodsAttribute() : base(new string[] { })
        { }

public static IEnumerable<string> EnumerateSupportedMethods()
    => typeof(HttpMethod)
        .GetProperties(BindingFlags.Public | BindingFlags.Static)
        .Where(p => p.PropertyType == typeof(HttpMethod))
        .Select(p => ((HttpMethod)p.GetGetMethod().Invoke(null, null)).Method);
    }
}
