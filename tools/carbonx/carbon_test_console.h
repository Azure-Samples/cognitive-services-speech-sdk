#pragma once

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

    bool ParseConsoleArgs(int argc, const wchar_t* argv[], ConsoleArgs *pconsoleArgs);
    bool ValidateConsoleArgs(ConsoleArgs *pconsoleArgs);
    void ProcessConsoleArgs(ConsoleArgs *pconsoleArgs);

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

    void ConsoleInput_Recognizer(const wchar_t *psz);
    void ConsoleInput_Speech(const wchar_t *psz);
    void ConsoleInput_Intent(const wchar_t *psz);
    void ConsoleInput_CommandSystem(const wchar_t *psz);

    bool ShouldInitCarbon() { return m_recognizer == nullptr; }
    void EnsureInitCarbon(ConsoleArgs *pconsoleArgs);

    void InitCarbon(ConsoleArgs *pconsoleArgs);
    void TermCarbon();

    void InitRecognizer(const std::string& recognizerType);
    void InitCommandSystem();

    void WaitForDebugger();

    void RecognizeAsync();
    void ContinuousRecognition(uint16_t seconds);

    void RunSample(const std::wstring& strSampleName);

    void RunInteractivePrompt();

    void Sample_HelloWorld();

private:

    std::shared_ptr<BaseAsyncRecognizer> m_recognizer;
    std::shared_ptr<SpeechRecognizer> m_speechRecognizer;
    std::shared_ptr<IntentRecognizer> m_intentRecognizer;
    void* m_commandSystem = nullptr;
};
