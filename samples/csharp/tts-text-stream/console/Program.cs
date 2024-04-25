//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Demo
{
    using Azure;
    using Azure.AI.OpenAI;
    using Microsoft.CognitiveServices.Speech;
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
            speechConfig.SetProperty(PropertyId.SpeechServiceConnection_SynthVoice, "en-US-AvaMultilingualNeural");

            // set timeout value to bigger ones to avoid sdk cancel the request when GPT latency too high
            speechConfig.SetProperty("SpeechSynthesis_FrameTimeoutInterval", "10000");
            speechConfig.SetProperty("SpeechSynthesis_RtfTimeoutThreshold", "10");

            speechSynthesizer = new SpeechSynthesizer(speechConfig);
            speechSynthesizer.Synthesizing += SpeechSynthesizer_Synthesizing;

            // create request with TextStream input type
            using var request = new SpeechSynthesisRequest(SpeechSynthesisRequestInputType.TextStream);

            var ttsTask = speechSynthesizer.SpeakAsync(request);

            audioData = new MemoryStream();

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

            // wait all tts audio bytes return
            var result = await ttsTask;
            lock (consoleLock)
            {
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.Write($"[TTS END]");
                Console.ResetColor();
            }

            var totalSampleCount = audioData.Length * 8 / 16;
            WriteWavHeader(audioData, false, 1, 16, 24000, (int)totalSampleCount, 0);
            File.WriteAllBytes("streaming.wav", audioData.ToArray());
            audioData.Close();
        }

        private static void SpeechSynthesizer_Synthesizing(object? sender, SpeechSynthesisEventArgs e)
        {
            lock (consoleLock)
            {
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.Write($"[audio]");
                Console.ResetColor();
            }

            audioData.Write(e.Result.AudioData, 0, e.Result.AudioData.Length);
        }

        public static void WriteWavHeader(MemoryStream stream, bool isFloatingPoint, ushort channelCount, ushort bitDepth, int sampleRate, int totalSampleCount, int extraChunkSize)
        {
            stream.Position = 0;

            // RIFF header.
            // Chunk ID.
            stream.Write(System.Text.Encoding.ASCII.GetBytes("RIFF"), 0, 4);

            // Chunk size.
            stream.Write(BitConverter.GetBytes(((bitDepth / 8) * totalSampleCount) + 36 + extraChunkSize), 0, 4);

            // Format.
            stream.Write(System.Text.Encoding.ASCII.GetBytes("WAVE"), 0, 4);

            // Sub-chunk 1.
            // Sub-chunk 1 ID.
            stream.Write(System.Text.Encoding.ASCII.GetBytes("fmt "), 0, 4);

            // Sub-chunk 1 size.
            stream.Write(BitConverter.GetBytes(16), 0, 4);

            // Audio format (floating point (3) or PCM (1)). Any other format indicates compression.
            stream.Write(BitConverter.GetBytes((ushort)(isFloatingPoint ? 3 : 1)), 0, 2);

            // Channels.
            stream.Write(BitConverter.GetBytes(channelCount), 0, 2);

            // Sample rate.
            stream.Write(BitConverter.GetBytes(sampleRate), 0, 4);

            // Bytes rate.
            stream.Write(BitConverter.GetBytes(sampleRate * channelCount * (bitDepth / 8)), 0, 4);

            // Block align.
            stream.Write(BitConverter.GetBytes((ushort)channelCount * (bitDepth / 8)), 0, 2);

            // Bits per sample.
            stream.Write(BitConverter.GetBytes(bitDepth), 0, 2);

            // Sub-chunk 2.
            // Sub-chunk 2 ID.
            stream.Write(System.Text.Encoding.ASCII.GetBytes("data"), 0, 4);

            // Sub-chunk 2 size.
            stream.Write(BitConverter.GetBytes((bitDepth / 8) * totalSampleCount), 0, 4);
        }
    }
}
