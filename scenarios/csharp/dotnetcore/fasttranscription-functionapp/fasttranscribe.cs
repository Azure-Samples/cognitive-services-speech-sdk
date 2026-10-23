using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Extensions.Logging;
using Azure.Storage.Blobs;
using Azure.Identity;
using Azure.Core;
using Newtonsoft.Json;
using System.Net.Http.Json;

namespace FastTranscription.Function
{
    public class FastTranscribe
    {
        private readonly ILogger<FastTranscribe> _logger;

        public FastTranscribe(ILogger<FastTranscribe> logger)
        {
            _logger = logger;
        }

        public sealed class FastTranscriptionOptions
        {
            public string[] Locales { get; set; } = { "en-US" };
        }

        public class TranscriptionPayload
        {
            public string AudioFileUrl { get; set; }

            public string CustomEndpoint { get; set; }
        }

        [Function("fasttranscribe")]
        public async Task<IActionResult> Run([HttpTrigger(AuthorizationLevel.Function, "post")] HttpRequest req)
        {
            var requestStr = await new StreamReader(req.Body).ReadToEndAsync();
            var request = JsonConvert.DeserializeObject<TranscriptionPayload>(requestStr);

            if (request == null || string.IsNullOrEmpty(request.AudioFileUrl) || string.IsNullOrEmpty(request.CustomEndpoint)) 
            {
                return new BadRequestObjectResult("AudioFileUrl and CustomEndpoint fields are required.");
            }

            _logger.LogInformation($"POST /fasttranscribe on '{request.AudioFileUrl}'.");

            // Construct the blob container endpoint from the arguments.
            var blobUrlBuilder = new BlobUriBuilder(new Uri(request.AudioFileUrl));
            var containerEndpoint = string.Format("https://{0}.blob.core.windows.net/{1}", blobUrlBuilder.AccountName, blobUrlBuilder.BlobContainerName);
            
            // Important!!
            // Ensure you or Managed Identity of the function app has "Storage Blob Data Contributor" and "Cognitive Services User" RBAC roles for storage account and speech/AI resource respectively. 
            var azureCredential = new DefaultAzureCredential();
            var containerClient = new BlobContainerClient(new Uri(containerEndpoint), azureCredential);

            var accessToken = await azureCredential.GetTokenAsync(new TokenRequestContext(new[] { "https://cognitiveservices.azure.com/.default" }));

            try
            {
                // Create the container if it does not exist.
                var blobClient = containerClient.GetBlobClient(blobUrlBuilder.BlobName);
                if (!blobClient.Exists())
                {
                    return new BadRequestObjectResult($"{request.AudioFileUrl} does not exist.");
                }

                var httpClient = new HttpClient();
                httpClient.DefaultRequestHeaders.Add("Authorization", $"Bearer {accessToken.Token}");

                var multipartFormDataContent = new MultipartFormDataContent();
                var fastTranscriptionOptions = new FastTranscriptionOptions();
                var fastTranscriptionOptionsDefinition = JsonContent.Create(fastTranscriptionOptions);
                multipartFormDataContent.Add(fastTranscriptionOptionsDefinition, "definition");

                var memoryStream = new MemoryStream();
                var blob = await blobClient.DownloadToAsync(memoryStream);
                var streamContent = new ByteArrayContent(memoryStream.ToArray());
                streamContent.Headers.Add("Content-Type", "multipart/form-data");
                multipartFormDataContent.Add(streamContent, "audio", blobUrlBuilder.BlobName);

                // Call fast transcription
                var speechEndpoint = $"{request.CustomEndpoint}/speechtotext/transcriptions:transcribe?api-version=2024-11-15";
                var response = await httpClient.PostAsync(speechEndpoint, multipartFormDataContent);

                var content = await response.Content.ReadAsStreamAsync();
                var transcriptionFileName = blobUrlBuilder.BlobName + ".transcript";
                var transcriptionClient = containerClient.GetBlobClient(transcriptionFileName);

                // Save the transcription
                await transcriptionClient.UploadAsync(content, overwrite:true);
                return new OkResult();
            }
            catch (Exception e)
            {
                _logger.LogError($"POST /fasttranscribe failed with '{e.Message}'.");
                return new BadRequestObjectResult(e.Message);
            }
        }
    }
}