//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_test_console.h: Declarations for CarbonTestConsole tool C++ class
//

#pragma once

using namespace CARBON_NAMESPACE_ROOT;
using namespace CARBON_NAMESPACE_ROOT::Recognition;
using namespace CARBON_NAMESPACE_ROOT::Recognition::Speech;
using namespace CARBON_NAMESPACE_ROOT::Recognition::Intent;

class CarbonTestConsole
{
public:

    CarbonTestConsole();
    ~CarbonTestConsole();

    int Run(int argc, const wchar_t* argv[]);

private:

    struct ConsoleArgs
    {
        bool m_fWaitForDebugger = false;

        std::string m_strRecognizerType;
        bool m_fCommandSystem = false;
        
        bool m_fMicrophoneInput = true;
        std::wstring m_strInput;

        std::wstring m_strEndpointUri;
        std::wstring m_strSubscriptionKey;

        bool m_fRecognizeAsync = false;

        bool m_fContinuousRecognition = false;
        std::wstring m_strContinuousRecognitionSeconds;
        uint16_t m_continuousRecognitionSeconds = 0;

        std::wstring m_strRunSampleName;

        bool m_fInteractivePrompt = false;
    };

    bool ParseConsoleArgs(int argc, const wchar_t* argv[], ConsoleArgs* pconsoleArgs);
    bool ValidateConsoleArgs(ConsoleArgs* pconsoleArgs);
    void ProcessConsoleArgs(ConsoleArgs* pconsoleArgs);

    void DisplayConsoleHeader();
    void DisplayConsoleUsage();
    void DisplayConsolePrompt();

    void ConsoleWrite(const wchar_t* psz, ...);
    void ConsoleWriteLine(const wchar_t* pszFormat, ...);
    bool ConsoleReadLine(std::wstring& str);

    bool GetConsoleInput(std::wstring& str);
    void ProcessConsoleInput(const wchar_t* psz);

    void ConsoleInput_Help();
    void ConsoleInput_HelpOn(const wchar_t* psz);
    void ConsoleInput_HelpOnRecognizer();
    void ConsoleInput_HelpOnSpeech();
    void ConsoleInput_HelpOnIntent();
    void ConsoleInput_HelpOnCommandSystem();

    void ConsoleInput_Recognizer(const wchar_t* psz, std::shared_ptr<BaseAsyncRecognizer>& recognizer);
    void ConsoleInput_SpeechRecognizer(const wchar_t* psz, std::shared_ptr<SpeechRecognizer>& speechRecognizer);
    void ConsoleInput_IntentRecognizer(const wchar_t* psz, std::shared_ptr<IntentRecognizer>& intentRecognizer);

    template <class T>
    void Recognizer_IsEnabled(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_Enable(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_Disable(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_Recognize(std::shared_ptr<T>& recognizer);
    void Recognizer_Recognize(std::shared_ptr<SpeechRecognizer>& recognizer);

    template <class T>
    void Recognizer_StartContinuousRecognition(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_StopContinuousRecognition(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_Event(const wchar_t* psz, EventSignal<T>& recognizerEvent, typename::EventSignal<T>::CallbackFunction callback);

    void Recognizer_SessionStartedHandler(const SessionEventArgs& e) { ConsoleWriteLine(L"SessionStartedHandler: %ls", e.SessionId.c_str()); };
    void Recognizer_SessionStoppedHandler(const SessionEventArgs& e) { ConsoleWriteLine(L"SessionStoppedHandler: %ls", e.SessionId.c_str()); };
    void Recognizer_SoundStartedHandler(const SessionEventArgs& e) {};
    void Recognizer_SoundStoppedHandler(const SessionEventArgs& e) {};

    void Recognizer_IntermediateResultHandler(const RecognitionEventArgs& e) {};
    void Recognizer_FinalResultHandler(const RecognitionEventArgs& e) {};
    void Recognizer_NoMatchHandler(const RecognitionEventArgs& e) {};
    void Recognizer_CanceledHandler(const RecognitionEventArgs& e) {};

    void SpeechRecognizer_IntermediateResultHandler(const SpeechRecognitionEventArgs& e) { ConsoleWriteLine(L"IntermediateResultHandler: %ls", ToString(e).c_str()); };
    void SpeechRecognizer_FinalResultHandler(const SpeechRecognitionEventArgs& e) { ConsoleWriteLine(L"FinalResultHandler: %ls", ToString(e).c_str());}
    void SpeechRecognizer_NoMatchHandler(const SpeechRecognitionEventArgs& e) { ConsoleWriteLine(L"NoMatchHandler: %ls", ToString(e).c_str()); }
    void SpeechRecognizer_CanceledHandler(const SpeechRecognitionEventArgs& e) { ConsoleWriteLine(L"SpeechRecognizer_CanceledHandler!!!"); };

    void IntentRecognizer_IntermediateResultHandler(const int& e) {};
    void IntentRecognizer_FinalResultHandler(const int& e) {};
    void IntentRecognizer_NoMatchHandler(const int& e) {};
    void IntentRecognizer_CanceledHandler(const int& e) {};

    std::wstring ToString(const SpeechRecognitionEventArgs& e);

    void ConsoleInput_CommandSystem(const wchar_t* psz);

    bool ShouldInitCarbon() { return m_recognizer == nullptr; }
    void EnsureInitCarbon(ConsoleArgs* pconsoleArgs);

    void InitCarbon(ConsoleArgs* pconsoleArgs);
    void TermCarbon();

    void InitRecognizer(const std::string& recognizerType, const std::wstring& wavFileName);
    void InitCommandSystem();

    void WaitForDebugger();

    void RecognizeAsync();
    void ContinuousRecognition(uint16_t seconds);

    void RunSample(const std::wstring& strSampleName);

    void RunInteractivePrompt();

    void Sample_HelloWorld();
    void Sample_HelloWorld_WithEvents();
    void Sample_HelloWorld_In_C();

private:

    std::shared_ptr<BaseAsyncRecognizer> m_recognizer;
    std::shared_ptr<SpeechRecognizer> m_speechRecognizer;
    std::shared_ptr<IntentRecognizer> m_intentRecognizer;
    void* m_commandSystem = nullptr;
};
