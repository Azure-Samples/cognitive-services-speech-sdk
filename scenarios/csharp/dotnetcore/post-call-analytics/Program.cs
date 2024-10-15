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

            var prompt = $"You are an AI assistant that helps extract information from customer call center transcripts. " +
                $"Summarize the conversation in a couple sentences. \n\"\"\"\n{transcription}\n\"\"\"\n";

            var completion = await chatClient.CompleteChatAsync(
                new SystemChatMessage(prompt)
            );

            Console.WriteLine($"{openAiEndpoint} : {completion.GetRawResponse().Status}");

            var summary = completion.Value.Content[0].Text;
            return summary;
        }

        internal static async Task AnalyzeAudioAsync(string speechKey, string speechRegion, FileInfo? inputAudio, string openAiKey, string openAiEndpoint, string deploymentOrModelName)
        {
            if (string.IsNullOrEmpty(speechKey) || string.IsNullOrEmpty(speechRegion) || inputAudio == null || string.IsNullOrEmpty(openAiKey) || string.IsNullOrEmpty(openAiEndpoint) || string.IsNullOrEmpty(deploymentOrModelName))
            {
                Console.WriteLine("Error: missing required option");
                return;
            }

            var transcription = await TranscribeAsync(speechKey, speechRegion, inputAudio);
            Console.WriteLine($"Transcription: {transcription}");

            var summary = await SummarizeAsync(openAiKey, openAiEndpoint, deploymentOrModelName, transcription);
            Console.WriteLine($"Summary: {summary}");
        }

        public async static Task<int> Main(string[] args)
        {
            var inputAudio = new Option<FileInfo>(name: "--inputAudio", description: "Path to the audio file. Required.");
            var speechKey = new Option<string>(name: "--speechKey", description: "Your Cognitive Services or Speech resource key. Required.");
            var speechRegion = new Option<string>(name: "--speechRegion", description: "Your Cognitive Services or Speech resource region. Example: eastus, northeurope. Required.");
            var openAiKey = new Option<string>(name: "--openAiKey", description: "Your Azure OpenAI resource key. Required.");
            var openAiEndpoint = new Option<string>(name: "--openAiEndpoint", description: "Your Azure OpenAI resource endpoint. Required. Example: https://YourResourceName.openai.azure.com");
            var openAiDeploymentName = new Option<string>(name: "--openAiDeploymentName", description: "Your Azure OpenAI deployment name. Example: my-gpt-4o-mini. Required.");

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
