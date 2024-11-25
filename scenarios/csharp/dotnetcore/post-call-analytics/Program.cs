using Azure.AI.OpenAI;
using OpenAI.Chat;
using System.ClientModel;
using System.CommandLine;
using System.Net.Http.Json;
using System.Text.Json;

namespace PostCallAnalytics
{
    public sealed class FastTranscriptionOptions
    {
        public string[] Locales { get; set; } = { "en-US" };
    }

    public class Program
    {
        internal static async Task<ByteArrayContent> GetAudioContentAsync(FileInfo inputAudio)
        {
            var audioStream = inputAudio.OpenRead();
            byte[] byteArray;
            using (MemoryStream memoryStream = new MemoryStream())
            {
                await audioStream.CopyToAsync(memoryStream);
                byteArray = memoryStream.ToArray();
            }
            return new ByteArrayContent(byteArray);
        }

        internal static async Task<string?> TranscribeAsync(string speechKey, string speechRegion, FileInfo inputAudio)
        {
            var speechEndpoint = $"https://{speechRegion}.api.cognitive.microsoft.com/speechtotext/transcriptions:transcribe?api-version=2024-05-15-preview";

            var httpClient = new HttpClient();
            httpClient.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", speechKey);

            var multipartFormDataContent = new MultipartFormDataContent();
            var fastTranscriptionOptions = new FastTranscriptionOptions();
            var fastTranscriptionOptionsDefinition = JsonContent.Create(fastTranscriptionOptions);
            multipartFormDataContent.Add(fastTranscriptionOptionsDefinition, "definition");

            var streamContent = await GetAudioContentAsync(inputAudio);
            streamContent.Headers.Add("Content-Type", "multipart/form-data");

            // Speech Fast TranscribeAsync requires a filename to be specified when sending a file.
            multipartFormDataContent.Add(streamContent, "audio", "audio.wav");

            var response = await httpClient.PostAsync(speechEndpoint, multipartFormDataContent, CancellationToken.None);
            Console.WriteLine($"{speechEndpoint} : {response.StatusCode}");

            var content = await response.Content.ReadAsStringAsync();
            var json = JsonDocument.Parse(content).RootElement;

            var combinedTranscript = json.GetProperty("combinedPhrases")[0].GetProperty("text").GetString();
            return combinedTranscript;
        }

        internal static async Task<string> SummarizeAsync(string openAiKey, string openAiEndpoint, string deploymentOrModelName, string transcription)
        {
            var azureClient = new AzureOpenAIClient(new Uri(openAiEndpoint), new ApiKeyCredential(openAiKey));
            var chatClient = azureClient.GetChatClient(deploymentOrModelName);

            var completion = await chatClient.CompleteChatAsync(
                [
                    new SystemChatMessage("You are an AI assistant that helps extract information from customer call center transcripts. Summarize the conversation in a couple sentences."),
                    new UserChatMessage(transcription)
                ]
            );

            Console.WriteLine($"{openAiEndpoint} : {completion.GetRawResponse().Status}");

            var summary = completion.Value.Content[0].Text;
            return summary;
        }

        internal static async Task AnalyzeAudioAsync(string speechKey, string speechRegion, FileInfo inputAudio, string openAiKey, string openAiEndpoint, string deploymentOrModelName)
        {
            var transcription = await TranscribeAsync(speechKey, speechRegion, inputAudio);
            Console.WriteLine($"Transcription: {transcription}");

            if (!string.IsNullOrEmpty(openAiKey) && !string.IsNullOrEmpty(openAiEndpoint) && !string.IsNullOrEmpty(deploymentOrModelName))
            {
                var summary = await SummarizeAsync(openAiKey, openAiEndpoint, deploymentOrModelName, transcription);
                Console.WriteLine($"Summary: {summary}");
            }
            else
            {
                Console.WriteLine($"Missing AOAI configuration. Skipping Summarization");
            }
        }

        public async static Task<int> Main(string[] args)
        {
            var inputAudio = new Argument<FileInfo>(name: "inputAudio", description: "Path to the audio file. Required.");

            // Speech service is used for transcription.
            var speechKey = new Option<string>(name: "--speechKey", description: "Your Cognitive Services or Speech resource key. Required.", getDefaultValue: () => Environment.GetEnvironmentVariable("SPEECH_KEY"));
            speechKey.AddValidator(result =>
            {
                if (string.IsNullOrEmpty(result.GetValueForOption(speechKey)))
                {
                    result.ErrorMessage = $"Speech key is required. Set via --{speechKey.Name} or SPEECH_KEY environment variable.";
                }
            });
            var speechRegion = new Option<string>(name: "--speechRegion", description: "Your Cognitive Services or Speech resource region. Example: eastus, northeurope. Required.", getDefaultValue: () => Environment.GetEnvironmentVariable("SPEECH_REGION"));
            speechRegion.AddValidator(result =>
            {
                if (string.IsNullOrEmpty(result.GetValueForOption(speechRegion)))
                {
                    result.ErrorMessage = $"Speech region is required. Set via --{speechRegion.Name} or SPEECH_REGION environment variable.";
                }
            });

            // AOAI is used for summarization. This step is optional.
            var openAiKey = new Option<string>(name: "--openAiKey", description: "Your Azure OpenAI resource key. Optional.", getDefaultValue: () => Environment.GetEnvironmentVariable("AOAI_KEY"));
            var openAiEndpoint = new Option<string>(name: "--openAiEndpoint", description: "Your Azure OpenAI resource endpoint. Optional. Example: https://YourResourceName.openai.azure.com", getDefaultValue: () => Environment.GetEnvironmentVariable("AOAI_ENDPOINT"));
            var openAiDeploymentName = new Option<string>(name: "--openAiDeploymentName", description: "Your Azure OpenAI deployment name. Example: my-gpt-4o-mini. Optional.", getDefaultValue: () => Environment.GetEnvironmentVariable("AOAI_DEPLOYMENT_NAME"));

            var rootCommand = new RootCommand()
            {
                inputAudio,
                speechKey,
                speechRegion,
                openAiKey,
                openAiEndpoint,
                openAiDeploymentName
            };

            rootCommand.SetHandler(AnalyzeAudioAsync, speechKey, speechRegion, inputAudio, openAiKey, openAiEndpoint, openAiDeploymentName);

            return await rootCommand.InvokeAsync(args);
        }
    }
}
