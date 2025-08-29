package com.microsoft.cognitiveservices.speech.samples.console;

import java.util.Scanner;

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

@SuppressWarnings("resource") // scanner
public class Main {

    public static void main(String[] args) {
        String prompt = "Your choice (0: Stop): ";

        System.out.println("1. Pronunciation assessment with microphone input.");
        System.out.println("2. Pronunciation assessment with stream input.");
        System.out.println("3. Pronunciation assessment continuous with file.");
        System.out.println("4. Pronunciation assessment configured with JSON.");
        System.out.println("5. Pronunciation assessment with REST API.");

        System.out.print(prompt);

        try {
            String x;
            do {
                x = new Scanner(System.in).nextLine();
                System.out.println("");
                switch (x.toLowerCase()) {
                case "1":
                    SpeechRecognitionSamples.pronunciationAssessmentWithMicrophoneAsync();
                    break;
                case "2":
                    SpeechRecognitionSamples.pronunciationAssessmentWithPushStream();
                    break;
                case "3":
                    SpeechRecognitionSamples.pronunciationAssessmentContinuousWithFile();;
                    break;
                case "4":
                    SpeechRecognitionSamples.pronunciationAssessmentConfiguredWithJson();
                    break;
                case "5":
                    SpeechRecognitionSamples.pronunciationAssessmentWithRestApi();
                    break;
                case "0":
                    System.out.println("Exiting...");
                    break;
                }
                System.out.println("\nExecution done. " + prompt);
            } while (!x.equals("0"));

            System.out.println("Finishing demo.");
            System.exit(0);
        } catch (Exception ex) {
            System.out.println("Unexpected " + ex.toString());
            System.exit(1);
        }
    }
}
