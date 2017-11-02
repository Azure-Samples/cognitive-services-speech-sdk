#include "stdafx.h"

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

        std::wstring m_strRecognizerType;
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
    void ConsoleInput_Recognizer(const wchar_t *psz);

    bool NeedsInit() { return true; } // return m_recognizer == nullptr; } // TODO: ROBCH: Implement this correctly
    void EnsureInit(bool fMicrophone, const std::wstring &strInputFileName, const std::wstring& strEndpointUri, const std::wstring& strSubscriptionKey);

    void Init(bool fMicrophone, const std::wstring &strInputFileName, const std::wstring& strEndpointUri, const std::wstring& strSubscriptionKey);
    void Term();

    void InitRecognizer();
    void InitCommandSystem();

    void WaitForDebugger();

    void RecognizeAsync();
    void ContinuousRecognition(uint16_t seconds);

    void RunSample(const std::wstring& strSampleName);

    void RunInteractivePrompt();

private:

    // TODO: ROBCH: declare members here ****

    // shared_ptr<Recognizer> 		 m_recognizer;
    // shared_ptr<SpeechRecognizer>  m_speechRecognizer;
    // shared_ptr<IntentRecognizer>  m_intentRecognizer;

    // shared_ptr<CommandSystem>     m_commandSystem;
};


CarbonTestConsole::CarbonTestConsole()
{
}


CarbonTestConsole::~CarbonTestConsole()
{
    Term();
}

int CarbonTestConsole::Run(int argc, const wchar_t* argv[])
{
    DisplayConsoleHeader();

    ConsoleArgs consoleArgs;
    if (!ParseConsoleArgs(argc, argv, &consoleArgs) || !ValidateConsoleArgs(&consoleArgs))
    {
        DisplayConsoleUsage();
        return -1;
    }

    ProcessConsoleArgs(&consoleArgs);

    return 0;
}

bool CarbonTestConsole::ParseConsoleArgs(int argc, const wchar_t* argv[], ConsoleArgs *pconsoleArgs)
{
    std::wstring* pstrNextArg = nullptr;
    bool fNextArgRequired = false;
    bool fShowOptions = false;

    for (int i = 1; !fShowOptions && i < argc; i++)
    {
        const wchar_t *pszArg = argv[i];
        if (_wcsicmp(pszArg, L"/debugbreak") == 0)
        {
            WaitForDebugger();
            fShowOptions = pconsoleArgs->m_fWaitForDebugger || fNextArgRequired;
            pconsoleArgs->m_fWaitForDebugger = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (_wcsicmp(pszArg, L"/speech") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = L"speech";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (_wcsicmp(pszArg, L"/intent") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = L"intent";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (_wcsicmp(pszArg, L"/commands") == 0)
        {
            fShowOptions = (pconsoleArgs->m_strRecognizerType.length() > 0 && pconsoleArgs->m_strRecognizerType != L"intent") || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = L"intent";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (_wcsnicmp(pszArg, L"/input", wcslen(L"/input")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strInput.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strInput;
            fNextArgRequired = true;
        }
        else if (_wcsnicmp(pszArg, L"/endpoint", wcslen(L"/endpoint")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strEndpointUri.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strEndpointUri;
            fNextArgRequired = true;
        }
        else if (_wcsnicmp(pszArg, L"/subscription", wcslen(L"/subscription")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strSubscriptionKey.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strSubscriptionKey;
            fNextArgRequired = true;
        }
        else if (_wcsicmp(pszArg, L"/single") == 0)
        {
            fShowOptions = pconsoleArgs->m_fContinuousRecognition || fNextArgRequired;
            pconsoleArgs->m_fRecognizeAsync = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (_wcsnicmp(pszArg, L"/continuous", wcslen(L"/continuous")) == 0)
        {
            fShowOptions = pconsoleArgs->m_fRecognizeAsync || fNextArgRequired;
            pconsoleArgs->m_fContinuousRecognition = true;
            pconsoleArgs->m_continuousRecognitionSeconds = UINT16_MAX;
            pstrNextArg = &pconsoleArgs->m_strContinuousRecognitionSeconds;
            fNextArgRequired = false;
        }
        else if (_wcsnicmp(pszArg, L"/sample", wcslen(L"/sample")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strRunSampleName.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strRunSampleName;
            fNextArgRequired = true;
        }
        else if (_wcsicmp(pszArg, L"/interactive") == 0)
        {
            fShowOptions = pconsoleArgs->m_fInteractivePrompt || fNextArgRequired;
            pconsoleArgs->m_fInteractivePrompt = true;
            pstrNextArg = NULL;
            fNextArgRequired = false;
        }
        else if (pstrNextArg != NULL)
        {
            fShowOptions = pstrNextArg->length() > 0;
            *pstrNextArg = pszArg;
            pstrNextArg = NULL;
            fNextArgRequired = false;
            continue;
        }
        else
        {
            fShowOptions = true;
        }

        if (fShowOptions) break;

        const wchar_t *pszArgInsideThisArg = wcspbrk(pszArg, L":=");
        if (pszArgInsideThisArg)
        {
            if (pstrNextArg == NULL)
            {
                fShowOptions = true;
                break;
            }

            fShowOptions = pstrNextArg->length() > 0;
            *pstrNextArg = pszArgInsideThisArg + 1;
            pstrNextArg = NULL;
            fNextArgRequired = false;
        }
    }

    return !(fShowOptions || fNextArgRequired);
}

bool CarbonTestConsole::ValidateConsoleArgs(ConsoleArgs *pconsoleArgs)
{
    auto fValid = true;

    if (pconsoleArgs->m_strInput.length() > 0 && _wcsicmp(pconsoleArgs->m_strInput.c_str(), L"microphone") == 0)
    {
        pconsoleArgs->m_fMicrophoneInput = true;
        pconsoleArgs->m_strInput.clear();
    }

    if (pconsoleArgs->m_strInput.length() > 0 && _waccess(pconsoleArgs->m_strInput.c_str(), 0) != 0)
    {
        SPX_TRACE_ERROR("File does not exist: %S", pconsoleArgs->m_strInput.c_str());
        fValid = false;
    }

    if (pconsoleArgs->m_fContinuousRecognition && pconsoleArgs->m_strContinuousRecognitionSeconds.length() > 0)
    {
        auto seconds = std::stoi(pconsoleArgs->m_strContinuousRecognitionSeconds.c_str());
        pconsoleArgs->m_continuousRecognitionSeconds = std::min(std::max(seconds, 0), 30);
    }

    return fValid;
}

void CarbonTestConsole::ProcessConsoleArgs(ConsoleArgs *pconsoleArgs)
{
    if (pconsoleArgs->m_strRunSampleName.length() > 0)
    {
        RunSample(pconsoleArgs->m_strRunSampleName);
    }

    EnsureInit(pconsoleArgs->m_fMicrophoneInput, pconsoleArgs->m_strInput, pconsoleArgs->m_strEndpointUri, pconsoleArgs->m_strSubscriptionKey);

    if (pconsoleArgs->m_fRecognizeAsync)
    {
        RecognizeAsync();
    }

    if (pconsoleArgs->m_fContinuousRecognition)
    {
        ContinuousRecognition(pconsoleArgs->m_continuousRecognitionSeconds);
    }

    if (pconsoleArgs->m_fInteractivePrompt)
    {
        RunInteractivePrompt();
    }
}

void CarbonTestConsole::DisplayConsoleHeader()
{
    ConsoleWriteLine(L"CTC - Carbon Test Console\nCopyright (C) 2017 Microsoft Corporation. All Rights Reserved.\n");
}

void CarbonTestConsole::DisplayConsoleUsage()
{
    ConsoleWriteLine(L"  ctc [/speech | /intent | /commands] {input} {auth} {additional}");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"     Input: /input:[microphone | {waveFileName}]");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"       /input:microphone      Use the default microphone for audio input.");
    ConsoleWriteLine(L"       /input:{waveFileName}  Use WAV file for audio input.");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"     Authentication:");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"       /endpoint:{uri}        Use {uri} as the USP endpoint.");
    ConsoleWriteLine(L"       /subscription:{key}    Use {key} as the subscription key.");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"     Additional:");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"       /single                Use RecognizeAsync for a single utterance.");
    ConsoleWriteLine(L"       /continuous:{seconds}  Use [Start/Stop]ContinuousRecognition, waiting");
    ConsoleWriteLine(L"                              {seconds} in between starting and stopping.");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"       /sample:{sampleName}   Run the sample named {sampleName}.");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"       /debugbreak            Stops program execution and waits for debugger.");
    ConsoleWriteLine(L"       /interactive           Allows interactive Carbon use via console window.");
    ConsoleWriteLine(L"");
}

void CarbonTestConsole::DisplayConsolePrompt()
{
    ConsoleWrite(L"> ");
}

void CarbonTestConsole::ConsoleWrite(const wchar_t* pszFormat, ...)
{
    va_list argptr;
    va_start(argptr, pszFormat);
    vfwprintf(stdout, pszFormat, argptr);
    va_end(argptr);
}

void CarbonTestConsole::ConsoleWriteLine(const wchar_t* pszFormat, ...)
{
    va_list argptr;
    va_start(argptr, pszFormat);
    vfwprintf(stdout, pszFormat, argptr);
    va_end(argptr);

    fwprintf(stdout, L"\n");
}

bool CarbonTestConsole::ConsoleReadLine(std::wstring& str)
{
    const int cchMax = 1024;
    str.reserve(cchMax);
    str[0] = L'\0';

    wchar_t* data = (wchar_t*)str.data(); // should use CX17 .data(); VC2017 works fine; might not work cross-platform
    str = fgetws(data, cchMax - 1, stdin);

    return str.length() > 0;
}

bool CarbonTestConsole::GetConsoleInput(std::wstring& str)
{
    DisplayConsolePrompt();

    bool readLine = ConsoleReadLine(str);
    auto lastLF = str.find_last_of(L'\n');

    if (lastLF != std::wstring::npos)
    {
        str.resize(lastLF);
    }

    return str.length() > 0;
}

void CarbonTestConsole::ProcessConsoleInput(const wchar_t* psz)
{
    if (_wcsicmp(psz, L"help") == 0 || _wcsicmp(psz, L"?") == 0)
    {
        ConsoleInput_Help();
    }
    else if (_wcsnicmp(psz, L"help ", wcslen(L"help ")) == 0)
    {
        ConsoleInput_HelpOn(psz + wcslen(L"help "));
    }
    else if (_wcsnicmp(psz, L"recognizer ", wcslen(L"recognizer ")) == 0)
    {
        ConsoleInput_Recognizer(psz + wcslen(L"recognizer "));
    }
    else if (_wcsnicmp(psz, L"reco ", wcslen(L"reco ")) == 0)
    {
        ConsoleInput_Recognizer(psz + wcslen(L"reco "));
    }
}

void CarbonTestConsole::ConsoleInput_Help()
{
    ConsoleWriteLine(L"HELP Not Yet Implemented...");
}

void CarbonTestConsole::ConsoleInput_HelpOn(const wchar_t* psz)
{
    ConsoleWriteLine(L"HELP on %s Not Yet Implemented...", psz);
}

void CarbonTestConsole::ConsoleInput_Recognizer(const wchar_t *psz)
{
    // TODO: ROBCH: Implement this methods

    // if (_wcsicmp(psz, L"foo") == 0)
    // {
    //     ConsoleInput_Recognizer_FooBar();
    // }
    // else if (_wcsnicmp(psz, L"foo_with_param ", wcslen(L"foo_with_param ")) == 0)
    // {
    //     ConsoleInput_Recognizer_Foo(psz + wcslen(L"foo_with_param  "));
    // }
    // else if (_wcsicmp(psz, L"baz") == 0)
    // {
    //     ConsoleInput_Recognizer_Baz();
    // }
}

void CarbonTestConsole::EnsureInit(bool fMicrophone, 
                                   const std::wstring &strInputFileName, 
                                   const std::wstring& strEndpointUri, 
                                   const std::wstring& strSubscriptionKey)
{
    if (NeedsInit())
    {
        Init(fMicrophone, strInputFileName, strEndpointUri, strSubscriptionKey);
    }
}

void CarbonTestConsole::Term()
{
    // TODO: ROBCH: implement this method

    //m_recognizer = nullptr;
    //m_speechRecognizer = nullptr;
    //m_intentRecognizer = nullptr;
    //m_commandSystem = nullptr;
}

void CarbonTestConsole::Init(bool fMicrophone, const std::wstring &strInputFileName, const std::wstring& strEndpointUri, const std::wstring& strSubscriptionKey)
{
    try
    {
        InitRecognizer();
        InitCommandSystem();
    }
    catch (std::exception ex)
    {
        Term();
    }
}

void CarbonTestConsole::InitRecognizer()
{
    // TODO: ROBCH: implement this method
}

void CarbonTestConsole::InitCommandSystem()
{
    // TODO: ROBCH: implement this method
}

void CarbonTestConsole::WaitForDebugger()
{
    // TODO: ROBCH: Implement this methods
    PAL_DebugBreak();
    
    //uint16_t msWaitForDebugger = 30 * 1000;
    //uint16_t msSleepPeriod = 100;
    //bool fDebugBreak = false;

    //for(DWORD dwTickStart = GetTickCount(); GetTickCount() - dwTickStart < msWaitForDebugger; Sleep(msSleepPeriod))
    //{
    //    if (PAL_IsDebuggerPresent())
    //    {
    //        fDebugBreak = TRUE;
    //        break;
    //    }
    //}

    //if (fDebugBreak)
    //{
    //    PAL_DebugBreak();
    //}
}

void CarbonTestConsole::RecognizeAsync()
{
    SPX_TRACE_INFO("CTC: RecognizeAsync...");

    // TODO: ROBCH: implement this method

    SPX_TRACE_INFO("CTC: RecognizeAsync... Done!");
}

void CarbonTestConsole::ContinuousRecognition(uint16_t seconds)
{
    SPX_TRACE_INFO("CTC: ContinuousRecognition...");

    // TODO: ROBCH: implement this method

    SPX_TRACE_INFO("CTC: ContinuousRecognition... Done!");
}

void CarbonTestConsole::RunSample(const std::wstring& strSampleName)
{
    SPX_TRACE_INFO("CTC: RunSample('%S')...", strSampleName.c_str());

    // TODO: ROBCH: implement this method
    throw new std::exception("hmm... we shoudl do something about that");

    SPX_TRACE_INFO("CTC: RunSample('%S')... Done!", strSampleName.c_str());
}

void CarbonTestConsole::RunInteractivePrompt()
{
    std::wstring strInput;
    ConsoleWriteLine(L"Enter you command (? for help, EXIT to exit)...");
    while (GetConsoleInput(strInput))
    {
        if (_wcsicmp(strInput.c_str(), L"exit") == 0) break;
        if (_wcsicmp(strInput.c_str(), L"quit") == 0) break;
        ProcessConsoleInput(strInput.c_str());
    }
}

int __cdecl wmain(int argc, const wchar_t* argv[])
{
    SPX_DBG_TRACE_SCOPE("CTC.exe (Carbon Test Console) started", "CTC.exe terminated");

    auto unhandledException = []() ->void { SPX_TRACE_ERROR("CTC.exe: Unhandled exception!"); };
    set_terminate(unhandledException);

    try
    {
        CarbonTestConsole test;
        auto result = test.Run(argc, argv);
        return result;
    }
    catch (std::exception e)
    {
        SPX_TRACE_ERROR("CTC.exe: Unhandled exception on main thread! what=%s", e.what());
        exit(0);
    }
    catch (...)
    {
        SPX_TRACE_ERROR("CTC.exe: Unhandled exception on main thread!");
        exit(0);
    }
}
