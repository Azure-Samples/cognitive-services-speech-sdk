//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace WebHookReceiver
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Security.Cryptography;
    using System.Text;
    using System.Threading.Tasks;
    using System.Web.Http;
    using BatchClient;
    using Microsoft.AspNetCore.Http;
    using Microsoft.AspNetCore.Mvc;
    using Microsoft.Azure.WebJobs;
    using Microsoft.Azure.WebJobs.Extensions.Http;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public static class WebHookReceiver
    {
        private const string WebHookEventKindHeaderName = "X-MicrosoftSpeechServices-Event";
        public const string WebHookSignatureHeaderName = "X-MicrosoftSpeechServices-Signature";
        private const string ValidationTokenKeyQueryParameterName = "validationToken";

        [FunctionName("WebHookReceiver")]
        public static async Task<IActionResult> Run(
            [HttpTrigger(AuthorizationLevel.Anonymous, "post", Route = "callback")] HttpRequest request,
            ILogger logger)
        {
            logger.LogInformation("C# HTTP trigger function processed a request.");

            var headers = new Dictionary<string, string>();
            foreach (var header in request.Headers)
            {
                headers.Add(header.Key, header.Value.ToString());
            }

            if (!headers.TryGetValue(WebHookEventKindHeaderName, out var eventKindString) ||
                !Enum.TryParse<WebHookEventKind>(eventKindString, out var eventKind))
            {
                var message = $"Missing or invalid value for required header \"{WebHookEventKindHeaderName}\".";
                logger.LogError(message);

                return new BadRequestErrorMessageResult(message);
            }

            string requestBody = null, validationToken = null, payload;
            if (eventKind == WebHookEventKind.Challenge)
            {
                validationToken = request.Query[ValidationTokenKeyQueryParameterName].FirstOrDefault();
                payload = validationToken;
            }
            else
            {
                requestBody = await new StreamReader(request.Body).ReadToEndAsync();
                payload = requestBody;
            }

            if (headers.TryGetValue(WebHookSignatureHeaderName, out var actualSignature))
            {
                var contentBytes = Encoding.UTF8.GetBytes(payload);
                var secretBytes = Encoding.UTF8.GetBytes(Program.WebHookSecret);
                using (var hmacsha256 = new HMACSHA256(secretBytes))
                {
                    var hash = hmacsha256.ComputeHash(contentBytes);
                    var expectedSignature = Convert.ToBase64String(hash);

                    if (expectedSignature != actualSignature)
                    {
                        logger.LogError($"Notification has invalid signature.");
                        return new BadRequestErrorMessageResult("Invalid signature detected.");
                    }
                }
            }

            if (eventKind == WebHookEventKind.Challenge)
            {
                logger.LogInformation("Received challenge and responded.");

                return new OkObjectResult(validationToken);
            }

            logger.LogInformation($"Received web hook notification, kind={eventKindString}");

            var webHookNotification = JsonConvert.DeserializeObject<WebHookNotification>(requestBody);

            // invocationId can be used for deduplication, it's unique per notification event
            logger.LogInformation($"Processing notification {webHookNotification.InvocationId}.");

            using (var client = BatchClient.CreateApiV3Client(Program.SubscriptionKey, $"{Program.Region}.api.cognitive.microsoft.com"))
            {
                if (eventKind == WebHookEventKind.TranscriptionCompletion)
                {
                    var transcription = await client.GetTranscriptionAsync(webHookNotification.Self).ConfigureAwait(false);
                    var paginatedfiles = await client.GetTranscriptionFilesAsync(transcription.Links.Files).ConfigureAwait(false);
                    var resultFile = paginatedfiles.Values.FirstOrDefault(f => f.Kind == ArtifactKind.Transcription);
                    var result = await client.GetTranscriptionResultAsync(new Uri(resultFile.Links.ContentUrl)).ConfigureAwait(false);

                    logger.LogInformation("Transcription succeeded. Results: ");
                    logger.LogInformation(JsonConvert.SerializeObject(result, SpeechJsonContractResolver.WriterSettings));

                    await client.DeleteTranscriptionAsync(webHookNotification.Self).ConfigureAwait(false);
                }
            }

            return new OkResult();
        }
    }
}
