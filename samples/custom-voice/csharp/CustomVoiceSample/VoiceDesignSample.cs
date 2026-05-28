//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// Provide credentials via SPEECH_KEY / SPEECH_REGION environment variables, or by editing the
// constants below.

using System.Net.Http.Headers;
using System.Net.Http.Json;
using System.Text.Json;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

public class VoiceDesignSample
{
    private static readonly string SpeechKey = Environment.GetEnvironmentVariable("SPEECH_KEY") ?? "<paste-your-speech-resource-key-here>";
    private static readonly string SpeechRegion = Environment.GetEnvironmentVariable("SPEECH_REGION") ?? "eastus";

    private const string VoicePrompt = "An elderly man in his seventies with a low, bold, strong voice. He speaks slowly and deliberately.";
    private const string SampleText = "Hello, I am an AI generated voice. Nice to meet you.";
    private const string Locale = "en-US";
    private const int CandidateCount = 3;
    private const int CandidateIndex = 0; // Which candidate to turn into a personal voice (audition the preview WAVs first).

    private const string ProjectId = "voice-design-sample-project";
    private static readonly string PersonalVoiceId = $"voice-design-{Guid.NewGuid().ToString("N").Substring(0, 8)}";

    private const string SynthesisText = "This voice was created from a text prompt -- no recording, no consent file, just words.";
    private static readonly string OutputDir = Path.Combine(AppContext.BaseDirectory, "voice_design_output");

    private const string ApiVersion = "2024-02-01-preview";

    public static async Task VoiceDesignTestAsync()
    {
        if (SpeechKey.StartsWith("<"))
        {
            Console.WriteLine("ERROR: SPEECH_KEY is not set. Set the SPEECH_KEY env var or edit the constant in VoiceDesignSample.cs.");
            return;
        }

        Directory.CreateDirectory(OutputDir);

        using var http = new HttpClient { BaseAddress = new Uri($"https://{SpeechRegion}.api.cognitive.microsoft.com") };
        http.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", SpeechKey);

        await EnsureProjectAsync(http, ProjectId);

        var candidates = await DesignVoiceCandidatesAsync(http, VoicePrompt, SampleText, Locale, CandidateCount);
        await DownloadPreviewsAsync(candidates, OutputDir);

        if (CandidateIndex < 0 || CandidateIndex >= candidates.Count)
        {
            Console.WriteLine($"ERROR: CandidateIndex={CandidateIndex} is out of range (got {candidates.Count} candidates).");
            return;
        }
        var chosen = candidates[CandidateIndex];

        var speakerProfileId = await CreatePersonalVoiceAsync(http, ProjectId, PersonalVoiceId, chosen.GetProperty("id").GetString()!);

        var outputPath = Path.Combine(OutputDir, "voice_design_output.wav");
        await SynthesizeAsync(speakerProfileId, SynthesisText, outputPath);
    }

    // Step 1: create the personal-voice project if it doesn't already exist.
    private static async Task EnsureProjectAsync(HttpClient http, string projectId)
    {
        var response = await http.PutAsJsonAsync(
            $"/customvoice/projects/{projectId}?api-version={ApiVersion}",
            new { kind = "PersonalVoice", description = "voice design sample" });
        response.EnsureSuccessStatusCode();
        await PrintResponseAsync($"PUT project {projectId}", response);
    }

    // Step 2: design voice candidates from the prompt.
    private static async Task<List<JsonElement>> DesignVoiceCandidatesAsync(HttpClient http, string voicePrompt, string sampleText, string locale, int candidateCount)
    {
        var response = await http.PostAsJsonAsync(
            $"/customvoice/personalvoices:design?api-version={ApiVersion}",
            new
            {
                VoicePrompt = voicePrompt,
                SampleText = sampleText,
                candidateCount,
                locale,
            });
        response.EnsureSuccessStatusCode();
        var body = await PrintResponseAsync("POST personalvoices:design", response);

        var candidates = body.GetProperty("candidates").EnumerateArray().ToList();
        if (candidates.Count == 0)
        {
            throw new InvalidOperationException("Service returned no candidates. Try a more descriptive VoicePrompt.");
        }
        return candidates;
    }

    // Step 3: download each candidate's preview WAV. The uri is a short-lived SAS URL.
    private static async Task DownloadPreviewsAsync(List<JsonElement> candidates, string outputDir)
    {
        using var downloader = new HttpClient();
        for (int i = 0; i < candidates.Count; i++)
        {
            var id = candidates[i].GetProperty("id").GetString();
            var uri = candidates[i].GetProperty("uri").GetString()!;
            var path = Path.Combine(outputDir, $"candidate_{i}.wav");
            await using (var fs = File.Create(path))
            await using (var stream = await downloader.GetStreamAsync(uri))
            {
                await stream.CopyToAsync(fs);
            }
            Console.WriteLine($"  candidate {i}: id={id}  preview -> {path}");
        }
        Console.WriteLine();
    }

    // Step 4: turn the chosen candidate into a personal voice and return its speakerProfileId.
    private static async Task<string> CreatePersonalVoiceAsync(HttpClient http, string projectId, string personalVoiceId, string candidateId)
    {
        var response = await http.PutAsJsonAsync(
            $"/customvoice/personalvoices/{personalVoiceId}?api-version={ApiVersion}",
            new { projectId, candidateId });
        response.EnsureSuccessStatusCode();
        var body = await PrintResponseAsync($"PUT personalvoices/{personalVoiceId}", response);
        return body.GetProperty("speakerProfileId").GetString()!;
    }

    // Step 5: synthesize speech using the new personal voice. Identical to any other personal voice.
    private static async Task SynthesizeAsync(string speakerProfileId, string text, string outputPath)
    {
        var ssml = "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' " +
                   "xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'>" +
                   "<voice name='DragonLatestNeural'>" +
                   $"<mstts:ttsembedding speakerProfileId='{speakerProfileId}'>" +
                   $"<lang xml:lang='{Locale}'>{text}</lang>" +
                   "</mstts:ttsembedding></voice></speak>";

        var speechConfig = SpeechConfig.FromSubscription(SpeechKey, SpeechRegion);
        speechConfig.SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm);
        using var audioConfig = AudioConfig.FromWavFileOutput(outputPath);
        using var synthesizer = new SpeechSynthesizer(speechConfig, audioConfig);

        using var result = await synthesizer.SpeakSsmlAsync(ssml).ConfigureAwait(false);
        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
        {
            Console.WriteLine($"Synthesized audio -> {outputPath}");
        }
        else if (result.Reason == ResultReason.Canceled)
        {
            var details = SpeechSynthesisCancellationDetails.FromResult(result);
            throw new InvalidOperationException($"Synthesis canceled: {details.Reason}. {details.ErrorDetails}");
        }
    }

    private static async Task<JsonElement> PrintResponseAsync(string label, HttpResponseMessage response)
    {
        var text = await response.Content.ReadAsStringAsync();
        Console.WriteLine($"--- {label} [{(int)response.StatusCode}] ---");
        try
        {
            using var doc = JsonDocument.Parse(text);
            Console.WriteLine(JsonSerializer.Serialize(doc.RootElement, new JsonSerializerOptions { WriteIndented = true }));
            Console.WriteLine();
            return doc.RootElement.Clone();
        }
        catch (JsonException)
        {
            Console.WriteLine(text);
            Console.WriteLine();
            return default;
        }
    }
}
