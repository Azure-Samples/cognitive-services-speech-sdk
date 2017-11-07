#include "stdafx.h"
#include "carbon_test_console.h"
#include "speechapi_c.h"


CarbonTestConsole::CarbonTestConsole()
{
}

CarbonTestConsole::~CarbonTestConsole()
{
    TermCarbon();
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

bool CarbonTestConsole::ParseConsoleArgs(int argc, const wchar_t* argv[], ConsoleArgs* pconsoleArgs)
{
    std::wstring* pstrNextArg = nullptr;
    bool fNextArgRequired = false;
    bool fShowOptions = false;

    for (int i = 1; !fShowOptions && i < argc; i++)
    {
        const wchar_t *pszArg = argv[i];
        if (_wcsicmp(pszArg, L"/debug") == 0)
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
            pconsoleArgs->m_strRecognizerType = typeid(SpeechRecognizer).name();
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (_wcsicmp(pszArg, L"/intent") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = typeid(IntentRecognizer).name();
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (_wcsicmp(pszArg, L"/commands") == 0)
        {
            fShowOptions = (pconsoleArgs->m_strRecognizerType.length() > 0 && pconsoleArgs->m_strRecognizerType != typeid(IntentRecognizer).name()) || pconsoleArgs->m_fCommandSystem || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = typeid(IntentRecognizer).name();
            pconsoleArgs->m_fCommandSystem = true;
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

bool CarbonTestConsole::ValidateConsoleArgs(ConsoleArgs* pconsoleArgs)
{
    auto fValid = true;

    if (pconsoleArgs->m_strInput.length() > 0 && _wcsicmp(pconsoleArgs->m_strInput.c_str(), L"microphone") == 0)
    {
        pconsoleArgs->m_fMicrophoneInput = true;
        pconsoleArgs->m_strInput.clear();
    }

    if (pconsoleArgs->m_strInput.length() > 0 && _waccess(pconsoleArgs->m_strInput.c_str(), 0) != 0)
    {
        SPX_DBG_TRACE_ERROR("File does not exist: %S", pconsoleArgs->m_strInput.c_str());
        fValid = false;
    }

    if (pconsoleArgs->m_fContinuousRecognition && pconsoleArgs->m_strContinuousRecognitionSeconds.length() > 0)
    {
        auto seconds = std::stoi(pconsoleArgs->m_strContinuousRecognitionSeconds.c_str());
        pconsoleArgs->m_continuousRecognitionSeconds = std::min(std::max(seconds, 0), 30);
    }

    if (pconsoleArgs->m_fRecognizeAsync || pconsoleArgs->m_fContinuousRecognition)
    {
        if (pconsoleArgs->m_strRecognizerType.length() == 0)
        {
            pconsoleArgs->m_strRecognizerType = typeid(SpeechRecognizer).name();
        }
    }
    else if (pconsoleArgs->m_strRecognizerType.length() > 0)
    {
        pconsoleArgs->m_fRecognizeAsync = !pconsoleArgs->m_fCommandSystem && !pconsoleArgs->m_fInteractivePrompt;
    }

    return fValid;
}

void CarbonTestConsole::ProcessConsoleArgs(ConsoleArgs* pconsoleArgs)
{
    if (pconsoleArgs->m_strRunSampleName.length() > 0)
    {
        RunSample(pconsoleArgs->m_strRunSampleName);
    }

    EnsureInitCarbon(pconsoleArgs);

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
    ConsoleWriteLine(L"CarbonX - Carbon Test Console\nCopyright (C) 2017 Microsoft Corporation. All Rights Reserved.\n");
}

void CarbonTestConsole::DisplayConsoleUsage()
{
    ConsoleWriteLine(L"  carbonx [/speech | /intent | /commands] {input} {auth} {additional}");
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
    ConsoleWriteLine(L"       /debug                 Stops execution and waits (max 30s) for debugger.");
    ConsoleWriteLine(L"       /interactive           Allows interactive Carbon use via console window.");
    ConsoleWriteLine(L"");
}

void CarbonTestConsole::DisplayConsolePrompt()
{
    ConsoleWrite(L"carbonx> ");
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
    else if (_wcsnicmp(psz, L"sample ", wcslen(L"sample ")) == 0)
    {
        RunSample(psz + wcslen(L"sample "));
    }
    else if (_wcsnicmp(psz, L"recognizer ", wcslen(L"recognizer ")) == 0)
    {
        ConsoleInput_Recognizer(psz + wcslen(L"recognizer "), m_recognizer);
    }
    else if (_wcsnicmp(psz, L"speech ", wcslen(L"speech ")) == 0)
    {
        ConsoleInput_SpeechRecognizer(psz + wcslen(L"speech "), m_speechRecognizer);
    }
    else if (_wcsnicmp(psz, L"intent ", wcslen(L"intent ")) == 0)
    {
        ConsoleInput_IntentRecognizer(psz + wcslen(L"intent "), m_intentRecognizer);
    }
    else if (_wcsnicmp(psz, L"commandsystem ", wcslen(L"commandsystem ")) == 0)
    {
        ConsoleInput_CommandSystem(psz + wcslen(L"commandsystem "));
    }
    else
    {
        ConsoleWriteLine(L"\nUnknown command: '%s'.\n\nUse 'HELP' for a list of valid commands.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_Help()
{
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"COMMANDs: ");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    intent           Access methods/properties/events on the base RECOGNIZER object.");
    ConsoleWriteLine(L"    speech           Access methods/properties/events on the SPEECH recognizer object.");
    ConsoleWriteLine(L"    intent           Access methods/properties/events on the INTENT recognizer object.");
    ConsoleWriteLine(L"    commandsystem    Access methods/properties/events on the COMMAND SYSTEM object.");
    ConsoleWriteLine(L"    sample {name}    Run the sample named 'NAME'.");
    ConsoleWriteLine(L"    help {command}   Get help w/ 'recognizer', 'speech', 'intent', or 'commandsystem' commands.");
    ConsoleWriteLine(L"    exit             Exit interactive mode.");
    ConsoleWriteLine(L"");
}

void CarbonTestConsole::ConsoleInput_HelpOn(const wchar_t* psz)
{
    if (_wcsicmp(psz, L"recognizer") == 0)
    {
        ConsoleInput_HelpOnRecognizer();
    }
    else if (_wcsicmp(psz, L"speech") == 0)
    {
        ConsoleInput_HelpOnSpeech();
    }
    else if (_wcsicmp(psz, L"intent") == 0)
    {
        ConsoleInput_HelpOnIntent();
    }
    else if (_wcsicmp(psz, L"commandsystem") == 0)
    {
        ConsoleInput_HelpOnCommandSystem();
    }
}

void CarbonTestConsole::ConsoleInput_HelpOnRecognizer()
{
    ConsoleWriteLine(L"RECOGNIZER {method_command | event_command}");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"  Methods:");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    IsEnabled");
    ConsoleWriteLine(L"    Enable");
    ConsoleWriteLine(L"    Disable");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    Recognize");
    ConsoleWriteLine(L"    StartContinuous");
    ConsoleWriteLine(L"    StopContinuous");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"  Events: ");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    SessionStarted     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    SessionStopped     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    SoundStarted       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    SoundStopped       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    IntermediateResult {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    FinalResult        {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    NoMatch            {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    Canceled           {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"");
}

void CarbonTestConsole::ConsoleInput_HelpOnSpeech()
{
    ConsoleWriteLine(L"SPEECH {method_command | event_command}");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"  Methods:");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    IsEnabled");
    ConsoleWriteLine(L"    Enable");
    ConsoleWriteLine(L"    Disable");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    Recognize");
    ConsoleWriteLine(L"    StartContinuous");
    ConsoleWriteLine(L"    StopContinuous");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"  Events: ");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    SessionStarted     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    SessionStopped     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    SoundStarted       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    SoundStopped       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    IntermediateResult {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    FinalResult        {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    NoMatch            {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    Canceled           {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"");
}

void CarbonTestConsole::ConsoleInput_HelpOnIntent()
{
    ConsoleWriteLine(L"INTENT {method_command | event_command}");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"  Methods:");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    IsEnabled");
    ConsoleWriteLine(L"    Enable");
    ConsoleWriteLine(L"    Disable");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    Recognize");
    ConsoleWriteLine(L"    StartContinuous");
    ConsoleWriteLine(L"    StopContinuous");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"  Events: ");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    SessionStarted     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    SessionStopped     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    SoundStarted       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    SoundStopped       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    IntermediateResult {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    FinalResult        {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    NoMatch            {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"    Canceled           {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine(L"");
}

void CarbonTestConsole::ConsoleInput_HelpOnCommandSystem()
{
    ConsoleWriteLine(L"COMMANDSYSTEM {method_command | event_command}");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"  Methods:");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    NYI");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"  Events: ");
    ConsoleWriteLine(L"");
    ConsoleWriteLine(L"    NYI");
    ConsoleWriteLine(L"");
}

void CarbonTestConsole::ConsoleInput_Recognizer(const wchar_t* psz, std::shared_ptr<BaseAsyncRecognizer>& recognizer)
{
     if (_wcsicmp(psz, L"isenabled") == 0)
     {
         Recognizer_IsEnabled(recognizer);
     }
     else if (_wcsicmp(psz, L"enable") == 0)
     {
         Recognizer_Enable(recognizer);
     }
     else if (_wcsicmp(psz, L"disable") == 0)
     {
         Recognizer_Disable(recognizer);
     }
     else if (_wcsicmp(psz, L"recognize") == 0)
     {
         Recognizer_Recognize(recognizer);
     }
     else if (_wcsicmp(psz, L"startcontinuous") == 0)
     {
         Recognizer_StartContinuousRecognition(recognizer);
     }
     else if (_wcsicmp(psz, L"stopcontinuous") == 0)
     {
         Recognizer_StopContinuousRecognition(recognizer);
     }
     else if (_wcsnicmp(psz, L"sessionstarted ", wcslen(L"sessionstarted ")) == 0)
     {
         Recognizer_Event(psz + wcslen(L"sessionstarted "), m_recognizer->SessionStarted, Recognizer_SessionStartedHandler);
     }
     else if (_wcsnicmp(psz, L"sessionstopped ", wcslen(L"sessionstopped ")) == 0)
     {
         Recognizer_Event(psz + wcslen(L"sessionstopped "), m_recognizer->SessionStopped, Recognizer_SessionStoppedHandler);
     }
     else if (_wcsnicmp(psz, L"soundstarted ", wcslen(L"soundstarted ")) == 0)
     {
         Recognizer_Event(psz + wcslen(L"soundstarted "), m_recognizer->SoundStarted, Recognizer_SoundStartedHandler);
     }
     else if (_wcsnicmp(psz, L"soundstopped ", wcslen(L"soundstopped ")) == 0)
     {
         Recognizer_Event(psz + wcslen(L"soundstopped "), m_recognizer->SoundStopped, Recognizer_SoundStoppedHandler);
     }
     else if (_wcsnicmp(psz, L"intermediateresult ", wcslen(L"intermediateresult ")) == 0)
     {
         Recognizer_Event(psz + wcslen(L"intermediateresult "), m_recognizer->IntermediateResult, Recognizer_IntermediateResultHandler);
     }
     else if (_wcsnicmp(psz, L"finalresult ", wcslen(L"finalresult ")) == 0)
     {
         Recognizer_Event(psz + wcslen(L"finalresult "), m_recognizer->FinalResult, Recognizer_FinalResultHandler);
     }
     else if (_wcsnicmp(psz, L"nomatch ", wcslen(L"nomatch ")) == 0)
     {
         Recognizer_Event(psz + wcslen(L"nomatch "), m_recognizer->NoMatch, Recognizer_NoMatchHandler);
     }
     else if (_wcsnicmp(psz, L"canceled ", wcslen(L"canceled ")) == 0)
     {
         Recognizer_Event(psz + wcslen(L"canceled "), m_recognizer->Canceled, Recognizer_CanceledHandler);
     }
     else
     {
         ConsoleWriteLine(L"\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
     }
}

void CarbonTestConsole::ConsoleInput_SpeechRecognizer(const wchar_t* psz, std::shared_ptr<SpeechRecognizer>& speechRecognizer)
{
    if (_wcsicmp(psz, L"isenabled") == 0)
    {
        Recognizer_IsEnabled(speechRecognizer);
    }
    else if (_wcsicmp(psz, L"enable") == 0)
    {
        Recognizer_Enable(speechRecognizer);
    }
    else if (_wcsicmp(psz, L"disable") == 0)
    {
        Recognizer_Disable(speechRecognizer);
    }
    else if (_wcsicmp(psz, L"recognize") == 0)
    {
        Recognizer_Recognize(speechRecognizer);
    }
    else if (_wcsicmp(psz, L"startcontinuous") == 0)
    {
        Recognizer_StartContinuousRecognition(speechRecognizer);
    }
    else if (_wcsicmp(psz, L"stopcontinuous") == 0)
    {
        Recognizer_StopContinuousRecognition(speechRecognizer);
    }
    else if (_wcsnicmp(psz, L"sessionstarted ", wcslen(L"sessionstarted ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"sessionstarted "), m_speechRecognizer->SessionStarted, Recognizer_SessionStartedHandler);
    }
    else if (_wcsnicmp(psz, L"sessionstopped ", wcslen(L"sessionstopped ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"sessionstopped "), m_speechRecognizer->SessionStopped, Recognizer_SessionStoppedHandler);
    }
    else if (_wcsnicmp(psz, L"soundstarted ", wcslen(L"soundstarted ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"soundstarted "), m_speechRecognizer->SoundStarted, Recognizer_SoundStartedHandler);
    }
    else if (_wcsnicmp(psz, L"soundstopped ", wcslen(L"soundstopped ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"soundstopped "), m_speechRecognizer->SoundStopped, Recognizer_SoundStoppedHandler);
    }
    else if (_wcsnicmp(psz, L"intermediateresult ", wcslen(L"intermediateresult ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"intermediateresult "), m_speechRecognizer->IntermediateResult, SpeechRecognizer_IntermediateResultHandler);
    }
    else if (_wcsnicmp(psz, L"finalresult ", wcslen(L"finalresult ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"finalresult "), m_speechRecognizer->FinalResult, SpeechRecognizer_FinalResultHandler);
    }
    else if (_wcsnicmp(psz, L"nomatch ", wcslen(L"nomatch ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"nomatch "), m_speechRecognizer->NoMatch, SpeechRecognizer_NoMatchHandler);
    }
    else if (_wcsnicmp(psz, L"canceled ", wcslen(L"canceled ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"canceled "), m_speechRecognizer->Canceled, SpeechRecognizer_CanceledHandler);
    }
    else
    {
        ConsoleWriteLine(L"\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_IntentRecognizer(const wchar_t* psz, std::shared_ptr<IntentRecognizer>& intentRecognizer)
{
    if (_wcsicmp(psz, L"isenabled") == 0)
    {
        Recognizer_IsEnabled(intentRecognizer);
    }
    else if (_wcsicmp(psz, L"enable") == 0)
    {
        Recognizer_Enable(intentRecognizer);
    }
    else if (_wcsicmp(psz, L"disable") == 0)
    {
        Recognizer_Disable(intentRecognizer);
    }
    else if (_wcsicmp(psz, L"recognize") == 0)
    {
        Recognizer_Recognize(intentRecognizer);
    }
    else if (_wcsicmp(psz, L"startcontinuous") == 0)
    {
        Recognizer_StartContinuousRecognition(intentRecognizer);
    }
    else if (_wcsicmp(psz, L"stopcontinuous") == 0)
    {
        Recognizer_StopContinuousRecognition(intentRecognizer);
    }
    else if (_wcsnicmp(psz, L"sessionstarted ", wcslen(L"sessionstarted ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"sessionstarted "), m_intentRecognizer->SessionStarted, Recognizer_SessionStartedHandler);
    }
    else if (_wcsnicmp(psz, L"sessionstopped ", wcslen(L"sessionstopped ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"sessionstopped "), m_intentRecognizer->SessionStopped, Recognizer_SessionStoppedHandler);
    }
    else if (_wcsnicmp(psz, L"soundstarted ", wcslen(L"soundstarted ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"soundstarted "), m_intentRecognizer->SoundStarted, Recognizer_SoundStartedHandler);
    }
    else if (_wcsnicmp(psz, L"soundstopped ", wcslen(L"soundstopped ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"soundstopped "), m_intentRecognizer->SoundStopped, Recognizer_SoundStoppedHandler);
    }
    else if (_wcsnicmp(psz, L"intermediateresult ", wcslen(L"intermediateresult ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"intermediateresult "), m_intentRecognizer->IntermediateResult, IntentRecognizer_IntermediateResultHandler);
    }
    else if (_wcsnicmp(psz, L"finalresult ", wcslen(L"finalresult ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"finalresult "), m_intentRecognizer->FinalResult, IntentRecognizer_FinalResultHandler);
    }
    else if (_wcsnicmp(psz, L"nomatch ", wcslen(L"nomatch ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"nomatch "), m_intentRecognizer->NoMatch, IntentRecognizer_NoMatchHandler);
    }
    else if (_wcsnicmp(psz, L"canceled ", wcslen(L"canceled ")) == 0)
    {
        Recognizer_Event(psz + wcslen(L"canceled "), m_intentRecognizer->Canceled, IntentRecognizer_CanceledHandler);
    }
    else
    {
        ConsoleWriteLine(L"\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_CommandSystem(const wchar_t* psz)
{
    // TODO: ROBCH: Implement this methods
}

template <class T>
void CarbonTestConsole::Recognizer_IsEnabled(std::shared_ptr<T>& recognizer)
{
    ConsoleWrite(L"\n%S.IsEnabled == ", typeid(*recognizer.get()).name());
    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine(L"%S\n", enabled ? "true" : "false");
}

template <class T>
void CarbonTestConsole::Recognizer_Enable(std::shared_ptr<T>& recognizer)
{
    ConsoleWriteLine(L"\nEnabling %S...", typeid(*recognizer.get()).name());
    recognizer->Enable();
    ConsoleWriteLine(L"Ensabling %S... Done!\n", typeid(*recognizer.get()).name());

    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine(L"%S.IsEnabled == %S\n", typeid(*recognizer.get()).name(), enabled ? "true" : "false");
}

template <class T>
void CarbonTestConsole::Recognizer_Disable(std::shared_ptr<T>& recognizer)
{
    ConsoleWriteLine(L"\nDisabling %S...", typeid(*recognizer.get()).name());
    recognizer->Disable();
    ConsoleWriteLine(L"Disabling %S... Done!\n", typeid(*recognizer.get()).name());

    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine(L"%S.IsEnabled == %S\n", typeid(*recognizer.get()).name(), enabled ? "true" : "false");
}

template <class T>
void CarbonTestConsole::Recognizer_Recognize(std::shared_ptr<T>& recognizer)
{
    ConsoleWriteLine(L"\nRecognizeAsync %S...", typeid(*recognizer.get()).name());
    auto future = recognizer->RecognizeAsync();
    ConsoleWriteLine(L"RecognizeAsync %S... Waiting...", typeid(*recognizer.get()).name());
    future.get();
    ConsoleWriteLine(L"RecognizeAsync %S... Waiting... Done!\n", typeid(*recognizer.get()).name());
}

template <class T>
void CarbonTestConsole::Recognizer_StartContinuousRecognition(std::shared_ptr<T>& recognizer)
{
    ConsoleWriteLine(L"\nStartContinuousRecognitionAsync %S...", typeid(*recognizer.get()).name());
    auto future = recognizer->StartContinuousRecognitionAsync();
    ConsoleWriteLine(L"StartContinuousRecognitionAsync %S... Waiting...", typeid(*recognizer.get()).name());
    future.get();
    ConsoleWriteLine(L"StartContinuousRecognitionAsync %S... Waiting... Done!\n", typeid(*recognizer.get()).name());
}

template <class T>
void CarbonTestConsole::Recognizer_StopContinuousRecognition(std::shared_ptr<T>& recognizer)
{
    ConsoleWriteLine(L"\nStopContinuousRecognitionAsync %S...", typeid(*recognizer.get()).name());
    auto future = recognizer->StopContinuousRecognitionAsync();
    ConsoleWriteLine(L"StopContinuousRecognitionAsync %S... Waiting...", typeid(*recognizer.get()).name());
    future.get();
    ConsoleWriteLine(L"StopContinuousRecognitionAsync %S... Waiting... Done!\n", typeid(*recognizer.get()).name());
}

template <class T>
void CarbonTestConsole::Recognizer_Event(const wchar_t* psz, EventSignal<T>& recognizerEvent, typename::EventSignal<T>::Callback2 callback)
{
    if (_wcsicmp(psz, L"connect") == 0)
    {
        recognizerEvent.Connect(callback, this);
    }
    else if (_wcsicmp(psz, L"disconnect") == 0)
    {
        recognizerEvent.Disconnect(callback, this);
    }
    else if (_wcsicmp(psz, L"disconnectall") == 0)
    {
        recognizerEvent.DisconnectAll();
    }
    else
    {
        ConsoleWriteLine(L"\nUnknown event method: '%s'.\n\nUse 'HELP' for a list of valid commands.", psz);
    }
}

void CarbonTestConsole::EnsureInitCarbon(ConsoleArgs* pconsoleArgs)
{
    if (ShouldInitCarbon())
    {
        InitCarbon(pconsoleArgs);
    }
}

void CarbonTestConsole::TermCarbon()
{
    m_recognizer = nullptr;
    m_speechRecognizer = nullptr;
    m_intentRecognizer = nullptr;
    m_commandSystem = nullptr;
}

void CarbonTestConsole::InitCarbon(ConsoleArgs* pconsoleArgs)
{
    try
    {
        InitRecognizer(pconsoleArgs->m_strRecognizerType);
        InitCommandSystem();
    }
    catch (std::exception ex)
    {
        TermCarbon();
    }
}

void CarbonTestConsole::InitRecognizer(const std::string& recognizerType)
{
    if (recognizerType == typeid(SpeechRecognizer).name())
    {
        m_speechRecognizer = RecognizerFactory::CreateSpeechRecognizer();
        m_recognizer = BaseAsyncRecognizer::From(m_speechRecognizer);
    }
    else if (recognizerType == typeid(IntentRecognizer).name())
    {
        m_intentRecognizer = RecognizerFactory::CreateIntentRecognizer();
        m_recognizer = BaseAsyncRecognizer::From(m_intentRecognizer);
    }
}

void CarbonTestConsole::InitCommandSystem()
{
    // TODO: ROBCH: implement this method
}

void CarbonTestConsole::WaitForDebugger()
{
    PAL_WaitForDebugger();
}

void CarbonTestConsole::RecognizeAsync()
{
    if (m_intentRecognizer != nullptr)
    {
        Recognizer_Recognize(m_intentRecognizer);
    }
    else if (m_speechRecognizer != nullptr)
    {
        Recognizer_Recognize(m_speechRecognizer);
    }
    else if (m_recognizer != nullptr)
    {
        Recognizer_Recognize(m_recognizer);
    }
}

void CarbonTestConsole::ContinuousRecognition(uint16_t seconds)
{
    if (m_intentRecognizer != nullptr)
    {
        Recognizer_StartContinuousRecognition(m_intentRecognizer);

        ConsoleWrite(L"Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine(L"Done!");

        Recognizer_StopContinuousRecognition(m_intentRecognizer);
    }
    else if (m_speechRecognizer != nullptr)
    {
        Recognizer_StartContinuousRecognition(m_speechRecognizer);

        ConsoleWrite(L"Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine(L"Done!");

        Recognizer_StopContinuousRecognition(m_speechRecognizer);
    }
    else if (m_recognizer != nullptr)
    {
        Recognizer_StartContinuousRecognition(m_recognizer);

        ConsoleWrite(L"Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine(L"Done!");

        Recognizer_StopContinuousRecognition(m_recognizer);
    }
}

void CarbonTestConsole::RunSample(const std::wstring& strSampleName)
{
    if (_wcsicmp(strSampleName.c_str(), L"helloworld") == 0)
    {
        ConsoleWriteLine(L"Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld();
    }
    else if (_wcsicmp(strSampleName.c_str(), L"helloworld c") == 0)
    {
        ConsoleWriteLine(L"Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_C();
    }
    else
    {
        ConsoleWriteLine(L"\nUnknown sample: '%s'.\n", strSampleName.c_str());
    }
}

void CarbonTestConsole::RunInteractivePrompt()
{
    std::wstring strInput;
    while (GetConsoleInput(strInput))
    {
        if (_wcsicmp(strInput.c_str(), L"exit") == 0) break;
        if (_wcsicmp(strInput.c_str(), L"quit") == 0) break;

        try
        {
            ProcessConsoleInput(strInput.c_str());
        }
        catch (NotYetImplementedException e)
        {
            SPX_TRACE_ERROR("CarbonX: Not Yet Implemented!!");
        }
    }
}

void CarbonTestConsole::Sample_HelloWorld()
{
    auto recognizer = RecognizerFactory::CreateSpeechRecognizer();

    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said %s", result->Text);
}

void CarbonTestConsole::Sample_HelloWorld_C()
{
    // SPXHR hr = SPX_OK;
    // SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
    // if (SPX_SUCCEEDED(hr))
    // {
    //    hr = ::RecognizerFactory_CreateSpeechRecognzier_With_Defaults(&hreco);
    // }

    // SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    // if (SPX_SUCCEEDED(hr))
    // {
    //    ConsoleWriteLine(L"Say something...");
    //    hr = ::Recognizer_RecognizeAsync(hreco, &hasync);
    // }

    // SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
    // if (SPX_SUCCEEDED(hr))
    // {
    //    hr = ::Recognizer_RecognizeAsync_WaitFor(hasync, 30 * 1000, &hresult);
    // }

    // wchar_t text[1024];
    // if (SPX_SUCCEEDED(hr))
    // {
    //    hr = ::Result_GetText(hresult, text, sizeof(text) / sizeof(text[0]));
    // }

    // if (SPX_SUCCEEDED(hr))
    // {
    //    ConsoleWriteLine(L"You said %s", text);
    // }

    // ::Recognizer_AsyncHandle_Close(hresult);
    // hasync = SPXHANDLE_INVALID;

    // ::Recognizer_ResultHandle_Close(hresult);
    // hresult = SPXHANDLE_INVALID;

    // ::Recognizer_Handle_Close(hreco);
    // hreco = SPXHANDLE_INVALID;
}

int __cdecl wmain(int argc, const wchar_t* argv[])
{
    SPX_DBG_TRACE_SCOPE("\r\n=== CarbonX (Carbon Test Console) started\n\n", "\r\n=== CarbonX (carbon test console) terminated");

    auto unhandledException = []() ->void { SPX_TRACE_ERROR("CarbonX: Unhandled exception!"); };
    set_terminate(unhandledException);

    try
    {
        CarbonTestConsole test;
        auto result = test.Run(argc, argv);
        return result;
    }
    catch (std::exception e)
    {
        SPX_TRACE_ERROR("CarbonX: Unhandled exception on main thread! what=%s", e.what());
        exit(0);
    }
    catch (...)
    {
        SPX_TRACE_ERROR("CarbonX: Unhandled exception on main thread!");
        exit(0);
    }
}
