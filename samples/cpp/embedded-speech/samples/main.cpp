//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <iostream>
#include <speechapi_cxx.h>

using namespace std;

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

extern void EmbeddedIntentRecognitionFromMicrophone();
extern void EmbeddedIntentRecognitionWithKeywordFromMicrophone();

extern void ListEmbeddedSpeechSynthesisVoices();
extern void EmbeddedSpeechSynthesisToSpeaker();
extern void HybridSpeechSynthesisToSpeaker();

extern void ListEmbeddedSpeechTranslationModels();
extern void EmbeddedSpeechTranslationFromMicrophone();

extern void EmbeddedSpeechRecognitionPerformanceTest();


int main()
{
    try
    {
        if (VerifySettings() != true)
        {
            return 1;
        }

        string input;
        do
        {
            cout << "\nEMBEDDED SPEECH SAMPLES\n";
            cout << "\nSpeech recognition\n";
            cout << " 1. List embedded speech recognition models.\n";
            cout << " 2. Embedded speech recognition with microphone input.\n";
            cout << " 3. Embedded speech recognition with microphone input, keyword-triggered.\n";
            cout << " 4. Embedded speech recognition with WAV file input.\n";
            cout << " 5. Embedded speech recognition with push stream input.\n";
            cout << " 6. Embedded speech recognition with pull stream input.\n";
            cout << " 7. Hybrid (cloud & embedded) speech recognition with microphone input.\n";
            cout << "\nIntent recognition\n";
            cout << " 8. Embedded intent recognition with microphone input.\n";
            cout << " 9. Embedded intent recognition with microphone input, keyword-triggered.\n";
            cout << "\nSpeech synthesis\n";
            cout << "10. List embedded speech synthesis voices.\n";
            cout << "11. Embedded speech synthesis with speaker output.\n";
            cout << "12. Hybrid (cloud & embedded) speech synthesis with speaker output.\n";
            cout << "\nSpeech translation\n";
            cout << "13. List embedded speech translation models.\n";
            cout << "14. Embedded speech translation with microphone input.\n";
            cout << "\nDevice performance measurement\n";
            cout << "15. Embedded speech recognition.\n";
            cout << "\nChoose a number (or none for exit) and press Enter: ";
            cout.flush();

            input.clear();
            getline(cin, input);
            if (input.empty()) break;

            int choice;
            istringstream iss(input);
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
                if (HasSpeechRecognitionModel()) EmbeddedIntentRecognitionFromMicrophone();
                break;
            case 9:
                if (HasSpeechRecognitionModel()) EmbeddedIntentRecognitionWithKeywordFromMicrophone();
                break;
            case 10:
                ListEmbeddedSpeechSynthesisVoices();
                break;
            case 11:
                if (HasSpeechSynthesisVoice()) EmbeddedSpeechSynthesisToSpeaker();
                break;
            case 12:
                if (HasSpeechSynthesisVoice()) HybridSpeechSynthesisToSpeaker();
                break;
            case 13:
                ListEmbeddedSpeechTranslationModels();
                break;
            case 14:
                if (HasSpeechTranslationModel()) EmbeddedSpeechTranslationFromMicrophone();
                break;
            case 15:
                if (HasSpeechRecognitionModel()) EmbeddedSpeechRecognitionPerformanceTest();
                break;
            default:
                break;
            }
        } while (true);
    }
    catch (const exception& e)
    {
        cerr << "Exception caught: " << e.what() << endl;
        return 2;
    }

    return 0;
}
