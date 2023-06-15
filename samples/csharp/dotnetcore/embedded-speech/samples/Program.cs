//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

using System;

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

                string input;
                do
                {
                    Console.WriteLine("\nEMBEDDED SPEECH SAMPLES");
                    Console.WriteLine("\nSpeech recognition");
                    Console.WriteLine(" 1. List embeddded speech recognition models.");
                    Console.WriteLine(" 2. Embedded speech recognition with microphone input.");
                    Console.WriteLine(" 3. Embedded speech recognition with microphone input, keyword-triggered.");
                    Console.WriteLine(" 4. Embedded speech recognition with WAV file input.");
                    Console.WriteLine(" 5. Embedded speech recognition with push stream input.");
                    Console.WriteLine(" 6. Embedded speech recognition with pull stream input.");
                    Console.WriteLine(" 7. Hybrid (cloud & embedded) speech recognition with microphone input.");
                    Console.WriteLine("\nIntent recognition");
                    Console.WriteLine(" 8. Embedded intent recognition with microphone input.");
                    Console.WriteLine(" 9. Embedded intent recognition with microphone input, keyword-triggered.");
                    Console.WriteLine("\nSpeech synthesis");
                    Console.WriteLine("10. List embeddded speech synthesis voices.");
                    Console.WriteLine("11. Embedded speech synthesis with speaker output.");
                    Console.WriteLine("12. Hybrid (cloud & embedded) speech synthesis with speaker output.");
                    Console.WriteLine("\nSpeech translation");
                    Console.WriteLine("13. List embeddded speech translation models.");
                    Console.WriteLine("14. Embedded speech translation with microphone input.");
                    Console.Write("\nChoose a number (or none for exit) and press Enter: ");

                    input = Console.ReadLine();
                    if (string.IsNullOrEmpty(input)) break;

                    int choice;
                    bool success = int.TryParse(input, out choice);
                    if (!success) continue;

                    switch (choice)
                    {
                        case 1:
                            if (Settings.HasSpeechRecognitionModel()) SpeechRecognitionSamples.ListEmbeddedModels();
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
                            if (Settings.HasSpeechRecognitionModel()) IntentRecognitionSamples.EmbeddedRecognitionFromMicrophone();
                            break;
                        case 9:
                            if (Settings.HasSpeechRecognitionModel()) IntentRecognitionSamples.EmbeddedRecognitionWithKeywordFromMicrophone();
                            break;
                        case 10:
                            if (Settings.HasSpeechSynthesisVoice()) SpeechSynthesisSamples.ListEmbeddedVoicesAsync().Wait();
                            break;
                        case 11:
                            if (Settings.HasSpeechSynthesisVoice()) SpeechSynthesisSamples.EmbeddedSynthesisToSpeaker();
                            break;
                        case 12:
                            if (Settings.HasSpeechSynthesisVoice()) SpeechSynthesisSamples.HybridSynthesisToSpeaker();
                            break;
                        case 13:
                            if (Settings.HasSpeechTranslationModel()) SpeechTranslationSamples.ListEmbeddedModels();
                            break;
                        case 14:
                            if (Settings.HasSpeechTranslationModel()) SpeechTranslationSamples.EmbeddedTranslationFromMicrophone();
                            break;
                        default:
                            break;
                    }
                } while (true);
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
