using System;
using System.Text;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc.Formatters;

namespace Azure.AI.Test.Common.Servers.Adapters.NetCore
{
    /// <summary>
    /// A formatter to ensure that .Net core does not send back string HTTP response bodies with
    /// using the chunked transport format. Basically a super fancy way to set the Content-Length
    /// before we write to the response body stream
    /// </summary>
    public class NonChunkedStringOutputFormatter : TextOutputFormatter
    {
        /// <summary>
        /// Creates a new instance
        /// </summary>
        public NonChunkedStringOutputFormatter()
        {
            SupportedEncodings.Add(Encoding.UTF8);
            SupportedEncodings.Add(Encoding.Unicode);
            SupportedMediaTypes.Add("text/plain");
        }

        /// <inheritdoc />
        public override bool CanWriteResult(OutputFormatterCanWriteContext context)
        {
            if (context == null)
            {
                throw new ArgumentNullException(nameof(context));
            }

            if (context.ObjectType == typeof(string) || context.Object is string)
            {
                // Call into base to check if the current request's content type is a supported media type.
                return base.CanWriteResult(context);
            }

            return false;
        }

        /// <inheritdoc/>
        public override Task WriteResponseBodyAsync(
            OutputFormatterWriteContext context,
            Encoding encoding)
        {
            if (context == null)
            {
                throw new ArgumentNullException(nameof(context));
            }
            if (encoding == null)
            {
                throw new ArgumentNullException(nameof(encoding));
            }

            var valueAsString = (string)context.Object;
            if (string.IsNullOrEmpty(valueAsString))
            {
                return Task.CompletedTask;
            }

            var bytes = encoding.GetBytes(valueAsString);
            context.HttpContext.Response.ContentType = context.ContentType.Value;
            context.HttpContext.Response.ContentLength = bytes.Length;

            return context.HttpContext.Response.Body.WriteAsync(
                bytes,
                0,
                bytes.Length,
                context.HttpContext.RequestAborted);
        }
    }
}
