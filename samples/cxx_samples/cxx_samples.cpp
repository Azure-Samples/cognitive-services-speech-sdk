//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "stdafx.h"
#include <iostream>
#include <string>

using namespace std;

extern void SpeechRecognitionWithMicrophone();
extern void SpeechRecognitionWithLanguage();
extern void SpeechRecognitionWithFile();
extern void SpeechRecognitionUsingCustomizedModel();
extern void SpeechContinuousRecognitionUsingEvents();
extern void TranslationWithMicrophone();
extern void TranslationWithFile();
extern void TranslationContinuousRecognitionUsingEvents();

int wmain(int argc, wchar_t **argv)
{
    cout << "1. Speech recognition with microphone input.\n";
    cout << "2. Speech recognition in the specified language.\n";
    cout << "3. Speech recognition with file input.\n";
    cout << "4. Speech recognition using CRIS model.\n";
    cout << "5. Speech continuous recognition using events.\n";
    cout << "6. Translation with microphone input.\n";
    cout << "7. Translation with file input.\n";
    cout << "8. Translation continuous recognition using events.\n";
    cout << "Your choice (0. Exit.): ";

    string input;
    do
    {
        input.empty();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            SpeechRecognitionWithMicrophone();
            break;
        case '2':
            SpeechRecognitionWithLanguage();
            break;
        case '3':
            SpeechRecognitionWithFile();
            break;
        case '4':
            SpeechRecognitionUsingCustomizedModel();
            break;
        case '5':
            SpeechContinuousRecognitionUsingEvents();
            break;
        case '6':
            TranslationWithMicrophone();
            break;
        case '7':
            TranslationWithFile();
            break;
        case '8':
            TranslationContinuousRecognitionUsingEvents();
            break;
        case '0':
            cout << "\nStop recognition";
            break;
        }
        cout << "\nRecognition done. Your Choice (0. Exit): ";
    } while (input[0] != '0');
}
