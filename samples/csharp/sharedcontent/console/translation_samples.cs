//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using System;
using System.ClientModel;
#if NET462
using System.IO;
using System.Media;
#endif
using System.Threading.Tasks;
using Azure.Core;
using Azure.Identity;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Translation;
// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class TranslationSamples
    {
        // Translation from microphone.
        public static async Task TranslationWithMicrophoneAsync()
        {
            // <TranslationWithMicrophoneAsync>
            // Translation source language.
            // Replace with a language of your choice.
            string fromLanguage = "en-US";

            // Voice name of synthesis output.
            const string germanVoice = "de-DE-AmalaNeural";

            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechTranslationConfig.FromEndpoint(new Uri("https://ServiceRegron.cognitiveservices.azure.com"), "YourSubscriptionKey");

            config.SpeechRecognitionLanguage = fromLanguage;
            config.VoiceName = germanVoice;

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");

            // Creates a translation recognizer using microphone as audio input.
            using (var recognizer = new TranslationRecognizer(config))
            {
                // Subscribes to events.
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine($"RECOGNIZING in '{fromLanguage}': Text={e.Result.Text}");
                    foreach (var element in e.Result.Translations)
                    {
                        Console.WriteLine($"    TRANSLATING into '{element.Key}': {element.Value}");
                    }
                };

                recognizer.Recognized += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.TranslatedSpeech)
                    {
                        Console.WriteLine($"RECOGNIZED in '{fromLanguage}': Text={e.Result.Text}");
                        foreach (var element in e.Result.Translations)
                        {
                            Console.WriteLine($"    TRANSLATED into '{element.Key}': {element.Value}");
                        }
                    }
                    else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                        Console.WriteLine($"    Speech not translated.");
                    }
                    else if (e.Result.Reason == ResultReason.NoMatch)
                    {
                        Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                    }
                };

                recognizer.Synthesizing += (s, e) =>
                {
                    var audio = e.Result.GetAudio();
                    Console.WriteLine(audio.Length != 0
                        ? $"AudioSize: {audio.Length}"
                        : $"AudioSize: {audio.Length} (end of synthesis data)");

                    if (audio.Length > 0)
                    {
#if NET462
                        using (var m = new MemoryStream(audio))
                        {
                            SoundPlayer simpleSound = new SoundPlayer(m);
                            simpleSound.PlaySync();
                        }
#endif
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine($"CANCELED: Reason={e.Reason}");

                    if (e.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                        Console.WriteLine($"CANCELED: Did you update the subscription info?");
                    }
                };

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("\nSession started event.");
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine("\nSession stopped event.");
                };

                // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                Console.WriteLine("Say something...");
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                do
                {
                    Console.WriteLine("Press Enter to stop");
                } while (Console.ReadKey().Key != ConsoleKey.Enter);

                // Stops continuous recognition.
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
            // </TranslationWithMicrophoneAsync>
        }

        // Translation with inline commit using a push audio stream.
        public static async Task TranslationWithInlineCommitAsync()
        {
            // <TranslationWithInlineCommitAsync>
            var endpoint = Environment.GetEnvironmentVariable("SPEECH_ENDPOINT");
            var subscriptionKey = Environment.GetEnvironmentVariable("SPEECH_RESOURCE_KEY");
            if (string.IsNullOrWhiteSpace(endpoint) || string.IsNullOrWhiteSpace(subscriptionKey))
            {
                throw new InvalidOperationException("Set SPEECH_ENDPOINT and SPEECH_RESOURCE_KEY before running this sample.");
            }

            var config = SpeechTranslationConfig.FromEndpoint(new Uri(endpoint), subscriptionKey);
            config.SpeechRecognitionLanguage = "en-US";
            config.AddTargetLanguage("de");
            config.AddTargetLanguage("fr");
            // Enable inline commit on a service deployment that supports the feature.
            config.SetServiceProperty("setfeature", "forcecommit", ServicePropertyChannel.UriQueryParameter);

            // The input must be 16 kHz, 16-bit, mono PCM audio.
            using (var reader = Helper.CreateWavReader(@"whatstheweatherlike.wav"))
            using (var pushStream = AudioInputStream.CreatePushStream())
            using (var audioInput = AudioConfig.FromStreamInput(pushStream))
            using (var recognizer = new TranslationRecognizer(config, audioInput))
            {
                var commitAcknowledged = new TaskCompletionSource<uint>(TaskCreationOptions.RunContinuationsAsynchronously);
                var recognitionStopped = new TaskCompletionSource<string>(TaskCreationOptions.RunContinuationsAsynchronously);

                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine($"RECOGNIZING: Text={e.Result.Text}");
                };
                recognizer.Recognized += (s, e) =>
                {
                    var token = e.Result.CommitToken;
                    var tokenText = token != 0 ? $" commit_token={token}" : "";
                    Console.WriteLine($"RECOGNIZED: Reason={e.Result.Reason} Text={e.Result.Text}{tokenText}");
                    foreach (var translation in e.Result.Translations)
                    {
                        Console.WriteLine($"  {translation.Key}: {translation.Value}");
                    }
                    // NoMatch can acknowledge a commit; naturally segmented results have token 0.
                    if (token != 0)
                    {
                        // Keep the token even if this event arrives before Commit() returns.
                        commitAcknowledged.TrySetResult(token);
                    }
                };
                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine($"CANCELED: Reason={e.Reason} Details={e.ErrorDetails}");
                    recognitionStopped.TrySetResult(e.Reason == CancellationReason.Error
                        ? $"Recognition canceled: {e.ErrorCode}. {e.ErrorDetails}" : null);
                };
                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"SESSION STOPPED: SessionId={e.SessionId}");
                    recognitionStopped.TrySetResult(null);
                };

                const int commitAtMilliseconds = 590;
                const int bytesPerSecond = 16000 * 2;
                var buffer = new byte[3200];
                bool streamClosed = false;
                try
                {
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                    for (int segment = 0; segment < 2; ++segment)
                    {
                        Console.WriteLine($"Writing audio segment {segment + 1}");
                        int bytesRemaining = bytesPerSecond * commitAtMilliseconds / 1000;
                        while (segment != 0 || bytesRemaining > 0)
                        {
                            if (recognitionStopped.Task.IsCompleted)
                            {
                                throw new InvalidOperationException("Recognition ended before all audio was written. "
                                    + await recognitionStopped.Task.ConfigureAwait(false));
                            }
                            int bytesToRead = segment == 0 ? Math.Min(buffer.Length, bytesRemaining) : buffer.Length;
                            int bytesRead = reader.Read(buffer, (uint)bytesToRead);
                            if (bytesRead == 0)
                            {
                                if (segment == 0)
                                {
                                    throw new InvalidOperationException("The WAV file ended before the commit boundary.");
                                }
                                break;
                            }
                            pushStream.Write(buffer, bytesRead);
                            if (segment == 0)
                            {
                                bytesRemaining -= bytesRead;
                            }
                            await Task.Delay(TimeSpan.FromSeconds((double)bytesRead / bytesPerSecond)).ConfigureAwait(false);
                        }

                        // Closing the stream later finalizes the remaining audio without another commit.
                        if (segment != 0)
                        {
                            continue;
                        }
                        // Commit the audio written so far without closing the stream or stopping recognition.
                        uint token = pushStream.Commit();
                        Console.WriteLine($"COMMIT: token={token}");
                        if (token == 0)
                        {
                            throw new InvalidOperationException("The SDK rejected the commit request.");
                        }
                        // A returned token is a local request; wait for a matching service acknowledgment.
                        await Task.WhenAny(commitAcknowledged.Task, recognitionStopped.Task,
                            Task.Delay(TimeSpan.FromSeconds(15))).ConfigureAwait(false);
                        if (!commitAcknowledged.Task.IsCompleted || await commitAcknowledged.Task.ConfigureAwait(false) != token)
                        {
                            var error = recognitionStopped.Task.IsCompleted
                                ? await recognitionStopped.Task.ConfigureAwait(false) : "";
                            throw new InvalidOperationException("No acknowledgment before timeout or session end. Check inline commit support. " + error);
                        }
                        Console.WriteLine($"COMMIT ACKNOWLEDGED: token={token}");
                    }

                    pushStream.Close();
                    streamClosed = true;
                    if (await Task.WhenAny(recognitionStopped.Task, Task.Delay(TimeSpan.FromSeconds(15))).ConfigureAwait(false)
                        != recognitionStopped.Task)
                    {
                        throw new TimeoutException("Timed out waiting for recognition to finish after the end of audio.");
                    }
                    var cancellationError = await recognitionStopped.Task.ConfigureAwait(false);
                    if (cancellationError != null)
                    {
                        throw new InvalidOperationException(cancellationError);
                    }
                }
                finally
                {
                    if (!streamClosed)
                    {
                        pushStream.Close();
                    }
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
            // </TranslationWithInlineCommitAsync>
        }

        // Translation using file input.
        public static async Task TranslationWithFileAsync()
        {
            // <TranslationWithFileAsync>
            // Translation source language.
            // Replace with a language of your choice.
            string fromLanguage = "en-US";

            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechTranslationConfig.FromEndpoint(new Uri("https://ServiceRegron.cognitiveservices.azure.com"), "YourSubscriptionKey");
            config.SpeechRecognitionLanguage = fromLanguage;

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");
            config.AddTargetLanguage("fr");

            var stopTranslation = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Creates a translation recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var audioInput = AudioConfig.FromWavFileInput(@"whatstheweatherlike.wav"))
            {
                using (var recognizer = new TranslationRecognizer(config, audioInput))
                {
                    // Subscribes to events.
                    recognizer.Recognizing += (s, e) =>
                    {
                        Console.WriteLine($"RECOGNIZING in '{fromLanguage}': Text={e.Result.Text}");
                        foreach (var element in e.Result.Translations)
                        {
                            Console.WriteLine($"    TRANSLATING into '{element.Key}': {element.Value}");
                        }
                    };

                    recognizer.Recognized += (s, e) => {
                        if (e.Result.Reason == ResultReason.TranslatedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED in '{fromLanguage}': Text={e.Result.Text}");
                            foreach (var element in e.Result.Translations)
                            {
                                Console.WriteLine($"    TRANSLATED into '{element.Key}': {element.Value}");
                            }
                        }
                        else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                            Console.WriteLine($"    Speech not translated.");
                        }
                        else if (e.Result.Reason == ResultReason.NoMatch)
                        {
                            Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                        }
                    };

                    recognizer.Canceled += (s, e) =>
                    {
                        Console.WriteLine($"CANCELED: Reason={e.Reason}");

                        if (e.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }

                        stopTranslation.TrySetResult(0);
                    };

                    recognizer.SpeechStartDetected += (s, e) => {
                        Console.WriteLine("\nSpeech start detected event.");
                    };

                    recognizer.SpeechEndDetected += (s, e) => {
                        Console.WriteLine("\nSpeech end detected event.");
                    };

                    recognizer.SessionStarted += (s, e) => {
                        Console.WriteLine("\nSession started event.");
                    };

                    recognizer.SessionStopped += (s, e) => {
                        Console.WriteLine("\nSession stopped event.");
                        Console.WriteLine($"\nStop translation.");
                        stopTranslation.TrySetResult(0);
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    Console.WriteLine("Start translation...");
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopTranslation.Task });

                    // Stops translation.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
            // </TranslationWithFileAsync>
        }

        // Translation using compressed file input.
        public static async Task TranslationWithFileCompressedInputAsync()
        {
            // <TranslationWithFileCompressedInputAsync>
            // Translation source language with compressed format.
            // Replace with a language of your choice.
            string fromLanguage = "en-US";

            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechTranslationConfig.FromEndpoint(new Uri("YourServiceEndpoint"), "YourSubscriptionKey");
            config.SpeechRecognitionLanguage = fromLanguage;

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");
            config.AddTargetLanguage("fr");

            var stopTranslation = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Creates a translation recognizer using file as audio input.
            using (var pushStream = AudioInputStream.CreatePushStream(AudioStreamFormat.GetCompressedFormat(AudioStreamContainerFormat.MP3)))
            {
                using (var audioInput = AudioConfig.FromStreamInput(pushStream))
                {
                    using (var recognizer = new TranslationRecognizer(config, audioInput))
                    {
                        // Subscribes to events.
                        recognizer.Recognizing += (s, e) =>
                        {
                            Console.WriteLine($"RECOGNIZING in '{fromLanguage}': Text={e.Result.Text}");
                            foreach (var element in e.Result.Translations)
                            {
                                Console.WriteLine($"    TRANSLATING into '{element.Key}': {element.Value}");
                            }
                        };

                        recognizer.Recognized += (s, e) =>
                        {
                            if (e.Result.Reason == ResultReason.TranslatedSpeech)
                            {
                                Console.WriteLine($"RECOGNIZED in '{fromLanguage}': Text={e.Result.Text}");
                                foreach (var element in e.Result.Translations)
                                {
                                    Console.WriteLine($"    TRANSLATED into '{element.Key}': {element.Value}");
                                }
                            }
                            else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                            {
                                Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                                Console.WriteLine($"    Speech not translated.");
                            }
                            else if (e.Result.Reason == ResultReason.NoMatch)
                            {
                                Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                            }
                        };

                        recognizer.Canceled += (s, e) =>
                        {
                            Console.WriteLine($"CANCELED: Reason={e.Reason}");

                            if (e.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }

                            stopTranslation.TrySetResult(0);
                        };

                        recognizer.SpeechStartDetected += (s, e) =>
                        {
                            Console.WriteLine("\nSpeech start detected event.");
                        };

                        recognizer.SpeechEndDetected += (s, e) =>
                        {
                            Console.WriteLine("\nSpeech end detected event.");
                        };

                        recognizer.SessionStarted += (s, e) =>
                        {
                            Console.WriteLine("\nSession started event.");
                        };

                        recognizer.SessionStopped += (s, e) =>
                        {
                            Console.WriteLine("\nSession stopped event.");
                            Console.WriteLine($"\nStop translation.");
                            stopTranslation.TrySetResult(0);
                        };

                        // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                        Console.WriteLine("Start translation...");
                        await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                        // Replace with your own audio file name.
                        using (BinaryAudioStreamReader reader = Helper.CreateBinaryFileReader(@"whatstheweatherlike.mp3"))
                        {
                            byte[] buffer = new byte[1000];
                            while (true)
                            {
                                var readSamples = reader.Read(buffer, (uint)buffer.Length);
                                if (readSamples == 0)
                                {
                                    break;
                                }
                                pushStream.Write(buffer, readSamples);
                            }
                        }
                        pushStream.Close();
                        // Waits for completion.
                        // Use Task.WaitAny to keep the task rooted.
                        Task.WaitAny(new[] { stopTranslation.Task });

                        // Stops translation.
                        await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                    }
                }
            }
            // </TranslationWithFileCompressedInputAsync>
        }

        // Translation using audio stream.
        public static async Task TranslationWithAudioStreamAsync()
        {
            // Translation source language.
            // Replace with a language of your choice.
            string fromLanguage = "en-US";

            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechTranslationConfig.FromEndpoint(new Uri("YourServiceEndpoint"), "YourSubscriptionKey");
            config.SpeechRecognitionLanguage = fromLanguage;

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");
            config.AddTargetLanguage("fr");

            var stopTranslation = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Create an audio stream from a wav file.
            // Replace with your own audio file name.
            using (var audioInput = Helper.OpenWavFile(@"whatstheweatherlike.wav"))
            {
                // Creates a translation recognizer using audio stream as input.
                using (var recognizer = new TranslationRecognizer(config, audioInput))
                {
                    // Subscribes to events.
                    recognizer.Recognizing += (s, e) =>
                    {
                        Console.WriteLine($"RECOGNIZING in '{fromLanguage}': Text={e.Result.Text}");
                        foreach (var element in e.Result.Translations)
                        {
                            Console.WriteLine($"    TRANSLATING into '{element.Key}': {element.Value}");
                        }
                    };

                    recognizer.Recognized += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.TranslatedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED in '{fromLanguage}': Text={e.Result.Text}");
                            foreach (var element in e.Result.Translations)
                            {
                                Console.WriteLine($"    TRANSLATED into '{element.Key}': {element.Value}");
                            }
                        }
                        else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                            Console.WriteLine($"    Speech not translated.");
                        }
                        else if (e.Result.Reason == ResultReason.NoMatch)
                        {
                            Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                        }
                    };

                    recognizer.Canceled += (s, e) =>
                    {
                        Console.WriteLine($"CANCELED: Reason={e.Reason}");

                        if (e.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }

                        stopTranslation.TrySetResult(0);
                    };

                    recognizer.SpeechStartDetected += (s, e) =>
                    {
                        Console.WriteLine("\nSpeech start detected event.");
                    };

                    recognizer.SpeechEndDetected += (s, e) =>
                    {
                        Console.WriteLine("\nSpeech end detected event.");
                    };

                    recognizer.SessionStarted += (s, e) =>
                    {
                        Console.WriteLine("\nSession started event.");
                    };

                    recognizer.SessionStopped += (s, e) =>
                    {
                        Console.WriteLine($"\nSession stopped event.");
                        Console.WriteLine($"\nStop translation.");
                        stopTranslation.TrySetResult(0);
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    Console.WriteLine("Start translation...");
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopTranslation.Task });

                    // Stops translation.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
        }

        #region Language Detection enabled

        // Translation from microphone.
        public static async Task TranslationWithMicrophoneAsync_withLanguageDetectionEnabled()
        {
            // When you use Language ID with speech translation, you must set a v2 endpoint and use
            // SpeechTranslationConfig.FromEndpoint() to create the SpeechTranslationConfig object.
            // This will be fixed in a future version of Speech SDK.

            // Please replace the service region with your region (e.g. "westus").
            var v2EndpointInString = String.Format("wss://{0}.stt.speech.microsoft.com/speech/universal/v2", "YourServiceRegion");
            var v2EndpointUrl = new Uri(v2EndpointInString);

            // Creates an instance of a speech translation config.
            // Please replace the service subscription key with your subscription key.
            var config = SpeechTranslationConfig.FromEndpoint(v2EndpointUrl, "YourSubscriptionKey");

            // Translation target language(s).
            // Replace with language(s) of your choice. You can add more lines.
            config.AddTargetLanguage("de");

            // Optional: If in addition to the translated text you also need the synthesized audio, then provide the voice name
            var germanVoice = "de-DE-AmalaNeural";
            config.VoiceName = germanVoice;

            // Set the mode of input language detection to either "AtStart" (the default) or "Continuous".
            // Please refer to the documentation of Language ID for more information.
            // https://aka.ms/speech/lid?pivots=programming-language-csharp
            config.SetProperty(PropertyId.SpeechServiceConnection_LanguageIdMode, "Continuous");

            // Define the set of input spoken languages to detect
            var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(new string[] { "en-US", "es-MX" });

            // Creates a translation recognizer using microphone as audio input.
            using (var recognizer = new TranslationRecognizer(config, autoDetectSourceLanguageConfig))
            {
                // Subscribes to events.
                recognizer.Recognizing += (s, e) =>
                {
                    var lidResult = e.Result.Properties.GetProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult);

                    Console.WriteLine($"RECOGNIZING in '{lidResult}': Text={e.Result.Text}");
                    foreach (var element in e.Result.Translations)
                    {
                        Console.WriteLine($" TRANSLATING into '{element.Key}': {element.Value}");
                    }
                };

                recognizer.Recognized += (s, e) =>
                {
                    var lidResult = e.Result.Properties.GetProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult);

                    if (e.Result.Reason == ResultReason.TranslatedSpeech)
                    {
                        Console.WriteLine($"RECOGNIZED in '{lidResult}': Text={e.Result.Text}");
                        foreach (var element in e.Result.Translations)
                        {
                            Console.WriteLine($"    TRANSLATED into '{element.Key}': {element.Value}");
                        }
                    }
                    else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        Console.WriteLine($"RECOGNIZED in '{lidResult}': Text={e.Result.Text}");
                        Console.WriteLine($"    Speech not translated.");
                    }
                    else if (e.Result.Reason == ResultReason.NoMatch)
                    {
                        Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                    }
                };

                recognizer.Synthesizing += (s, e) =>
                {
                    var audio = e.Result.GetAudio();
                    Console.WriteLine(audio.Length != 0
                        ? $"AudioSize: {audio.Length}"
                        : $"AudioSize: {audio.Length} (end of synthesis data)");

                    if (audio.Length > 0)
                    {
#if NET462
                        using (var m = new MemoryStream(audio))
                        {
                            SoundPlayer simpleSound = new SoundPlayer(m);
                            simpleSound.PlaySync();
                        }
#endif
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine($"CANCELED: Reason={e.Reason}");

                    if (e.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                        Console.WriteLine($"CANCELED: Did you update the subscription info?");
                    }
                };

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("\nSession started event.");
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine("\nSession stopped event.");
                };

                // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                Console.WriteLine("Say something...");
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                do
                {
                    Console.WriteLine("Press Enter to stop");
                } while (Console.ReadKey().Key != ConsoleKey.Enter);

                // Stops continuous recognition.
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
        }

        // Translation using multi-lingual file input.
        public static async Task TranslationWithMultiLingualFileAsync_withLanguageDetectionEnabled()
        {
            // When you use Language ID with speech translation, you must set a v2 endpoint and use
            // SpeechTranslationConfig.FromEndpoint() to create the SpeechTranslationConfig object.
            // This will be fixed in a future version of Speech SDK.

            // Please replace the service region with your region
            var v2EndpointInString = String.Format("wss://{0}.stt.speech.microsoft.com/speech/universal/v2", "YourServiceRegion");
            var v2EndpointUrl = new Uri(v2EndpointInString);

            // Creates an instance of a speech translation config with specified subscription key and service region.
            // Please replace the service subscription key with your subscription key
            var config = SpeechTranslationConfig.FromEndpoint(v2EndpointUrl, "YourSubscriptionKey");

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");
            config.AddTargetLanguage("fr");

            // Set the mode of input language detection to either "AtStart" (the default) or "Continuous".
            // Please refer to the documentation of Language ID for more information.
            // https://aka.ms/speech/lid?pivots=programming-language-csharp
            config.SetProperty(PropertyId.SpeechServiceConnection_LanguageIdMode, "Continuous");

            // Define the set of languages to detect
            var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(new string[] { "en-US", "zh-CN" });

            var stopTranslation = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Creates a translation recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var audioInput = AudioConfig.FromWavFileInput(@"en-us_zh-cn.wav"))
            {
                using (var recognizer = new TranslationRecognizer(config, autoDetectSourceLanguageConfig, audioInput))
                {
                    // Subscribes to events.
                    recognizer.Recognizing += (s, e) =>
                    {
                        var lidResult = e.Result.Properties.GetProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult);

                        Console.WriteLine($"RECOGNIZING in '{lidResult}': Text={e.Result.Text}");
                        foreach (var element in e.Result.Translations)
                        {
                            Console.WriteLine($"    TRANSLATING into '{element.Key}': {element.Value}");
                        }
                    };

                    recognizer.Recognized += (s, e) => {
                        if (e.Result.Reason == ResultReason.TranslatedSpeech)
                        {
                            var lidResult = e.Result.Properties.GetProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult);

                            Console.WriteLine($"RECOGNIZED in '{lidResult}': Text={e.Result.Text}");
                            foreach (var element in e.Result.Translations)
                            {
                                Console.WriteLine($"    TRANSLATED into '{element.Key}': {element.Value}");
                            }
                        }
                        else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                            Console.WriteLine($"    Speech not translated.");
                        }
                        else if (e.Result.Reason == ResultReason.NoMatch)
                        {
                            Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                        }
                    };

                    recognizer.Canceled += (s, e) =>
                    {
                        Console.WriteLine($"CANCELED: Reason={e.Reason}");

                        if (e.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }

                        stopTranslation.TrySetResult(0);
                    };

                    recognizer.SpeechStartDetected += (s, e) => {
                        Console.WriteLine("\nSpeech start detected event.");
                    };

                    recognizer.SpeechEndDetected += (s, e) => {
                        Console.WriteLine("\nSpeech end detected event.");
                    };

                    recognizer.SessionStarted += (s, e) => {
                        Console.WriteLine("\nSession started event.");
                    };

                    recognizer.SessionStopped += (s, e) => {
                        Console.WriteLine("\nSession stopped event.");
                        Console.WriteLine($"\nStop translation.");
                        stopTranslation.TrySetResult(0);
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    Console.WriteLine("Start translation...");
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopTranslation.Task });

                    // Stops translation.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
        }

        #endregion

        #region Multilingual Translation with language identification
        public static async Task MultilingualTranslationWithLanguageIdentificationAsync()
        {
            // When you use Multilingual Translation with language identification, 
            // you don't need to define any candidate languages to detect, but you must set a v2 endpoint and use
            // SpeechTranslationConfig.FromEndpoint() to create the SpeechTranslationConfig object.
            // This will be fixed in a future version of Speech SDK.

            // Please replace the service region with your region
            var v2EndpointInString = String.Format("wss://{0}.stt.speech.microsoft.com/speech/universal/v2", "YourRegion");
            var v2EndpointUrl = new Uri(v2EndpointInString);

            // Creates an instance of a speech translation config with specified subscription key and service region.
            // Please replace the service subscription key with your subscription key
            var config = SpeechTranslationConfig.FromEndpoint(v2EndpointUrl, "YourSubscriptionKey");

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");
            config.AddTargetLanguage("fr");

            // You don't need to define any candidate languages to detect.
            var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromOpenRange();

            var stopTranslation = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Creates a translation recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var audioInput = AudioConfig.FromWavFileInput(@"YourAudioFile.wav"))
            {
                using (var recognizer = new TranslationRecognizer(config, autoDetectSourceLanguageConfig, audioInput))
                {
                    // Subscribes to events.
                    recognizer.Recognizing += (s, e) =>
                    {
                        var lidResult = e.Result.Properties.GetProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult);

                        Console.WriteLine($"RECOGNIZING in '{lidResult}': Text={e.Result.Text}");
                        if (e.Result.Reason == ResultReason.TranslatingSpeech)
                        {
                            foreach (var element in e.Result.Translations)
                            {
                                Console.WriteLine($"    TRANSLATING into '{element.Key}': {element.Value}");
                            }

                        }
                    };

                    recognizer.Recognized += (s, e) => {
                        if (e.Result.Reason == ResultReason.TranslatedSpeech)
                        {
                            var lidResult = e.Result.Properties.GetProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult);

                            Console.WriteLine($"RECOGNIZED in '{lidResult}': Text={e.Result.Text}");
                            foreach (var element in e.Result.Translations)
                            {
                                Console.WriteLine($"    TRANSLATED into '{element.Key}': {element.Value}");
                            }
                        }
                        else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                            Console.WriteLine($"    Speech not translated.");
                        }
                        else if (e.Result.Reason == ResultReason.NoMatch)
                        {
                            Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                        }
                    };

                    recognizer.Canceled += (s, e) =>
                    {
                        Console.WriteLine($"CANCELED: Reason={e.Reason}");

                        if (e.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }

                        stopTranslation.TrySetResult(0);
                    };

                    recognizer.SpeechStartDetected += (s, e) => {
                        Console.WriteLine("\nSpeech start detected event.");
                    };

                    recognizer.SpeechEndDetected += (s, e) => {
                        Console.WriteLine("\nSpeech end detected event.");
                    };

                    recognizer.SessionStarted += (s, e) => {
                        Console.WriteLine("\nSession started event.");
                    };

                    recognizer.SessionStopped += (s, e) => {
                        Console.WriteLine("\nSession stopped event.");
                        Console.WriteLine($"\nStop translation.");
                        stopTranslation.TrySetResult(0);
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    Console.WriteLine("Start translation...");
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopTranslation.Task });

                    // Stops translation.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
        }

        #endregion

        // Translation authenticated via AAD token crendential.
        public static async Task TranslationWithAADTokenCredential()
        {
            // Translation source language.
            // Replace with a language of your choice.
            string fromLanguage = "en-US";

            // Create a token credential using DefaultAzureCredential.
            // This credential supports multiple authentication methods, including Managed Identity, environment variables, and Azure CLI login.
            // For more types of token credentials, refer to: 
            // https://learn.microsoft.com/dotnet/api/azure.identity.defaultazurecredential?view=azure-dotnet
            TokenCredential credential = new DefaultAzureCredential(new DefaultAzureCredentialOptions()
            {
                ManagedIdentityClientId = "{Your App Id}", // Specify the Managed Identity Client ID if using a user-assigned managed identity.
            });

            // Define the v2 endpoint for Azure Speech Service.
            // This is required when using a private endpoint with a custom domain.
            // For details on setting up a custom domain with private links, see: 
            // https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link?tabs=portal#create-a-custom-domain-name
            var v2Endpoint = string.Format("wss://{custom domain}/stt/speech/universal/v2");

            // Create a SpeechTranslationConfig instance using the v2 endpoint and the token credential for authentication.
            var config = SpeechTranslationConfig.FromEndpoint(new Uri(v2Endpoint), credential);
            config.SpeechRecognitionLanguage = fromLanguage;

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");
            config.AddTargetLanguage("fr");

            var stopTranslation = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Create an audio stream from a wav file.
            // Replace with your own audio file name.
            using (var audioInput = Helper.OpenWavFile(@"whatstheweatherlike.wav"))
            {
                // Creates a translation recognizer using audio stream as input.
                using (var recognizer = new TranslationRecognizer(config, audioInput))
                {
                    // Subscribes to events.
                    recognizer.Recognizing += (s, e) =>
                    {
                        Console.WriteLine($"RECOGNIZING in '{fromLanguage}': Text={e.Result.Text}");
                        foreach (var element in e.Result.Translations)
                        {
                            Console.WriteLine($"    TRANSLATING into '{element.Key}': {element.Value}");
                        }
                    };

                    recognizer.Recognized += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.TranslatedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED in '{fromLanguage}': Text={e.Result.Text}");
                            foreach (var element in e.Result.Translations)
                            {
                                Console.WriteLine($"    TRANSLATED into '{element.Key}': {element.Value}");
                            }
                        }
                        else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                            Console.WriteLine($"    Speech not translated.");
                        }
                        else if (e.Result.Reason == ResultReason.NoMatch)
                        {
                            Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                        }
                    };

                    recognizer.Canceled += (s, e) =>
                    {
                        Console.WriteLine($"CANCELED: Reason={e.Reason}");

                        if (e.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }

                        stopTranslation.TrySetResult(0);
                    };

                    recognizer.SpeechStartDetected += (s, e) =>
                    {
                        Console.WriteLine("\nSpeech start detected event.");
                    };

                    recognizer.SpeechEndDetected += (s, e) =>
                    {
                        Console.WriteLine("\nSpeech end detected event.");
                    };

                    recognizer.SessionStarted += (s, e) =>
                    {
                        Console.WriteLine("\nSession started event.");
                    };

                    recognizer.SessionStopped += (s, e) =>
                    {
                        Console.WriteLine($"\nSession stopped event.");
                        Console.WriteLine($"\nStop translation.");
                        stopTranslation.TrySetResult(0);
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    Console.WriteLine("Start translation...");
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopTranslation.Task });

                    // Stops translation.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
        }

        // Translation authenticated via API key crendential.
        public static async Task TranslationWithApiKeyCredential()
        {
            // Translation source language.
            // Replace with a language of your choice.
            string fromLanguage = "en-US";

            var keyCred = new ApiKeyCredential("YourSubscriptionKey");

            // Create a SpeechTranslationConfig instance using the v2 endpoint and the token credential for authentication.
            var config = SpeechTranslationConfig.FromEndpoint(new Uri("YourEndpoint"), keyCred);
            config.SpeechRecognitionLanguage = fromLanguage;

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");
            config.AddTargetLanguage("fr");

            var stopTranslation = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Create an audio stream from a wav file.
            // Replace with your own audio file name.
            using (var audioInput = Helper.OpenWavFile(@"whatstheweatherlike.wav"))
            {
                // Creates a translation recognizer using audio stream as input.
                using (var recognizer = new TranslationRecognizer(config, audioInput))
                {
                    // Subscribes to events.
                    recognizer.Recognizing += (s, e) =>
                    {
                        Console.WriteLine($"RECOGNIZING in '{fromLanguage}': Text={e.Result.Text}");
                        foreach (var element in e.Result.Translations)
                        {
                            Console.WriteLine($"    TRANSLATING into '{element.Key}': {element.Value}");
                        }
                    };

                    recognizer.Recognized += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.TranslatedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED in '{fromLanguage}': Text={e.Result.Text}");
                            foreach (var element in e.Result.Translations)
                            {
                                Console.WriteLine($"    TRANSLATED into '{element.Key}': {element.Value}");
                            }
                        }
                        else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                            Console.WriteLine($"    Speech not translated.");
                        }
                        else if (e.Result.Reason == ResultReason.NoMatch)
                        {
                            Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                        }
                    };

                    recognizer.Canceled += (s, e) =>
                    {
                        Console.WriteLine($"CANCELED: Reason={e.Reason}");

                        if (e.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }

                        stopTranslation.TrySetResult(0);
                    };

                    recognizer.SpeechStartDetected += (s, e) =>
                    {
                        Console.WriteLine("\nSpeech start detected event.");
                    };

                    recognizer.SpeechEndDetected += (s, e) =>
                    {
                        Console.WriteLine("\nSpeech end detected event.");
                    };

                    recognizer.SessionStarted += (s, e) =>
                    {
                        Console.WriteLine("\nSession started event.");
                    };

                    recognizer.SessionStopped += (s, e) =>
                    {
                        Console.WriteLine($"\nSession stopped event.");
                        Console.WriteLine($"\nStop translation.");
                        stopTranslation.TrySetResult(0);
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    Console.WriteLine("Start translation...");
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopTranslation.Task });

                    // Stops translation.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
        }
    }
}
