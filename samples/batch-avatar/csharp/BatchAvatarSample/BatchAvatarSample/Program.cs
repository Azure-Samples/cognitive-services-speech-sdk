//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using BatchAvatarSample.dto;
using System.Net.Http.Json;
using System.Text.Json;
using System.Text.Json.Serialization;

internal class Program
{
    private const string ApiVersion = "2024-04-15-preview";

    // The endpoint (and key) could be gotten from the Keys and Endpoint page in the Speech service resource.
    // The endpoint would be like: https://<region>.api.cognitive.microsoft.com or https://<custom_domain>.cognitiveservices.azure.com
    private static readonly string subscriptionKey = "SPEECH_KEY";
    private static readonly string speechEndpoint = "SPEECH_ENDPOINT"; 

    private static readonly JsonSerializerOptions defaultJsonSerializerOptions = new(JsonSerializerDefaults.Web)
    {
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
    };

    private static async Task Main(string[] args)
    {
        using var httpClient = new HttpClient();
        httpClient.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", subscriptionKey);

        // Create a job ID.
        // Job ID must be unique within a speech resource.
        var jobId = $"batchavatar-{Guid.NewGuid()}";

        //
        // Submit a batch avatar job
        //
        var jobUri = $"{speechEndpoint}/avatar/batchsyntheses/{jobId}?api-version={ApiVersion}";

        try
        {
            var job = await CreateBatchAvatarJob(httpClient, jobUri);
            Console.WriteLine($"Batch avatar synthesis job {jobId} created.");

            //
            // Get job status
            //
            job = await GetJobAsync(httpClient, jobUri);
            Console.WriteLine($"Batch avatar synthesis job {job.Id} is in {job.Status} status.");

            //
            // Poll until job completes
            //
            while (job.Status is not ("Succeeded" or "Failed"))
            {
                Console.WriteLine($"Batch avatar synthesis job {job.Id} is still running.");
                await Task.Delay(5000);
                job = await GetJobAsync(httpClient, jobUri);
            }

            if (job.Status is "Failed")
            {
                Console.WriteLine($"Batch avatar synthesis job {job.Id} failed.");
                return;
            }

            Console.WriteLine($"Batch avatar synthesis job {job.Id} completed successfully.");

            //
            // Get outputs
            //
            Console.WriteLine("Video file can be downloaded from:");
            Console.WriteLine(job.Outputs!.Result);
            Console.WriteLine("Summary file can be downloaded from:");
            Console.WriteLine(job.Outputs!.Summary);

            //
            // Delete a job
            //
            await DeleteJobAsync(httpClient, jobUri);
            Console.WriteLine($"Batch avatar synthesis job {job.Id} deleted.");

            //
            // List jobs
            //
            var maxpagesize = 10;
            var listUri = $"{speechEndpoint}/avatar/batchsyntheses?maxpagesize={maxpagesize}&api-version={ApiVersion}";
            var allJobs = await ListJobsAsync(httpClient, listUri);
            Console.WriteLine($"Listed {allJobs.Count} jobs.");
        }
        catch (HttpRequestException exception)
        {
            Console.Error.WriteLine(exception.Message);
            return;
        }
    }


    private static async Task<BatchAvatarJob> CreateBatchAvatarJob(HttpClient httpClient, string jobUri)
    {
        // To use SSML as input, please refer to RequestExamples.SsmlRequest
        // To use your custom neural voice, please refer to RequestExamples.CustomVoiceRequest
        var requestBody = new BatchAvatarRequest
        {
            InputKind = "PlainText",
            Inputs =
            [
                new BatchAvatarInput
                {
                    Content = "Hi, I'm a virtual assistant created by Microsoft.",
                },
            ],
            SynthesisConfig = new()
            {
                Voice = "en-US-AvaMultilingualNeural",
            },
            AvatarConfig = new()
            {
                TalkingAvatarCharacter = "lisa",            // Avatar character
                TalkingAvatarStyle = "graceful-sitting",    // Avatar style, required for prebuilt avatar, optional for custom avatar
                VideoFormat = "mp4",                        // mp4 or webm, webm is required for transparent background
                VideoCodec = "h264",                        // hevc, h264 or vp9, vp9 is required for transparent background; default is hevc
                SubtitleType = "soft_embedded",             
                BackgroundColor = "#FFFFFFFF",              // background color in RGBA format, default is white; can be set to 'transparent' for transparent background
                Customized = false,                         // Set to true if you want to use custom avatar
            },
        };

        var response = await httpClient.PutAsJsonAsync(jobUri, requestBody, defaultJsonSerializerOptions);
        await PrintResponseOnError(response);
        response.EnsureSuccessStatusCode();
        var createdJob = await response.Content.ReadFromJsonAsync<BatchAvatarJob>(defaultJsonSerializerOptions);
        return createdJob!;
    }

    private static async Task<BatchAvatarJob> GetJobAsync(HttpClient httpClient, string jobUri)
    {
        var response = await httpClient.GetAsync(jobUri);
        await PrintResponseOnError(response);
        response.EnsureSuccessStatusCode();
        var job = await response.Content.ReadFromJsonAsync<BatchAvatarJob>(defaultJsonSerializerOptions);
        return job!;
    }

    private static async Task DeleteJobAsync(HttpClient httpClient, string jobUri)
    {
        var response = await httpClient.DeleteAsync(jobUri);
        await PrintResponseOnError(response);
        response.EnsureSuccessStatusCode();
    }

    private static async Task<IList<BatchAvatarJob>> ListJobsAsync(HttpClient httpClient, string listUri)
    {
        var allJobs = new List<BatchAvatarJob>();
        var response = await httpClient.GetAsync(listUri);
        await PrintResponseOnError(response);
        response.EnsureSuccessStatusCode();

        var pagedJobs = await response.Content.ReadFromJsonAsync<PaginatedResults<BatchAvatarJob>>(defaultJsonSerializerOptions);
        allJobs.AddRange(pagedJobs!.Value);
        while (pagedJobs!.NextLink is not null)
        {
            response = await httpClient.GetAsync(pagedJobs.NextLink);
            await PrintResponseOnError(response);
            response.EnsureSuccessStatusCode();
            
            pagedJobs = await response.Content.ReadFromJsonAsync<PaginatedResults<BatchAvatarJob>>(defaultJsonSerializerOptions);
            allJobs.AddRange(pagedJobs!.Value);
        }

        return allJobs;
    }

    private static async Task PrintResponseOnError(HttpResponseMessage response)
    {
        if (response.IsSuccessStatusCode)
        {
            return;
        }

        var responseBody = await response.Content.ReadAsStringAsync();
        var requestId = response.Headers.GetValues("apim-request-id").FirstOrDefault();
        Console.Error.WriteLine(responseBody);
        Console.Error.WriteLine($"Request ID: {requestId}");
    }
}