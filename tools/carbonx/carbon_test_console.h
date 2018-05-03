//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_test_console.h: Declarations for CarbonTestConsole tool C++ class
//

#pragma once


using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Intent;
using namespace Microsoft::CognitiveServices::Speech::Translation;


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
        std::wstring m_strUseRecoEngineProperty;
        std::wstring m_strUseLuEngineProperty;
        bool m_fCommandSystem = false;
        
        std::wstring m_audioInput;
        bool m_useInteractiveMicrophone = false;
        bool m_useMockMicrophone = false;

        bool m_useMockKws = false;

        std::wstring m_strMockMicrophoneRealTimePercentage;
        int16_t m_mockMicrophoneRealTimePercentage = 100;

        std::wstring m_mockWavFileName;

        std::wstring m_strEndpointUri;
        std::wstring m_strCustomSpeechModelId;
        std::wstring m_strSubscriptionKey;

        bool m_fRecognizeAsync = false;

        bool m_fContinuousRecognition = false;
        std::wstring m_strContinuousRecognitionSeconds;
        uint16_t m_continuousRecognitionSeconds = 0;

        std::wstring m_strRunSampleName;
        std::wstring m_strHowManyTimes;
        uint16_t m_runHowManyTimes = 1;

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
    void ConsoleInput_HelpOnGlobal();
    void ConsoleInput_HelpOnFactory();
    void ConsoleInput_HelpOnRecognizer();
    void ConsoleInput_HelpOnSpeech();
    void ConsoleInput_HelpOnIntent();
    void ConsoleInput_HelpOnSession();
    void ConsoleInput_HelpOnCommandSystem();

    void ConsoleInput_Factory(const wchar_t* psz);
    void ConsoleInput_Recognizer(const wchar_t* psz, std::shared_ptr<BaseAsyncRecognizer>& recognizer);
    void ConsoleInput_SpeechRecognizer(const wchar_t* psz, std::shared_ptr<SpeechRecognizer>& speechRecognizer);
    void ConsoleInput_IntentRecognizer(const wchar_t* psz, std::shared_ptr<IntentRecognizer>& intentRecognizer);

    void Factory_CreateSpeechRecognizer(const wchar_t* psz);
    // TODO: RobCh: Intent: Extend CarbonX Factory console method to be able to create intent recognizer
    // void Factory_CreateIntentRecognizer(const wchar_t* psz);

    template <class T>
    void Recognizer_IsEnabled(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_Enable(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_Disable(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_Recognize(std::shared_ptr<T>& recognizer);
    void Recognizer_Recognize(std::shared_ptr<SpeechRecognizer>& recognizer);
    void Recognizer_Recognize(std::shared_ptr<IntentRecognizer>& recognizer);
    void Recognizer_Recognize(std::shared_ptr<TranslationRecognizer>& recognizer);

    template <class T>
    void Recognizer_StartContinuousRecognition(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_StopContinuousRecognition(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_StartKeywordRecognition(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_StopKeywordRecognition(std::shared_ptr<T>& recognizer);

    template <class T>
    void Recognizer_Event(const wchar_t* psz, EventSignal<T>& recognizerEvent, typename::EventSignal<T>::CallbackFunction callback);

    void Recognizer_SessionStartedHandler(const SessionEventArgs& e) { ConsoleWriteLine(L"SessionStartedHandler: %ls", e.SessionId.c_str()); };
    void Recognizer_SessionStoppedHandler(const SessionEventArgs& e) { ConsoleWriteLine(L"SessionStoppedHandler: %ls", e.SessionId.c_str()); };
    void Recognizer_SpeechStartDetectedHandler(const RecognitionEventArgs& e) { ConsoleWriteLine(L"SpeechStartDetectedHandler: Session ID : %ls , Offset : %llu", e.SessionId.c_str(), e.Offset); };
    void Recognizer_SpeechEndDetectedHandler(const RecognitionEventArgs& e) { ConsoleWriteLine(L"SpeechEndDetectedHandler: Session ID : %ls, Offset : %llu", e.SessionId.c_str(), e.Offset); };

    void Recognizer_IntermediateResultHandler(const RecognitionEventArgs& e) { UNUSED(e); };
    void Recognizer_FinalResultHandler(const RecognitionEventArgs& e) { UNUSED(e); };
    void Recognizer_NoMatchHandler(const RecognitionEventArgs& e) { UNUSED(e); };
    void Recognizer_CanceledHandler(const RecognitionEventArgs& e) { UNUSED(e); };

    void SpeechRecognizer_IntermediateResultHandler(const SpeechRecognitionEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"IntermediateResultHandler: %ls", ToString(e).c_str()); };
    void SpeechRecognizer_FinalResultHandler(const SpeechRecognitionEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"FinalResultHandler: %ls", ToString(e).c_str());}
    void SpeechRecognizer_NoMatchHandler(const SpeechRecognitionEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"NoMatchHandler: %ls", ToString(e).c_str()); }
    void SpeechRecognizer_CanceledHandler(const SpeechRecognitionEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"CanceledHandler: %ls", ToString(e).c_str()); };

    void TranslationRecognizer_IntermediateResultHandler(const TranslationTextResultEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"Translation IntermediateResultHandler: %ls", ToString(e).c_str()); };
    void TranslationRecognizer_FinalResultHandler(const TranslationTextResultEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"Translation FinalResultHandler: %ls", ToString(e).c_str()); }
    void TranslationRecognizer_SynthesisResultHandler(const TranslationSynthesisResultEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"Translation SynthesisResultHandler: %ls", ToString(e).c_str()); }
    void TranslationRecognizer_ErrorHandler(const TranslationSynthesisResultEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"Translation ErrorHandler: %ls", ToString(e).c_str()); }

    void IntentRecognizer_IntermediateResultHandler(const IntentRecognitionEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"IntermediateResultHandler: %ls", ToString(e).c_str()); };
    void IntentRecognizer_FinalResultHandler(const IntentRecognitionEventArgs& e)  { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"FinalResultHandler: %ls", ToString(e).c_str()); }
    void IntentRecognizer_NoMatchHandler(const IntentRecognitionEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"NoMatchHandler: %ls", ToString(e).c_str()); }
    void IntentRecognizer_CanceledHandler(const IntentRecognitionEventArgs& e) { SPX_DBG_TRACE_VERBOSE("%ls: %ls", __FUNCTION__, ToString(e).c_str()); ConsoleWriteLine(L"CanceledHandler: %ls", ToString(e).c_str()); };

    bool ToBool(const wchar_t* psz);

    std::wstring ToString(bool f);
    std::wstring ToString(const SpeechRecognitionEventArgs& e);
    std::wstring ToString(const IntentRecognitionEventArgs& e);
    std::wstring ToString(const TranslationTextResultEventArgs& e);
    std::wstring ToString(const TranslationSynthesisResultEventArgs& e);

    void ConsoleInput_Session(const wchar_t*);

    void Session_FromSpeechRecognizer();
    // TODO: RobCh: Intent: Extend CarbonX session capability to come from intent recognizer
    // void Session_FromIntentRecognizer();

    template <class T>
    void Parameters_SetString(T &parameters, const wchar_t* psz);

    template <class T>
    void Parameters_GetString(T &parameters, const wchar_t* psz);

    template <class T>
    void Parameters_SetNumber(T &parameters, const wchar_t* psz);

    template <class T>
    void Parameters_GetNumber(T &parameters, const wchar_t* psz);

    template <class T>
    void Parameters_SetBool(T &parameters, const wchar_t* psz);

    template <class T>
    void Parameters_GetBool(T &parameters, const wchar_t* psz);

    void ConsoleInput_CommandSystem(const wchar_t* psz);

    void InitGlobalParameters(ConsoleArgs* pconsoleArgs);

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

    void Sample_HelloWorld_PickEngine(const wchar_t* pszEngine);

    void Sample_HelloWorld_Intent();
    void Sample_HelloWorld_Intent(const wchar_t* subscriptionKey, const wchar_t* appId, const wchar_t* region);
    void Sample_HelloWorld_Subscription();
    void Sample_HelloWorld_Subscription_With_CRIS();
    void Sample_HelloWorld_Language(const wchar_t* language);

    void Sample_HelloWorld_Kws();

    int Sample_Do_Channel9();
    

private:

    std::shared_ptr<BaseAsyncRecognizer> m_recognizer;
    std::shared_ptr<SpeechRecognizer> m_speechRecognizer;
    std::shared_ptr<TranslationRecognizer> m_translationRecognizer;
    std::shared_ptr<IntentRecognizer> m_intentRecognizer;
    std::shared_ptr<Session> m_session;

    std::wstring m_subscriptionKey;
    std::wstring m_regionId;
    std::wstring m_endpointUri;

    void* m_commandSystem = nullptr;
};

int channel9();

void ch9_do_speech();
void ch9_do_speech_intermediate();
void ch9_do_speech_continuous();

void ch9_do_intent();
void ch9_do_intent_continuous();

void ch9_do_kws_speech();
void ch9_do_kws_intent();

void ch9_do_translation();
