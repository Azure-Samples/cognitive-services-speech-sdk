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
extern void PronunciationAssessmentWithStream();
extern void PronunciationAssessmentConfiguredWithJson();
extern void PronunciationAssessmentWithContentAssessment();
extern void SpeechContinuousRecognitionFromDefaultMicrophoneWithMASEnabled();
extern void SpeechRecognitionFromMicrophoneWithMASEnabledAndPresetGeometrySpecified();
extern void SpeechContinuousRecognitionFromMultiChannelFileWithMASEnabledAndCustomGeometrySpecified();
extern void SpeechRecognitionFromPullStreamWithSelectMASEnhancementsEnabled();
extern void SpeechContinuousRecognitionFromPushStreamWithMASEnabledAndBeamformingAnglesSpecified();

extern void IntentRecognitionWithMicrophone();
extern void IntentRecognitionWithPatternMatchingAndMicrophone();
extern void IntentRecognitionWithLanguage();
extern void IntentContinuousRecognitionWithFile();
extern void IntentPatternMatchingWithMicrophoneAndKeywordSpotting();

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
extern void SpeechSynthesisUsingCustomVoice();
extern void SpeechSynthesisGetAvailableVoices();
extern void SpeechSynthesisVisemeEvent();
extern void SpeechSynthesisBookmarkEvent();

extern void ConversationTranscriptionWithMicrophone();
extern void ConversationTranscriptionWithFile();
extern void ConversationTranscriptionWithPullAudioStream();
extern void ConversationTranscriptionWithPushAudioStream();
extern void ConversationTranscriptionAndLanguageIdWithMicrophone();

extern void MeetingTranscriptionWithMicrophoneUsingAudioProcessingOptions();
extern void MeetingTranscriptionWithPullAudioStreamUsingMultichannelAudio();
extern void MeetingTranscriptionWithPushAudioStreamUsingMultichannelAudio();
extern void MeetingTranscriptionWithMultichannelAudioFile();

extern void SpeakerVerificationWithMicrophone();
extern void SpeakerVerificationWithPushStream();
extern void SpeakerIdentificationWithPullStream();
extern void SpeakerIdentificationWithMicrophone();

// Language Id related tests
extern void SpeechRecognitionAndLanguageIdWithMicrophone();
extern void SpeechRecognitionAndLanguageIdWithCustomModelsWithMicrophone();
extern void SpeechContinuousRecognitionAndLanguageIdWithMultiLingualFile();

extern void TranslationAndLanguageIdWithMicrophone();
extern void ContinuousTranslationAndLanguageIdWithMultiLingualFile();

extern void StandaloneLanguageDetectionWithMicrophone();
extern void StandaloneLanguageDetectionWithFileInput();
extern void StandaloneLanguageDetectionInContinuousModeWithMultiLingualFileInput();

extern void DiagnosticsLoggingFileLoggerWithoutFilter();
extern void DiagnosticsLoggingFileLoggerWithFilter();
extern void DiagnosticsLoggingEventLoggerWithoutFilter();
extern void DiagnosticsLoggingEventLoggerWithFilter();
extern void DiagnosticsLoggingMemoryLogger();

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
        cout << "9.) Speech recognition from default microphone with Microsoft Audio Stack enabled.\n";
        cout << "a.) Speech recognition from a microphone with Microsoft Audio Stack enabled and\n"
                "    pre-defined microphone array geometry specified.\n";
        cout << "b.) Speech recognition from multi-channel file with Microsoft Audio Stack enabled and\n"
                "    custom microphone array geometry specified.\n";
        cout << "c.) Speech recognition from pull stream with custom set of enhancements from\n"
                "    Microsoft Audio Stack enabled.\n";
        cout << "d.) Speech recognition from push stream with Microsoft Audio Stack enabled and\n"
                "    beam-forming angles specified.\n";
        cout << "e.) Pronunciation assessment with stream.\n";
        cout << "f.) Pronunciation assessment configured with json.\n";
        cout << "g.) Pronunciation assessment with content assessment.\n";
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
        case '9':
            SpeechContinuousRecognitionFromDefaultMicrophoneWithMASEnabled();
            break;
        case 'A':
        case 'a':
            SpeechRecognitionFromMicrophoneWithMASEnabledAndPresetGeometrySpecified();
            break;
        case 'B':
        case 'b':
            SpeechContinuousRecognitionFromMultiChannelFileWithMASEnabledAndCustomGeometrySpecified();
            break;
        case 'C':
        case 'c':
            SpeechRecognitionFromPullStreamWithSelectMASEnhancementsEnabled();
            break;
        case 'D':
        case 'd':
            SpeechContinuousRecognitionFromPushStreamWithMASEnabledAndBeamformingAnglesSpecified();
            break;
        case 'E':
        case 'e':
            PronunciationAssessmentWithStream();
            break;
        case 'F':
        case 'f':
            PronunciationAssessmentConfiguredWithJson();
            break;
        case 'G':
        case 'g':
            PronunciationAssessmentWithContentAssessment();
            break;
        case '0':
            break;
        }
    } while (input[0] != '0');
}

void SpeechWithLanguageIdSamples()
{
    string input;
    do
    {
        cout << "\nSPEECH RECOGNITION WITH LANGUAGE ID SAMPLES:\n";
        cout << "1.) Speech recognition with microphone input.\n";
        cout << "2.) Speech recognition with custom models, with microphone input.\n";
        cout << "3.) Continuous speech recognition with multi-lingual file input.\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            SpeechRecognitionAndLanguageIdWithMicrophone();
            break;
        case '2':
            SpeechRecognitionAndLanguageIdWithCustomModelsWithMicrophone();
            break;
        case '3':
            SpeechContinuousRecognitionAndLanguageIdWithMultiLingualFile();
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
        cout << "4.) Intent recognition from default microphone and pattern matching.\n";
        cout << "5.) Intent recognition with keyword spotting from default microphone and pattern matching.\n";
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
        case '4':
            IntentRecognitionWithPatternMatchingAndMicrophone();
            break;
        case '5':
            IntentPatternMatchingWithMicrophoneAndKeywordSpotting();
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
        cout << "1.) Single utterance translation with microphone input.\n";
        cout << "2.) Continuous translation with microphone input.\n";
        cout << "3.) Single utterance translation with language detection using microphone input.\n";
        cout << "4.) Continuous translation with language detection using multi-lingual input file.\n";
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
        case '3':
            TranslationAndLanguageIdWithMicrophone();
            break;
        case '4':
            ContinuousTranslationAndLanguageIdWithMultiLingualFile();
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
        cout << "D.) Speech synthesis using Custom Voice\n";
        cout << "E.) Speech synthesis get available voices\n";
        cout << "F.) Speech synthesis viseme event.\n";
        cout << "G.) Speech synthesis bookmark event.\n";
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
        case 'D':
        case 'd':
            SpeechSynthesisUsingCustomVoice();
            break;
        case 'E':
        case 'e':
            SpeechSynthesisGetAvailableVoices();
            break;
        case 'F':
        case 'f':
            SpeechSynthesisVisemeEvent();
            break;
        case 'G':
        case 'g':
            SpeechSynthesisBookmarkEvent();
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
        cout << "1.) ConversationTranscriber with microphone.\n";
        cout << "2.) ConversationTranscriber with file.\n";
        cout << "3.) ConversationTranscriber with pull audio stream.\n";
        cout << "4.) ConversationTranscriber with push audio stream.\n";
        cout << "5.) ConversationTranscriber with micrphone using language detection.\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            ConversationTranscriptionWithMicrophone();
            break;

        case '2':
            ConversationTranscriptionWithFile();
            break;

        case '3':
            ConversationTranscriptionWithPullAudioStream();
            break;

        case '4':
            ConversationTranscriptionWithPushAudioStream();
            break;

        case '5':
            ConversationTranscriptionAndLanguageIdWithMicrophone();
            break;

        case '0':
            break;            
        }
    } while (input[0] != '0');
}

void MeetingTranscriberSamples()
{
    string input;
    do
    {
        cout << "\nMeetingTranscriber SAMPLES:\n";
        cout << "1.) MeetingTranscriber with microphone using audio processing options.\n";
        cout << "2.) MeetingTranscriber with pull audio stream using multichannel audio.\n";
        cout << "3.) MeetingTranscriber with push audio stream using multichannel audio.\n";
        cout << "4.) MeetingTranscriber with audio file using multichannel audio.\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            MeetingTranscriptionWithMicrophoneUsingAudioProcessingOptions();
            break;

        case '2':
            MeetingTranscriptionWithPullAudioStreamUsingMultichannelAudio();
            break;

        case '3':
            MeetingTranscriptionWithPushAudioStreamUsingMultichannelAudio();
            break;

        case '4':
            MeetingTranscriptionWithMultichannelAudioFile();
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

void StandaloneLanguageDetectionSamples()
{
    string input;
    do
    {
        cout << "\nStandalone Language Detection SAMPLES:\n";
        cout << "1.) Standalone language detection (signle utterance) with microphone input.\n";
        cout << "2.) Standalone language detection (single utterance) with file input.\n";
        cout << "3.) Standalone language detection (continuous, multiple utterances) with multi-lingual file input.\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            StandaloneLanguageDetectionWithMicrophone();
            break;

        case '2':
            StandaloneLanguageDetectionWithFileInput();
            break;

        case '3':
            StandaloneLanguageDetectionInContinuousModeWithMultiLingualFileInput();
            break;

        case '0':
            break;
        }
    } while (input[0] != '0');
}

void DiagnosticsLoggingSamples()
{
    string input;
    do
    {
        cout << "\nTRACE LOGGING SAMPLES:\n";
        cout << "1.) Trace logging to a file (without filter).\n";
        cout << "2.) Trace logging to a file (with filter).\n";
        cout << "3.) Trace logging events (without filter).\n";
        cout << "4.) Trace logging events (with filter).\n";
        cout << "5.) Trace logging to memory.\n";
        cout << "\nChoice (0 for MAIN MENU): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            DiagnosticsLoggingFileLoggerWithoutFilter();
            break;
        case '2':
            DiagnosticsLoggingFileLoggerWithFilter();
            break;
        case '3':
            DiagnosticsLoggingEventLoggerWithoutFilter();
            break;
        case '4':
            DiagnosticsLoggingEventLoggerWithFilter();
            break;
        case '5':
            DiagnosticsLoggingMemoryLogger();
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
        cout << "6.) Meeting transcriber samples.\n";
        cout << "7.) Speaker recognition samples.\n";
        cout << "8.) Standalone language detection samples.\n";
        cout << "9.) Speech recognition with language detection samples.\n";
        cout << "10.) Diagnostics logging samples (trace logging).\n";
        cout << "\nChoice (0 to Exit): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        int choice = -1;

        try
        {
            choice = stoi(input);
        }
        catch (exception&)
        {
        }

        switch (choice)
        {
        case 1:
            SpeechSamples();
            break;
        case 2:
            IntentSamples();
            break;
        case 3:
            TranslationSamples();
            break;
        case 4:
            SpeechSynthesisSamples();
            break;
        case 5:
            ConversationTranscriberSamples();
            break;
        case 6:
            MeetingTranscriberSamples();
            break;
        case 7:
            SpeakerRecognitionSamples();
            break;
        case 8:
            StandaloneLanguageDetectionSamples();
            break;
        case 9:
            SpeechWithLanguageIdSamples();
            break;
        case 10:
            DiagnosticsLoggingSamples();
            break;
        case 0:
            break;
        }
    } while (input[0] != '0');
}
