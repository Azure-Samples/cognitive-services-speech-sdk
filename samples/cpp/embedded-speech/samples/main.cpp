//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <iostream>
#include <speechapi_cxx.h>

extern bool VerifySettings();
extern bool HasSpeechRecognitionModel();
extern bool HasSpeechSynthesisVoice();
extern bool HasSpeechTranslationModel();

extern void ListEmbeddedSpeechRecognitionModels();
extern void EmbeddedSpeechRecognitionFromMicrophone();
extern void EmbeddedSpeechRecognitionWithKeywordFromMicrophone();
extern void EmbeddedSpeechRecognitionFromWavFile();
extern void EmbeddedSpeechRecognitionFromPushStream();
extern void EmbeddedSpeechRecognitionFromPullStream();
extern void HybridSpeechRecognitionFromMicrophone();

extern void ListEmbeddedSpeechSynthesisVoices();
extern void EmbeddedSpeechSynthesisToSpeaker();
extern void HybridSpeechSynthesisToSpeaker();

extern void ListEmbeddedSpeechTranslationModels();
extern void EmbeddedSpeechTranslationFromMicrophone();

extern void EmbeddedSpeechRecognitionPerformanceTest();

using namespace Microsoft::CognitiveServices::Speech::Diagnostics::Logging;


int main()
{
    try
    {
        if (VerifySettings() != true)
        {
            return 1;
        }

        // Enable Speech SDK logging. If you want to report an issue, include this log with the report.
        // If no path is specified, the log file will be created in the program default working folder.
        // If a path is specified, make sure that it is writable by the application process.
        /*
        FileLogger::Start("SpeechSDK.log");
        */

        std::string input;
        do
        {
            std::cout << "\nEMBEDDED SPEECH SAMPLES\n";
            std::cout << "\nSpeech recognition\n";
            std::cout << " 1. List embedded speech recognition models.\n";
            std::cout << " 2. Embedded speech recognition with microphone input.\n";
            std::cout << " 3. Embedded speech recognition with microphone input, keyword-triggered.\n";
            std::cout << " 4. Embedded speech recognition with WAV file input.\n";
            std::cout << " 5. Embedded speech recognition with push stream input.\n";
            std::cout << " 6. Embedded speech recognition with pull stream input.\n";
            std::cout << " 7. Hybrid (cloud & embedded) speech recognition with microphone input.\n";
            std::cout << "\nSpeech synthesis\n";
            std::cout << " 8. List embedded speech synthesis voices.\n";
            std::cout << " 9. Embedded speech synthesis with speaker output.\n";
            std::cout << "10. Hybrid (cloud & embedded) speech synthesis with speaker output.\n";
            std::cout << "\nSpeech translation\n";
            std::cout << "11. List embedded speech translation models.\n";
            std::cout << "12. Embedded speech translation with microphone input.\n";
            std::cout << "\nDevice performance measurement\n";
            std::cout << "13. Embedded speech recognition.\n";
            std::cout << "\nChoose a number (or none for exit) and press Enter: ";
            std::cout.flush();

            input.clear();
            std::getline(std::cin, input);
            if (input.empty()) break;

            int choice;
            std::istringstream iss(input);
            iss >> choice;

            switch (choice)
            {
            case 1:
                ListEmbeddedSpeechRecognitionModels();
                break;
            case 2:
                if (HasSpeechRecognitionModel()) EmbeddedSpeechRecognitionFromMicrophone();
                break;
            case 3:
                if (HasSpeechRecognitionModel()) EmbeddedSpeechRecognitionWithKeywordFromMicrophone();
                break;
            case 4:
                if (HasSpeechRecognitionModel()) EmbeddedSpeechRecognitionFromWavFile();
                break;
            case 5:
                if (HasSpeechRecognitionModel()) EmbeddedSpeechRecognitionFromPushStream();
                break;
            case 6:
                if (HasSpeechRecognitionModel()) EmbeddedSpeechRecognitionFromPullStream();
                break;
            case 7:
                if (HasSpeechRecognitionModel()) HybridSpeechRecognitionFromMicrophone();
                break;
            case 8:
                ListEmbeddedSpeechSynthesisVoices();
                break;
            case 9:
                if (HasSpeechSynthesisVoice()) EmbeddedSpeechSynthesisToSpeaker();
                break;
            case 10:
                if (HasSpeechSynthesisVoice()) HybridSpeechSynthesisToSpeaker();
                break;
            case 11:
                ListEmbeddedSpeechTranslationModels();
                break;
            case 12:
                if (HasSpeechTranslationModel()) EmbeddedSpeechTranslationFromMicrophone();
                break;
            case 13:
                if (HasSpeechRecognitionModel()) EmbeddedSpeechRecognitionPerformanceTest();
                break;
            default:
                break;
            }
        } while (true);

        FileLogger::Stop();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 2;
    }

    return 0;
}
