//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

public class PersonalVoiceSample
{
    // Settings are loaded from environment variables.
    // You can set them in a .env file (copy .env.sample to .env and fill in your values),
    // or configure them in .vscode/launch.json "envFile" / "env" settings.

    private static readonly string subscriptionKey = Environment.GetEnvironmentVariable("SPEECH_KEY") ?? "";
    private static readonly string region = Environment.GetEnvironmentVariable("SPEECH_REGION") ?? "eastus";

    // Voice settings
    private static readonly string voiceTalentName = Environment.GetEnvironmentVariable("VOICE_TALENT_NAME") ?? "Sample Voice Actor";
    private static readonly string companyName = Environment.GetEnvironmentVariable("COMPANY_NAME") ?? "Contoso";
    private static readonly string locale = Environment.GetEnvironmentVariable("LOCALE") ?? "en-US";
    private static readonly string consentFilePath = Environment.GetEnvironmentVariable("CONSENT_FILE_PATH") ?? "TestData/VoiceTalentVerbalStatement.wav";
    private static readonly string audioFolder = Environment.GetEnvironmentVariable("AUDIO_FOLDER") ?? "TestData/PersonalVoice";

    // Resource IDs (can keep defaults)
    private static readonly string projectId = Environment.GetEnvironmentVariable("PERSONAL_PROJECT_ID") ?? "personal-voice-project-1";
    private static readonly string consentId = Environment.GetEnvironmentVariable("PERSONAL_CONSENT_ID") ?? "personal-voice-consent-1";
    private static readonly string personalVoiceId = Environment.GetEnvironmentVariable("PERSONAL_VOICE_ID") ?? "personal-voice-1";

    public static async Task PersonalVoiceTestAsync()
    {
        if (string.IsNullOrEmpty(subscriptionKey))
        {
            Console.WriteLine("ERROR: SPEECH_KEY environment variable is not set.");
            Console.WriteLine();
            Console.WriteLine("Please copy .env.sample to .env and fill in your values, or set them in .vscode/launch.json.");
            return;
        }

        var client = new CustomVoiceClient(region, subscriptionKey);

        try
        {
            Console.WriteLine("Personal voice test starts.");

            // Step 1: Create a project
            var project = await client.CreateProjectAsync(projectId, ProjectKind.PersonalVoice, "Test project for personal voice");
            Console.WriteLine($"Project created. project id: {project.Id}");

            // Step 2: Create a consent
            var consent = await client.UploadConsentAsync(
                consentId,
                projectId,
                voiceTalentName,
                companyName,
                locale,
                consentFilePath);
            Console.WriteLine($"Consent created. consent id: {consent.Id}");

            // Step 3: Create a personal voice
            var personalVoice = await client.CreatePersonalVoiceAsync(
                personalVoiceId,
                projectId,
                "personal voice create test",
                consentId,
                audioFolder).ConfigureAwait(false);

            // Step 4: speak test
            var outputFilePath = "personalvoice_output.wav";
            await PersonalVoiceSpeechSynthesisToWaveFileAsync("This is a personal voice test!", outputFilePath, personalVoice.SpeakerProfileId);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
        }
        finally
        {
            // Step 5: clean up
            Console.WriteLine("Uncomment below and Clean up resources if you don't need them.");

            // await client.DeleteConsentAsync(consentId).ConfigureAwait(false);
            // await client.DeletePersonalVoiceAsync(personalVoiceId).ConfigureAwait(false);
            // await client.DeleteProjectAsync(projectId).ConfigureAwait(false);
        }
    }

    private static async Task PersonalVoiceSpeechSynthesisToWaveFileAsync(string text, string outputFilePath, Guid speakerProfileId)
    {
        var speechConfig = SpeechConfig.FromSubscription(subscriptionKey, region);
        using var audioConfig = AudioConfig.FromWavFileOutput(outputFilePath);
        using var synthesizer = new SpeechSynthesizer(speechConfig, audioConfig);

        var ssml = $"<speak version='1.0' xml:lang='en-US' xmlns='http://www.w3.org/2001/10/synthesis' " +
           "xmlns:mstts='http://www.w3.org/2001/mstts'>" +
           "<voice name='DragonLatestNeural'>" +
           $"<mstts:ttsembedding speakerProfileId='{speakerProfileId}'/>" +
           "<mstts:express-as style='Prompt'>" +
           $"<lang xml:lang='en-US'> {text} </lang>" +
           "</mstts:express-as>" +
           "</voice></speak> ";

        using var result = await synthesizer.SpeakSsmlAsync(ssml).ConfigureAwait(false);

        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
        {
            Console.WriteLine($"Speech synthesis succeeded. The audio was saved to {outputFilePath}");
        }
        else if (result.Reason == ResultReason.Canceled)
        {
            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
            Console.WriteLine($"Speech synthesis canceled: Reason={cancellation.Reason}");
            if (cancellation.Reason == CancellationReason.Error)
            {
                Console.WriteLine($"Result id: {result.ResultId}");
                Console.WriteLine($"Error details: {cancellation.ErrorDetails}");
            }
        }
    }
}
