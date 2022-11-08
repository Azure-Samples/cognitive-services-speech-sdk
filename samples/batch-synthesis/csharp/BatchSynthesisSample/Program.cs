//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


// Your Speech resource key and region
// This example requires environment variables named "SPEECH_KEY" and "SPEECH_REGION"
string speechRegion;
string speechKey;

if (Environment.GetEnvironmentVariable("SPEECH_REGION") is string regionValue)
{
    speechRegion = regionValue;
}
else
{
    throw new ArgumentException($"Please set the SPEECH_REGION environment variable to set speech resource region.");
}

if (Environment.GetEnvironmentVariable("SPEECH_KEY") is string keyValue)
{
    speechKey = keyValue;
}
else
{
    throw new ArgumentException($"Please set the SPEECH_KEY environment variable to set speech resource key.");
}

var host = $"https://{speechRegion}.customvoice.api.speech.microsoft.com";

var sampleScript = "This is the waistline, and it's falling.\nAlexis, meet Bill and Hillary, and the rest of America.\nThis is Jordan, Scottie Pippen and the ring dynasty.\nThe more I looked, the gloomier I got.\nHe saw Macbeth, the three witches, and the boiling cauldron.\nBut it is good to get together, cook together, eat together and enjoy.\nThe priorities, they say, are sofa, blanket, and clothing.\nThe metallic walls curve, twist, and turn.\nThe seagrass fiber is tough, durable and smooth.\nThe dissenters were Stevens, Souter, Ginsburg and Breyer.";

var synthesisClient = new BatchSynthesisClient(host, speechKey);

// Get all synthesis tasks.
var synthesisJobs = await synthesisClient.GetAllSynthesesAsync().ConfigureAwait(false);

// Create a new synthesis task with plain text
var newSynthesisUri = await synthesisClient.CreateSynthesisAsync(
    "en-US-JennyNeural",
    "sample batch synthesis",
    "sample description",
    sampleScript,
    false).ConfigureAwait(false);

var newSynthesisId = Guid.Parse(newSynthesisUri.Segments.Last());

// Get a synthesis task.
var synthesis = await synthesisClient.GetSynthesisAsync(newSynthesisId).ConfigureAwait(false);

// Poll the synthesis until it completes
var terminatedStates = new[] { "Succeeded", "Failed" };
while (!terminatedStates.Contains(synthesis.Status))
{
    Console.WriteLine($"Synthesis {newSynthesisId}. Status: {synthesis.Status}");
    await Task.Delay(TimeSpan.FromSeconds(30)).ConfigureAwait(false);
    synthesis = await synthesisClient.GetSynthesisAsync(newSynthesisId).ConfigureAwait(false);
}

// Get outputs of the synthesis
if (!string.IsNullOrEmpty(synthesis.Outputs.Result))
{
    Console.WriteLine("Please download result from this URL before you delete the synthesis.");
    Console.WriteLine(synthesis.Outputs.Result);
}

if (!string.IsNullOrEmpty(synthesis.Outputs.Summary))
{
    Console.WriteLine("Please download summary file from this URL before you delete the synthesis.");
    Console.WriteLine(synthesis.Outputs.Summary);
}

// Delete a specific synthesis
await synthesisClient.DeleteSynthesisAsync(newSynthesisId);
Console.WriteLine($"Deleted synthesis {newSynthesisId}.");
