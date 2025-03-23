//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using SpeechSDKSamples.Remote.Service.GRPC;
using System.CommandLine;
using System.CommandLine.Parsing;

namespace SpeechSDKSamples.Remote.Client.App
{
    /// <summary>
    /// Main program class for the Speech SDK Remote Client application.
    /// </summary>
    /// <remarks>
    /// This application demonstrates streaming audio files to a Speech SDK remote service for speech recognition.
    /// It supports multiple transport protocols and can be configured via command line arguments:
    /// <list type="bullet">
    /// <item><description>--transport: The transport protocol to use. Options are "websocket" or "grpc" (default: grpc)</description></item>
    /// <item><description>--endpoint: The service endpoint URL (default: wss://localhost:5001)</description></item>
    /// <item><description>--file: Path to the WAV file to process</description></item>
    /// </list>
    /// </remarks>
    /// <example>
    /// Example usage:
    /// <code>
    /// SpeechSDKRemoteClientApp.exe --transport grpc --endpoint https://localhost:5001 --file audio.wav
    /// </code>
    /// </example>
    public class Program
    {
        /// <summary>
        /// Application entry point.
        /// </summary>
        /// <param name="args">Command line arguments.</param>
        /// <returns>0 for success, non-zero for failure.</returns>
        public static async Task<int> Main(string[] args)
        {
            var transportOption = new Option<TransportType>(
                name: "--transport",
                getDefaultValue: () => TransportType.GRPC,
                description: "The transport protocol to use. Options are 'websocket' and 'grpc'");

            var endpointOption = new Option<Uri>(
                name: "--endpoint",
                getDefaultValue: () => new Uri("wss://localhost:5001"),
                description: "The service endpoint URL");

            var fileOption = new Option<string>(
                name: "--file",
                description: "Path to the WAV file to process")
            {
                IsRequired = true
            };

            var rootCommand = new RootCommand("Speech SDK Remote Client Application")
            {
                transportOption,
                endpointOption,
                fileOption
            };

            rootCommand.SetHandler(async (transport, endpoint, file) =>
            {
                if (string.IsNullOrEmpty(file))
                {
                    Console.Error.WriteLine("Error: File path is required");
                    Environment.Exit(1);
                }

                if (!File.Exists(file))
                {
                    Console.Error.WriteLine($"Error: File not found: {file}");
                    Environment.Exit(1);
                }

                try
                {
                    // Validate WAV file format before proceeding
                    var audioFormat = WavFileUtilities.ReadWaveHeader(file);
                    Console.WriteLine($"Processing WAV file: {Path.GetFileName(file)}");
                    Console.WriteLine($"Format: {audioFormat.ChannelCount} channels, {audioFormat.SamplesPerSecond} Hz, {audioFormat.BitsPerSample} bits");

                    // Create appropriate transport based on type
                    IAudioStreamTransport audioTransport = transport switch
                    {
                        TransportType.WebSocket => new AudioStreamSource<int>(),
                        TransportType.GRPC => new AudioStreamSource<SpeechSDKRemoteResponse>(),
                        _ => throw new ArgumentException($"Unsupported transport type: {transport}")
                    };

                    using (audioTransport as IDisposable)
                    {
                        Console.WriteLine($"Connecting to service at {endpoint}...");
                        await audioTransport.SendAudio(file, endpoint);
                        Console.WriteLine("Processing complete");
                    }
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine($"Error: {ex.Message}");
                    if (ex.InnerException != null)
                        Console.Error.WriteLine($"Details: {ex.InnerException.Message}");
                    Environment.Exit(1);
                }
            }, transportOption, endpointOption, fileOption);

            return await rootCommand.InvokeAsync(args);
        }
    }

    /// <summary>
    /// Supported transport protocols for communication with the speech service.
    /// </summary>
    internal enum TransportType
    {
        /// <summary>
        /// WebSocket transport using JSON messages
        /// </summary>
        WebSocket,

        /// <summary>
        /// gRPC transport using Protocol Buffers
        /// </summary>
        GRPC
    }
}
