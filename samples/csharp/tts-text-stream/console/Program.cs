//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Demo
{
    using Azure;
    using Azure.AI.OpenAI;
    using Microsoft.CognitiveServices.Speech;
    using Microsoft.CognitiveServices.Speech.Audio;
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using static System.Net.Mime.MediaTypeNames;

    internal class Program
    {
        private static object consoleLock = new();
        private static OpenAIClient? aoaiClient;
        private static SpeechSynthesizer? speechSynthesizer;
        private static MemoryStream audioData;

        public static async Task Main()
        {
            // setup AOAI client
            aoaiClient = new OpenAIClient(
                new Uri(Environment.GetEnvironmentVariable("AZURE_OPENAI_API_ENDPOINT")),
                new AzureKeyCredential(Environment.GetEnvironmentVariable("AZURE_OPENAI_API_KEY")));

            // setup speech synthesizer
            // IMPORTANT: MUST use the websocket v2 endpoint
            var ttsEndpoint = $"wss://{Environment.GetEnvironmentVariable("AZURE_TTS_REGION")}.tts.speech.microsoft.com/cognitiveservices/websocket/v2";
            var speechConfig = SpeechConfig.FromEndpoint(
                new Uri(ttsEndpoint),
                Environment.GetEnvironmentVariable("AZURE_TTS_API_KEY"));

            // set output format
            speechConfig.SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Raw24Khz16BitMonoPcm);

            // set a voice name
            speechConfig.SetProperty(PropertyId.SpeechServiceConnection_SynthVoice, "en-US-BrianMultilingualNeural");

            // set timeout value to bigger ones to avoid sdk cancel the request when GPT latency too high
            speechConfig.SetProperty(PropertyId.SpeechSynthesis_FrameTimeoutInterval, "10000");
            speechConfig.SetProperty(PropertyId.SpeechSynthesis_RtfTimeoutThreshold, "10");

            speechSynthesizer = new SpeechSynthesizer(speechConfig);
            using var request = new SpeechSynthesisRequest(SpeechSynthesisRequestInputType.TextStream);

            // Adjust global pitch, rate, and volume if you need
            // request.SpeakingRate = "50%";
            // request.Pitch = "50%";
            // request.Volume = "25";

            var audioData = new MemoryStream();
            var ttsTask = await speechSynthesizer.StartSpeakingAsync(request);
            var audioTask = Task.Run(() => ReadAudioStream(ttsTask, audioData));
            var gptTask = GenerateGPTResponse(aoaiClient, request);

            await gptTask;
            await audioTask;
            File.WriteAllBytes("streaming.wav", audioData.ToArray());
            audioData.Close();
        }

        private static void ReadAudioStream(SpeechSynthesisResult ttsTask, MemoryStream audioData)
        {
            using var audioDataStream = AudioDataStream.FromResult(ttsTask);
            byte[] buffer = new byte[32000];
            uint totalSize = 0;
            uint totalRead = 0;
            while (true)
            {
                uint readSize = audioDataStream.ReadData(buffer);
                if (readSize == 0)
                {
                    break;
                }

                lock (consoleLock)
                {
                    Console.ForegroundColor = ConsoleColor.Yellow;
                    Console.Write($"[audio]");
                    Console.ResetColor();
                }

                totalRead += readSize;
                totalSize += readSize;
                audioData.Write(buffer, 0, (int)readSize);
            }
        }

        private static async Task GenerateGPTResponse(OpenAIClient aoaiClient, SpeechSynthesisRequest request)
        {
            string query = "tell me a joke in 100 words";

            // Get GPT output stream
            using var gptTokenStream = await aoaiClient.GetChatCompletionsStreamingAsync(
                new ChatCompletionsOptions()
                {
                    Messages =
                    {
                        new ChatRequestSystemMessage(@"You are an AI assistant that helps people find information."),
                        new ChatRequestUserMessage(query)
                    },
                    Temperature = (float)0.7,
                    MaxTokens = 800,


                    NucleusSamplingFactor = (float)0.95,
                    FrequencyPenalty = 0,
                    PresencePenalty = 0,
                    DeploymentName = "gpt-4-turbo"
                });

            await foreach (var message in gptTokenStream.EnumerateValues())
            {
                var text = message.ContentUpdate;
                if (string.IsNullOrEmpty(text))
                {
                    continue;
                }

                lock (consoleLock)
                {
                    Console.ForegroundColor = ConsoleColor.DarkBlue;
                    Console.Write($"{text}");
                    Console.ResetColor();
                }

                // send the gpt token to tts input stream
                request.InputStream.Write(text);
            }

            // close tts input stream when GPT finished
            request.InputStream.Close();
            lock (consoleLock)
            {
                Console.ForegroundColor = ConsoleColor.DarkBlue;
                Console.Write($"[GPT END]");
                Console.ResetColor();
            }
        }
    }
}
