//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

using System;
using Microsoft.CognitiveServices.Speech.Diagnostics.Logging;

namespace MicrosoftSpeechSDKSamples
{
    class Program
    {
        static int Main()
        {
            try
            {
                var result = Settings.VerifySettingsAsync().Result;
                if (!result)
                {
                    return 1;
                }

                // Enable Speech SDK logging. If you want to report an issue, include this log with the report.
                // If no path is specified, the log file will be created in the program default working folder.
                // If a path is specified, make sure that it is writable by the application process.
                /*
                FileLogger.Start("SpeechSDK.log");
                */

                string input;
                do
                {
                    Console.WriteLine("\nEMBEDDED SPEECH SAMPLES");
                    Console.WriteLine("\nSpeech recognition");
                    Console.WriteLine(" 1. List embedded speech recognition models.");
                    Console.WriteLine(" 2. Embedded speech recognition with microphone input.");
                    Console.WriteLine(" 3. Embedded speech recognition with microphone input, keyword-triggered.");
                    Console.WriteLine(" 4. Embedded speech recognition with WAV file input.");
                    Console.WriteLine(" 5. Embedded speech recognition with push stream input.");
                    Console.WriteLine(" 6. Embedded speech recognition with pull stream input.");
                    Console.WriteLine(" 7. Hybrid (cloud & embedded) speech recognition with microphone input.");
                    Console.WriteLine("\nSpeech synthesis");
                    Console.WriteLine(" 8. List embedded speech synthesis voices.");
                    Console.WriteLine(" 9. Embedded speech synthesis with speaker output.");
                    Console.WriteLine("10. Hybrid (cloud & embedded) speech synthesis with speaker output.");
                    Console.WriteLine("\nSpeech translation");
                    Console.WriteLine("11. List embedded speech translation models.");
                    Console.WriteLine("12. Embedded speech translation with microphone input.");
                    Console.WriteLine("\nDevice performance measurement");
                    Console.WriteLine("13. Embedded speech recognition.");
                    Console.Write("\nChoose a number (or none for exit) and press Enter: ");

                    input = Console.ReadLine();
                    if (string.IsNullOrEmpty(input)) break;

                    int choice;
                    bool success = int.TryParse(input, out choice);
                    if (!success) continue;

                    switch (choice)
                    {
                        case 1:
                            SpeechRecognitionSamples.ListEmbeddedModels();
                            break;
                        case 2:
                            if (Settings.HasSpeechRecognitionModel()) SpeechRecognitionSamples.EmbeddedRecognitionFromMicrophone();
                            break;
                        case 3:
                            if (Settings.HasSpeechRecognitionModel()) SpeechRecognitionSamples.EmbeddedRecognitionWithKeywordFromMicrophone();
                            break;
                        case 4:
                            if (Settings.HasSpeechRecognitionModel()) SpeechRecognitionSamples.EmbeddedRecognitionFromWavFile();
                            break;
                        case 5:
                            if (Settings.HasSpeechRecognitionModel()) SpeechRecognitionSamples.EmbeddedRecognitionFromPushStream();
                            break;
                        case 6:
                            if (Settings.HasSpeechRecognitionModel()) SpeechRecognitionSamples.EmbeddedRecognitionFromPullStream();
                            break;
                        case 7:
                            if (Settings.HasSpeechRecognitionModel()) SpeechRecognitionSamples.HybridRecognitionFromMicrophone();
                            break;
                        case 8:
                            SpeechSynthesisSamples.ListEmbeddedVoicesAsync().Wait();
                            break;
                        case 9:
                            if (Settings.HasSpeechSynthesisVoice()) SpeechSynthesisSamples.EmbeddedSynthesisToSpeaker();
                            break;
                        case 10:
                            if (Settings.HasSpeechSynthesisVoice()) SpeechSynthesisSamples.HybridSynthesisToSpeaker();
                            break;
                        case 11:
                            SpeechTranslationSamples.ListEmbeddedModels();
                            break;
                        case 12:
                            if (Settings.HasSpeechTranslationModel()) SpeechTranslationSamples.EmbeddedTranslationFromMicrophone();
                            break;
                        case 13:
                            if (Settings.HasSpeechRecognitionModel()) SpeechRecognitionSamples.EmbeddedSpeechRecognitionPerformanceTest().Wait();
                            break;
                        default:
                            break;
                    }
                } while (true);

                FileLogger.Stop();
            }
            catch (Exception e)
            {
                Console.Error.WriteLine($"Exception caught: {e}");
                return 2;
            }

            return 0;
        }
    }
}
