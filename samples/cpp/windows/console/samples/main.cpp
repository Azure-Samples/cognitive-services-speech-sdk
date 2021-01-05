//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "stdafx.h"
#include <iostream>
#include <string>

using namespace std;

extern void SpeechRecognitionWithMicrophone();
extern void SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat();
extern void SpeechContinuousRecognitionWithFile();
extern void SpeechRecognitionUsingCustomizedModel();
extern void SpeechContinuousRecognitionWithPullStream();
extern void SpeechContinuousRecognitionWithPushStream();
extern void KeywordTriggeredSpeechRecognitionWithMicrophone();
extern void PronunciationAssessmentWithMicrophone();

extern void IntentRecognitionWithMicrophone();
extern void IntentRecognitionWithLanguage();
extern void IntentContinuousRecognitionWithFile();

extern void TranslationWithMicrophone();
extern void TranslationContinuousRecognition();

extern void SpeechSynthesisToSpeaker();
extern void SpeechSynthesisWithLanguage();
extern void SpeechSynthesisWithVoice();
extern void SpeechSynthesisToWaveFile();
extern void SpeechSynthesisToMp3File();
extern void SpeechSynthesisToPullAudioOutputStream();
extern void SpeechSynthesisToPushAudioOutputStream();
extern void SpeechSynthesisToResult();
extern void SpeechSynthesisToAudioDataStream();
extern void SpeechSynthesisEvents();
extern void SpeechSynthesisWordBoundaryEvent();
extern void SpeechSynthesisWithSourceLanguageAutoDetection();

extern void ConversationWithPullAudioStream();
extern void ConversationWithPushAudioStream();

extern void SpeakerVerificationWithMicrophone();
extern void SpeakerVerificationWithPushStream();
extern void SpeakerIdentificationWithPullStream();
extern void SpeakerIdentificationWithMicrophone();

void SpeechSamples()
{
    string input;
    do
    {
        cout << "\nSPEECH RECOGNITION SAMPLES:\n";
        cout << "1.) Speech recognition with microphone input.\n";
        cout << "2.) Speech recognition in the specified language and using detailed output format.\n";
        cout << "3.) Speech continuous recognition with file input.\n";
        cout << "4.) Speech recognition using customized model.\n";
        cout << "5.) Speech recognition using pull stream input.\n";
        cout << "6.) Speech recognition using push stream input.\n";
        cout << "7.) Speech recognition using microphone with a keyword trigger.\n";
        cout << "8.) Pronunciation assessment using microphone input.\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            SpeechRecognitionWithMicrophone();
            break;
        case '2':
            SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat();
            break;
        case '3':
            SpeechContinuousRecognitionWithFile();
            break;
        case '4':
            SpeechRecognitionUsingCustomizedModel();
            break;
        case '5':
            SpeechContinuousRecognitionWithPullStream();
            break;
        case '6':
            SpeechContinuousRecognitionWithPushStream();
            break;
        case '7':
            KeywordTriggeredSpeechRecognitionWithMicrophone();
            break;
        case '8':
            PronunciationAssessmentWithMicrophone();
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
        cout << "3.) Intent continuous recognition with file input.\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
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
            IntentContinuousRecognitionWithFile();
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
        cout << "2.) Translation continuous recognition.\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            TranslationWithMicrophone();
            break;
        case '2':
            TranslationContinuousRecognition();
            break;
        case '0':
            break;
        }
    } while (input[0] != '0');
}

void SpeechSynthesisSamples()
{
    string input;
    do
    {
        cout << "\nSPEECH SYNTHESIS SAMPLES:\n";
        cout << "1.) Speech synthesis to speaker output.\n";
        cout << "2.) Speech synthesis with specified language.\n";
        cout << "3.) Speech synthesis with specified voice.\n";
        cout << "4.) Speech synthesis to wave file.\n";
        cout << "5.) Speech synthesis to mp3 file.\n";
        cout << "6.) Speech synthesis to pull audio output stream.\n";
        cout << "7.) Speech synthesis to push audio output stream.\n";
        cout << "8.) Speech synthesis to result.\n";
        cout << "9.) Speech synthesis to audio data stream.\n";
        cout << "A.) Speech synthesis events.\n";
        cout << "B.) Speech synthesis word boundary event.\n";
        cout << "C.) Speech synthesis with source language auto detection\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            SpeechSynthesisToSpeaker();
            break;
        case '2':
            SpeechSynthesisWithLanguage();
            break;
        case '3':
            SpeechSynthesisWithVoice();
            break;
        case '4':
            SpeechSynthesisToWaveFile();
            break;
        case '5':
            SpeechSynthesisToMp3File();
            break;
        case '6':
            SpeechSynthesisToPullAudioOutputStream();
            break;
        case '7':
            SpeechSynthesisToPushAudioOutputStream();
            break;
        case '8':
            SpeechSynthesisToResult();
            break;
        case '9':
            SpeechSynthesisToAudioDataStream();
            break;
        case 'A':
        case 'a':
            SpeechSynthesisEvents();
            break;
        case 'B':
        case 'b':
            SpeechSynthesisWordBoundaryEvent();
            break;
        case 'C':
        case 'c':
            SpeechSynthesisWithSourceLanguageAutoDetection();
            break;
        case '0':
            break;
        }
    } while (input[0] != '0');
}

void ConversationTranscriberSamples()
{
    string input;
    do
    {
        cout << "\nConversationTranscriber SAMPLES:\n";
        cout << "1.) ConversationTranscriber with pull input audio stream.\n";
        cout << "2.) ConversationTranscriber with push input audio stream.\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            ConversationWithPullAudioStream();
            break;
        case '2':
            ConversationWithPushAudioStream();
            break;
        case '0':
            break;
        }
    } while (input[0] != '0');
}

void SpeakerRecognitionSamples()
{
    string input;
    do
    {
        cout << "\nSpeakerRecognition SAMPLES:\n";
        cout << "1.) Speaker verification with microphone input.\n";
        cout << "2.) Speaker verification with push audio stream input.\n";
        cout << "3.) Speaker identification with pull audio stream input.\n";
        cout << "4.) Speaker identification with microphone input.\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            SpeakerVerificationWithMicrophone();
            break;

        case '2':
            SpeakerVerificationWithPushStream();
            break;

        case '3':
            SpeakerIdentificationWithPullStream();
            break;

        case '4':
            SpeakerIdentificationWithMicrophone();
            break;

        case '0':
            break;
        }
    } while (input[0] != '0');
}

#ifdef _WIN32
int wmain(int argc, wchar_t **argv)
#else
int main(int argc, char **argv)
#endif
{
    string input;
    do
    {
        cout << "\nMAIN MENU\n";
        cout << "1.) Speech recognition samples.\n";
        cout << "2.) Intent recognition samples.\n";
        cout << "3.) Translation samples.\n";
        cout << "4.) Speech synthesis samples.\n";
        cout << "5.) Conversation transcriber samples.\n";
        cout << "6.) Speaker Recognition samples.\n";
        cout << "\nChoice (0 to Exit): ";
        cout.flush();

        input.clear();
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
        case '4':
            SpeechSynthesisSamples();
            break;
        case '5':
            ConversationTranscriberSamples();
            break;
        case '6':
            SpeakerRecognitionSamples();
        case '0':
            break;
        }
    } while (input[0] != '0');
}
