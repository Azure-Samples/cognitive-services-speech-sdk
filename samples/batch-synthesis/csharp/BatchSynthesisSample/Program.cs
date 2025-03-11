//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Azure.Core;
using Azure.Identity;

// The endpoint (and key) could be gotten from the Keys and Endpoint page in the Speech service resource.
// The endpoint would be like: https://<region>.api.cognitive.microsoft.com or https://<custom_domain>.cognitiveservices.azure.com
// If you want to use token authentication, custom domain is required.
var host = Environment.GetEnvironmentVariable("SPEECH_ENDPOINT");
BatchSynthesisClient synthesisClient;
var apiVersion = "2024-04-01";
if (!string.IsNullOrEmpty(host))
{
    // Use Azure Identity SDK to acquire token
    var credential = new DefaultAzureCredential();
    var tokenRequestContext = new TokenRequestContext(new[] { "https://cognitiveservices.azure.com/.default" });
    var accessToken = (await credential.GetTokenAsync(tokenRequestContext)).Token;

    // Use access token for authentication
    synthesisClient = new BatchSynthesisClient(host, accessToken, apiVersion, isToken: true);
}
else
{
    string speechRegion = Environment.GetEnvironmentVariable("SPEECH_REGION") ?? throw new ArgumentException("Please set either the SPEECH_REGION or SPEECH_ENDPOINT environment variable.");
    string speechKey = Environment.GetEnvironmentVariable("SPEECH_KEY") ?? throw new ArgumentException("Please set either the SPEECH_KEY and SPEECH_REGION environment variables, or the SPEECH_ENDPOINT environment variable for authentication.");

    host = $"https://{speechRegion}.api.cognitive.microsoft.com";
    synthesisClient = new BatchSynthesisClient(host, speechKey, apiVersion);
}

var sampleScript = await File.ReadAllTextAsync("./Gatsby-chapter1.txt").ConfigureAwait(false);

// Get all synthesis jobs.
var synthesisJobs = await synthesisClient.GetAllSynthesesAsync().ConfigureAwait(false);
Console.WriteLine($"Found {synthesisJobs.Count()} jobs.");

var newJobId = $"SimpleJob-{DateTime.Now:u}".Replace(":", "-").Replace(" ", "-");

// Create a new synthesis job with plain text
await synthesisClient.CreateSynthesisAsync(newJobId, "en-US-AvaMultilingualNeural", sampleScript, false).ConfigureAwait(false);

// Get a synthesis job.
var synthesis = await synthesisClient.GetSynthesisAsync(newJobId).ConfigureAwait(false);

// Poll the synthesis until it completes
var terminatedStates = new[] { "Succeeded", "Failed" };
while (!terminatedStates.Contains(synthesis.Status))
{
    Console.WriteLine($"Synthesis {newJobId}. Status: {synthesis.Status}");
    await Task.Delay(TimeSpan.FromSeconds(30)).ConfigureAwait(false);
    synthesis = await synthesisClient.GetSynthesisAsync(newJobId).ConfigureAwait(false);
}

Console.WriteLine($"Synthesis {newJobId}. Status: {synthesis.Status}");

// Get outputs of the synthesis
if (!string.IsNullOrEmpty(synthesis.Outputs?.Result))
{
    Console.WriteLine("Please download result from this URL before you delete the synthesis.");
    Console.WriteLine(synthesis.Outputs.Result);
}

if (!string.IsNullOrEmpty(synthesis.Outputs?.Summary))
{
    Console.WriteLine("Please download summary file from this URL before you delete the synthesis.");
    Console.WriteLine(synthesis.Outputs.Summary);
}

// Ask for confirmation before deleting the synthesis
Console.Write("Do you want to delete the synthesis job? (y/n): ");
var input = Console.ReadLine();
if (input?.Trim().ToLower() == "y")
{
    await synthesisClient.DeleteSynthesisAsync(newJobId);
    Console.WriteLine($"Deleted synthesis {newJobId}.");
}
else
{
    Console.WriteLine("Synthesis deletion cancelled.");
}