// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

using System;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Translation;

internal class Program
{
    private static string RequiredEnvironment(string name)
    {
        var value = Environment.GetEnvironmentVariable(name);
        if (string.IsNullOrEmpty(value))
            throw new InvalidOperationException($"Set {name} before running this sample.");
        return value;
    }

    private static async Task<int> Main()
    {
        try
        {
            await RunAsync();
            return 0;
        }
        catch (Exception error)
        {
            Console.Error.WriteLine($"Live Interpreter failed: {error.Message}");
            return 1;
        }
    }

    private static async Task RunAsync()
    {
        var speechKey = RequiredEnvironment("SPEECH_KEY");
        var endpoint = new Uri(RequiredEnvironment("SPEECH_ENDPOINT"));
        var audioFile = RequiredEnvironment("LIVE_INTERPRETER_AUDIO_FILE");
        if (!File.Exists(audioFile))
            throw new FileNotFoundException("WAV input file not found.", audioFile);

        Console.OutputEncoding = Encoding.UTF8;
        var config = SpeechTranslationConfig.FromEndpoint(endpoint, speechKey);
        config.AddTargetLanguage("fr");
        config.VoiceName = "personal-voice";
        config.SetSpeechSynthesisOutputFormat(
            SpeechSynthesisOutputFormat.Riff16Khz16BitMonoPcm);
        var autoDetectConfig = AutoDetectSourceLanguageConfig.FromOpenRange();
        using var audioConfig = AudioConfig.FromWavFileInput(audioFile);
        using var recognizer = new TranslationRecognizer(
            config, autoDetectConfig, audioConfig);

        var stopped = new TaskCompletionSource<bool>(
            TaskCreationOptions.RunContinuationsAsynchronously);
        var stateLock = new object();
        Exception? failure = null;
        var audioIndex = 0;

        void LogResult(string label, TranslationRecognitionResult result)
        {
            var language = result.Properties.GetProperty(
                PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult);
            Console.WriteLine($"{label} in '{language}': {result.Text}");
            foreach (var translation in result.Translations)
                Console.WriteLine(
                    $"    TRANSLATED into '{translation.Key}': {translation.Value}");
        }

        recognizer.Recognizing += (_, e) => LogResult("RECOGNIZING", e.Result);
        recognizer.Recognized += (_, e) =>
        {
            if (e.Result.Reason == ResultReason.TranslatedSpeech)
                LogResult("RECOGNIZED", e.Result);
            else if (e.Result.Reason == ResultReason.NoMatch)
                Console.WriteLine("NOMATCH: Speech couldn't be recognized.");
        };
        recognizer.Synthesizing += (_, e) =>
        {
            var audio = e.Result.GetAudio();
            if (audio.Length == 0)
                return;
            lock (stateLock)
            {
                var file = $"translation-{++audioIndex}.wav";
                try
                {
                    using var output = new FileStream(
                        file, FileMode.CreateNew, FileAccess.Write);
                    output.Write(audio, 0, audio.Length);
                    Console.WriteLine($"Wrote {file}: {audio.Length} bytes");
                }
                catch (Exception error) when (
                    error is IOException || error is UnauthorizedAccessException)
                {
                    failure ??= error;
                    stopped.TrySetResult(true);
                }
            }
        };
        recognizer.Canceled += (_, e) =>
        {
            Console.WriteLine($"CANCELED: Reason={e.Reason}");
            lock (stateLock)
            {
                if (e.Reason == CancellationReason.Error)
                    failure ??= new InvalidOperationException(
                        $"ErrorCode={e.ErrorCode}, ErrorDetails={e.ErrorDetails}");
            }
            stopped.TrySetResult(true);
        };
        recognizer.SessionStopped += (_, e) => stopped.TrySetResult(true);

        Console.WriteLine("Start translation...");
        await recognizer.StartContinuousRecognitionAsync()
            .WaitAsync(TimeSpan.FromSeconds(30));
        try
        {
            await stopped.Task.WaitAsync(TimeSpan.FromMinutes(5));
        }
        finally
        {
            await recognizer.StopContinuousRecognitionAsync()
                .WaitAsync(TimeSpan.FromSeconds(30));
        }
        lock (stateLock)
        {
            if (failure != null)
                throw new InvalidOperationException(
                    $"Translation failed: {failure.Message}", failure);
        }
    }
}
