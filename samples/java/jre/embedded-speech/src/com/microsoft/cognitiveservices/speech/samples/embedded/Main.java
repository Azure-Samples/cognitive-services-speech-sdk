//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
package com.microsoft.cognitiveservices.speech.samples.embedded;

import java.util.Scanner;


public class Main
{
    @SuppressWarnings("resource") // Scanner
    public static void main(String[] args)
    {
        try
        {
            boolean result = Settings.verifySettings();
            if (!result)
            {
                System.exit(1);
            }

            String input;
            do
            {
                System.out.println("\nEMBEDDED SPEECH SAMPLES");
                System.out.println("\nSpeech recognition");
                System.out.println(" 1. List embeddded speech recognition models.");
                System.out.println(" 2. Embedded speech recognition with microphone input.");
                System.out.println(" 3. Embedded speech recognition with microphone input, keyword-triggered.");
                System.out.println(" 4. Embedded speech recognition with WAV file input.");
                System.out.println(" 5. Embedded speech recognition with push stream input.");
                System.out.println(" 6. Embedded speech recognition with pull stream input.");
                System.out.println(" 7. Hybrid (cloud & embedded) speech recognition with microphone input.");
                System.out.println("\nIntent recognition");
                System.out.println(" 8. Embedded intent recognition with microphone input.");
                System.out.println(" 9. Embedded intent recognition with microphone input, keyword-triggered.");
                System.out.println("\nSpeech synthesis");
                System.out.println("10. List embeddded speech synthesis voices.");
                System.out.println("11. Embedded speech synthesis with speaker output.");
                System.out.println("12. Hybrid (cloud & embedded) speech synthesis with speaker output.");
                System.out.print("\nChoose a number (or none for exit) and press Enter: ");
    
                input = new Scanner(System.in).nextLine();
                if (input == null || input.isEmpty()) break;

                int choice = Integer.parseInt(input);

                switch (choice)
                {
                case 1:
                    SpeechRecognitionSamples.listEmbeddedModels();
                    break;
                case 2:
                    SpeechRecognitionSamples.embeddedRecognitionFromMicrophone();
                    break;
                case 3:
                    SpeechRecognitionSamples.embeddedRecognitionWithKeywordFromMicrophone();
                    break;
                case 4:
                    SpeechRecognitionSamples.embeddedRecognitionFromWavFile();
                    break;
                case 5:
                    SpeechRecognitionSamples.embeddedRecognitionFromPushStream();
                    break;
                case 6:
                    SpeechRecognitionSamples.embeddedRecognitionFromPullStream();
                    break;
                case 7:
                    SpeechRecognitionSamples.hybridRecognitionFromMicrophone();
                    break;
                case 8:
                    IntentRecognitionSamples.embeddedRecognitionFromMicrophone();
                    break;
                case 9:
                    IntentRecognitionSamples.embeddedRecognitionWithKeywordFromMicrophone();
                    break;
                case 10:
                    SpeechSynthesisSamples.listEmbeddedVoicesAsync();
                    break;
                case 11:
                    SpeechSynthesisSamples.embeddedSynthesisToSpeaker();
                    break;
                case 12:
                    SpeechSynthesisSamples.hybridSynthesisToSpeaker();
                    break;
                default:
                    break;
                }
            } while (true);
        }
        catch (Exception e)
        {
            System.err.println("Exception caught: " + e.toString());
            System.exit(2);
        }

        System.exit(0);
    }
}
