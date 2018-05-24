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

extern void IntentRecognitionWithMicrophone();
extern void IntentRecognitionWithLanguage();
extern void IntentRecognitionWithFile();
extern void IntentContinuousRecognitionUsingEvents();

extern void TranslationWithMicrophone();
extern void TranslationWithFile();
extern void TranslationContinuousRecognitionUsingEvents();

void SpeechSamples()
{
    string input;
    do
    {
        cout << "\nSPEECH RECOGNITION SAMPLES:\n";
        cout << "1.) Speech recognition with microphone input.\n";
        cout << "2.) Speech recognition in the specified language.\n";
        cout << "3.) Speech recognition with file input.\n";
        cout << "4.) Speech recognition using CRIS model.\n";
        cout << "5.) Speech continuous recognition using events.\n";
        cout << "\nChoice (0 for MAIN MENU): ";

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
        case '0':
            break;
        }
    } while (input[0] != '0');
}

void IntentSamples()
{
    string input;
    do
    {
        cout << "\nINTENT RECOGNITION SAMPLES:\n";
        cout << "1.) Intent recognition with microphone input.\n";
        cout << "2.) Intent recognition in the specified language.\n";
        cout << "3.) Intent recognition with file input.\n";
        cout << "4.) Intent continuous recognition using events.\n";
        cout << "\nChoice (0 for MAIN MENU): ";

        input.empty();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            IntentRecognitionWithMicrophone();
            break;
        case '2':
            IntentRecognitionWithLanguage();
            break;
        case '3':
            IntentRecognitionWithFile();
            break;
        case '4':
            IntentContinuousRecognitionUsingEvents();
            break;
        case '0':
            break;
        }
    } while (input[0] != '0');
}

void TranslationSamples()
{
    string input;
    do
    {
        cout << "\nTRANSLATION SAMPLES:\n";
        cout << "1.) Translation with microphone input.\n";
        cout << "2.) Translation with file input.\n";
        cout << "3.) Translation continuous recognition using events.\n";
        cout << "\nChoice (0 for MAIN MENU): ";

        input.empty();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            TranslationWithMicrophone();
            break;
        case '2':
            TranslationWithFile();
            break;
        case '3':
            TranslationContinuousRecognitionUsingEvents();
            break;
        case '0':
            break;
        }
    } while (input[0] != '0');
}

int wmain(int argc, wchar_t **argv)
{
    string input;
    do
    {
        cout << "\nMAIN MENU\n";
        cout << "1.) Speech recognition samples.\n";
        cout << "2.) Intent recognition samples.\n";
        cout << "3.) Translation samples.\n";
        cout << "\nChoice (0 to Exit): ";

        input.empty();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            SpeechSamples();
            break;
        case '2':
            IntentSamples();
            break;
        case '3':
            TranslationSamples();
            break;
        case '0':
            break;
        }
    } while (input[0] != '0');
}
