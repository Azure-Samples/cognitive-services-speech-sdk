//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <iostream>
#include <speechapi_cxx.h>

using namespace std;

extern bool VerifySettings();

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
            cout << " 1. List embeddded speech recognition models.\n";
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
            cout << "10. List embeddded speech synthesis voices.\n";
            cout << "11. Embedded speech synthesis with speaker output.\n";
            cout << "12. Hybrid (cloud & embedded) speech synthesis with speaker output.\n";
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
                EmbeddedSpeechRecognitionFromMicrophone();
                break;
            case 3:
                EmbeddedSpeechRecognitionWithKeywordFromMicrophone();
                break;
            case 4:
                EmbeddedSpeechRecognitionFromWavFile();
                break;
            case 5:
                EmbeddedSpeechRecognitionFromPushStream();
                break;
            case 6:
                EmbeddedSpeechRecognitionFromPullStream();
                break;
            case 7:
                HybridSpeechRecognitionFromMicrophone();
                break;
            case 8:
                EmbeddedIntentRecognitionFromMicrophone();
                break;
            case 9:
                EmbeddedIntentRecognitionWithKeywordFromMicrophone();
                break;
            case 10:
                ListEmbeddedSpeechSynthesisVoices();
                break;
            case 11:
                EmbeddedSpeechSynthesisToSpeaker();
                break;
            case 12:
                HybridSpeechSynthesisToSpeaker();
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
