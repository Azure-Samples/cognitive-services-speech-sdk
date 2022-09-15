//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// The host name of the batch sytnhesis API, e.g. https://eastus.customvoice.api.speech.microsoft.com
var hostName = "https://centraluseuap.customvoice.api.speech.microsoft.com";
var subscriptionKey = "ab0c1d61c9aa45a0860affa6874b4e51";

var sampleScript = "This is the waistline, and it's falling.\r\nAlexis, meet Bill and Hillary, and the rest of America.\r\nThis is Jordan, Scottie Pippen and the ring dynasty.\r\nThe more I looked, the gloomier I got.\r\nHe saw Macbeth, the three witches, and the boiling cauldron.\r\nBut it is good to get together, cook together, eat together and enjoy.\r\nThe priorities, they say, are sofa, blanket, and clothing.\r\nThe metallic walls curve, twist, and turn.\r\nThe seagrass fiber is tough, durable and smooth.\r\nThe dissenters were Stevens, Souter, Ginsburg and Breyer.";

var synthesisClient = new BatchSynthesisClient(hostName, subscriptionKey);

// Get all synthesis tasks.
var syntheses = await synthesisClient.GetAllSynthesesAsync().ConfigureAwait(false);

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
