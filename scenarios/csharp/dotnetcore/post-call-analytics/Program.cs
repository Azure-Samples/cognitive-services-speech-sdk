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
        public string[] Locales { get; set; } = new string[] { "en-US" };
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

        internal static async Task<string?> GetTranscriptionAsync(string speechKey, string speechRegion, FileInfo inputAudio)
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
                // System messages represent instructions or other guidance about how the assistant should behave
                new SystemChatMessage($"You are an AI assistant that helps extract information from customer call center transcripts. " +
                $"Summarize the conversation in a couple sentences. \n\"\"\"\n{transcription}\n\"\"\"\n")
            );

            Console.WriteLine($"{openAiEndpoint} : {completion.GetRawResponse().Status}");

            var summary = completion.Value.Content[0].Text;
            return summary;
        }

        public async static Task<int> Main(string[] args)
        {
            var inputAudio = new Option<FileInfo>(
                name: "--inputAudio");

            var speechKey = new Option<string>(
                name: "--speechKey");

            var speechRegion = new Option<string>(
                name: "--speechRegion");

            var openAiKey = new Option<string>(
                name: "--openAiKey");

            var openAiEndpoint = new Option<string>(
                name: "--openAiEndpoint");

            var openAiDeploymentName = new Option<string>(
                name: "--openAiDeploymentName");

            var rootCommand = new RootCommand("PostCallAnalytics Sample");
            rootCommand.AddOption(inputAudio);
            rootCommand.AddOption(speechKey);
            rootCommand.AddOption(speechRegion);
            rootCommand.AddOption(openAiKey);
            rootCommand.AddOption(openAiEndpoint);
            rootCommand.AddOption(openAiDeploymentName);

            rootCommand.SetHandler(async (speechKey, speechRegion, openAiKey, openAiEndpoint, openAiDeploymentName, inputAudio) =>
            {
                var transcription = await GetTranscriptionAsync(speechKey, speechRegion, inputAudio);
                Console.WriteLine($"Transcription: {transcription}");

                var summary = await SummarizeAsync(openAiKey, openAiEndpoint, openAiDeploymentName, transcription);
                Console.WriteLine($"Summary: {summary}");
            }, speechKey, speechRegion, openAiKey, openAiEndpoint, openAiDeploymentName, inputAudio);

            return await rootCommand.InvokeAsync(args);
        }
    }
}
