//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_test_console.h: Definitions for the CarbonTestConsole tool C++ class
//

#include "stdafx.h"
#include "carbon_test_console.h"
#include "speechapi_c.h"
#include <string_utils.h>
#include <file_utils.h>
#include <platform.h>

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
        if (PAL::wcsicmp(pszArg, L"/debug") == 0)
        {
            WaitForDebugger();
            fShowOptions = pconsoleArgs->m_fWaitForDebugger || fNextArgRequired;
            pconsoleArgs->m_fWaitForDebugger = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::wcsicmp(pszArg, L"/speech") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<SpeechRecognizer>();
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::wcsicmp(pszArg, L"/intent") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<IntentRecognizer>();
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::wcsicmp(pszArg, L"/commands") == 0)
        {
            fShowOptions = (pconsoleArgs->m_strRecognizerType.length() > 0 && pconsoleArgs->m_strRecognizerType != PAL::GetTypeName<IntentRecognizer>()) || pconsoleArgs->m_fCommandSystem || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<IntentRecognizer>();
            pconsoleArgs->m_fCommandSystem = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::wcsnicmp(pszArg, L"/input", wcslen(L"/input")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strInput.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strInput;
            fNextArgRequired = true;
        }
        else if (PAL::wcsnicmp(pszArg, L"/endpoint", wcslen(L"/endpoint")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strEndpointUri.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strEndpointUri;
            fNextArgRequired = true;
        }
        else if (PAL::wcsnicmp(pszArg, L"/subscription", wcslen(L"/subscription")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strSubscriptionKey.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strSubscriptionKey;
            fNextArgRequired = true;
        }
        else if (PAL::wcsicmp(pszArg, L"/single") == 0)
        {
            fShowOptions = pconsoleArgs->m_fContinuousRecognition || fNextArgRequired;
            pconsoleArgs->m_fRecognizeAsync = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::wcsnicmp(pszArg, L"/continuous", wcslen(L"/continuous")) == 0)
        {
            fShowOptions = pconsoleArgs->m_fRecognizeAsync || fNextArgRequired;
            pconsoleArgs->m_fContinuousRecognition = true;
            pconsoleArgs->m_continuousRecognitionSeconds = UINT16_MAX;
            pstrNextArg = &pconsoleArgs->m_strContinuousRecognitionSeconds;
            fNextArgRequired = false;
        }
        else if (PAL::wcsnicmp(pszArg, L"/sample", wcslen(L"/sample")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strRunSampleName.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strRunSampleName;
            fNextArgRequired = true;
        }
        else if (PAL::wcsicmp(pszArg, L"/interactive") == 0)
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

    if (pconsoleArgs->m_strInput.length() > 0 && PAL::wcsicmp(pconsoleArgs->m_strInput.c_str(), L"microphone") == 0)
    {
        pconsoleArgs->m_fMicrophoneInput = true;
        pconsoleArgs->m_strInput.clear();
    }

    if (pconsoleArgs->m_strInput.length() > 0 && PAL::waccess(pconsoleArgs->m_strInput.c_str(), 0) != 0)
    {
        SPX_DBG_TRACE_ERROR("File does not exist: %ls", pconsoleArgs->m_strInput.c_str());
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
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<SpeechRecognizer>();
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
    std::wstring format(pszFormat);
    format += L"\n";

    va_list argptr;
    va_start(argptr, pszFormat);
    vfwprintf(stdout, format.c_str(), argptr);
    va_end(argptr);
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
    if (PAL::wcsicmp(psz, L"help") == 0 || PAL::wcsicmp(psz, L"?") == 0)
    {
        ConsoleInput_Help();
    }
    else if (PAL::wcsnicmp(psz, L"help ", wcslen(L"help ")) == 0)
    {
        ConsoleInput_HelpOn(psz + wcslen(L"help "));
    }
    else if (PAL::wcsnicmp(psz, L"sample ", wcslen(L"sample ")) == 0)
    {
        RunSample(psz + wcslen(L"sample "));
    }
    else if (PAL::wcsnicmp(psz, L"recognizer ", wcslen(L"recognizer ")) == 0)
    {
        ConsoleInput_Recognizer(psz + wcslen(L"recognizer "), m_recognizer);
    }
    else if (PAL::wcsnicmp(psz, L"speech ", wcslen(L"speech ")) == 0)
    {
        ConsoleInput_SpeechRecognizer(psz + wcslen(L"speech "), m_speechRecognizer);
    }
    else if (PAL::wcsnicmp(psz, L"intent ", wcslen(L"intent ")) == 0)
    {
        ConsoleInput_IntentRecognizer(psz + wcslen(L"intent "), m_intentRecognizer);
    }
    else if (PAL::wcsnicmp(psz, L"commandsystem ", wcslen(L"commandsystem ")) == 0)
    {
        ConsoleInput_CommandSystem(psz + wcslen(L"commandsystem "));
    }
    else
    {
        ConsoleWriteLine(L"\nUnknown command: '%ls'.\n\nUse 'HELP' for a list of valid commands.\n", psz);
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
    if (PAL::wcsicmp(psz, L"recognizer") == 0)
    {
        ConsoleInput_HelpOnRecognizer();
    }
    else if (PAL::wcsicmp(psz, L"speech") == 0)
    {
        ConsoleInput_HelpOnSpeech();
    }
    else if (PAL::wcsicmp(psz, L"intent") == 0)
    {
        ConsoleInput_HelpOnIntent();
    }
    else if (PAL::wcsicmp(psz, L"commandsystem") == 0)
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
     if (PAL::wcsicmp(psz, L"isenabled") == 0)
     {
         Recognizer_IsEnabled(recognizer);
     }
     else if (PAL::wcsicmp(psz, L"enable") == 0)
     {
         Recognizer_Enable(recognizer);
     }
     else if (PAL::wcsicmp(psz, L"disable") == 0)
     {
         Recognizer_Disable(recognizer);
     }
     else if (PAL::wcsicmp(psz, L"recognize") == 0)
     {
         Recognizer_Recognize(recognizer);
     }
     else if (PAL::wcsicmp(psz, L"startcontinuous") == 0)
     {
         Recognizer_StartContinuousRecognition(recognizer);
     }
     else if (PAL::wcsicmp(psz, L"stopcontinuous") == 0)
     {
         Recognizer_StopContinuousRecognition(recognizer);
     }
     else if (PAL::wcsnicmp(psz, L"sessionstarted ", wcslen(L"sessionstarted ")) == 0)
     {
         auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStartedHandler, this, std::placeholders::_1);
         Recognizer_Event(psz + wcslen(L"sessionstarted "), m_recognizer->SessionStarted, fn);
     }
     else if (PAL::wcsnicmp(psz, L"sessionstopped ", wcslen(L"sessionstopped ")) == 0)
     {
         auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStoppedHandler, this, std::placeholders::_1);
         Recognizer_Event(psz + wcslen(L"sessionstopped "), m_recognizer->SessionStopped, fn);
     }
     else if (PAL::wcsnicmp(psz, L"soundstarted ", wcslen(L"soundstarted ")) == 0)
     {
         auto fn = std::bind(&CarbonTestConsole::Recognizer_SoundStartedHandler, this, std::placeholders::_1);
         Recognizer_Event(psz + wcslen(L"soundstarted "), m_recognizer->SoundStarted, fn);
     }
     else if (PAL::wcsnicmp(psz, L"soundstopped ", wcslen(L"soundstopped ")) == 0)
     {
         auto fn = std::bind(&CarbonTestConsole::Recognizer_SoundStoppedHandler, this, std::placeholders::_1);
         Recognizer_Event(psz + wcslen(L"soundstopped "), m_recognizer->SoundStopped, fn);
     }
     else if (PAL::wcsnicmp(psz, L"intermediateresult ", wcslen(L"intermediateresult ")) == 0)
     {
         auto fn = std::bind(&CarbonTestConsole::Recognizer_IntermediateResultHandler, this, std::placeholders::_1);
         Recognizer_Event(psz + wcslen(L"intermediateresult "), m_recognizer->IntermediateResult, fn);
     }
     else if (PAL::wcsnicmp(psz, L"finalresult ", wcslen(L"finalresult ")) == 0)
     {
         auto fn = std::bind(&CarbonTestConsole::Recognizer_FinalResultHandler, this, std::placeholders::_1);
         Recognizer_Event(psz + wcslen(L"finalresult "), m_recognizer->FinalResult, fn);
     }
     else if (PAL::wcsnicmp(psz, L"nomatch ", wcslen(L"nomatch ")) == 0)
     {
         auto fn = std::bind(&CarbonTestConsole::Recognizer_NoMatchHandler, this, std::placeholders::_1);
         Recognizer_Event(psz + wcslen(L"nomatch "), m_recognizer->NoMatch, fn);
     }
     else if (PAL::wcsnicmp(psz, L"canceled ", wcslen(L"canceled ")) == 0)
     {
         auto fn = std::bind(&CarbonTestConsole::Recognizer_CanceledHandler, this, std::placeholders::_1);
         Recognizer_Event(psz + wcslen(L"canceled "), m_recognizer->Canceled, fn);
     }
     else
     {
         ConsoleWriteLine(L"\nUnknown method/event: '%ls'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
     }
}

void CarbonTestConsole::ConsoleInput_SpeechRecognizer(const wchar_t* psz, std::shared_ptr<SpeechRecognizer>& speechRecognizer)
{
    if (PAL::wcsicmp(psz, L"isenabled") == 0)
    {
        Recognizer_IsEnabled(speechRecognizer);
    }
    else if (PAL::wcsicmp(psz, L"enable") == 0)
    {
        Recognizer_Enable(speechRecognizer);
    }
    else if (PAL::wcsicmp(psz, L"disable") == 0)
    {
        Recognizer_Disable(speechRecognizer);
    }
    else if (PAL::wcsicmp(psz, L"recognize") == 0)
    {
        Recognizer_Recognize(speechRecognizer);
    }
    else if (PAL::wcsicmp(psz, L"startcontinuous") == 0)
    {
        Recognizer_StartContinuousRecognition(speechRecognizer);
    }
    else if (PAL::wcsicmp(psz, L"stopcontinuous") == 0)
    {
        Recognizer_StopContinuousRecognition(speechRecognizer);
    }
    else if (PAL::wcsnicmp(psz, L"sessionstarted ", wcslen(L"sessionstarted ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStartedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"sessionstarted "), m_speechRecognizer->SessionStarted, fn);
    }
    else if (PAL::wcsnicmp(psz, L"sessionstopped ", wcslen(L"sessionstopped ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStoppedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"sessionstopped "), m_speechRecognizer->SessionStopped, fn);
    }
    else if (PAL::wcsnicmp(psz, L"soundstarted ", wcslen(L"soundstarted ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SoundStartedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"soundstarted "), m_speechRecognizer->SoundStarted, fn);
    }
    else if (PAL::wcsnicmp(psz, L"soundstopped ", wcslen(L"soundstopped ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SoundStoppedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"soundstopped "), m_speechRecognizer->SoundStopped, fn);
    }
    else if (PAL::wcsnicmp(psz, L"intermediateresult ", wcslen(L"intermediateresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::SpeechRecognizer_IntermediateResultHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"intermediateresult "), m_speechRecognizer->IntermediateResult, fn);
    }
    else if (PAL::wcsnicmp(psz, L"finalresult ", wcslen(L"finalresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::SpeechRecognizer_FinalResultHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"finalresult "), m_speechRecognizer->FinalResult, fn);
    }
    else if (PAL::wcsnicmp(psz, L"nomatch ", wcslen(L"nomatch ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::SpeechRecognizer_NoMatchHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"nomatch "), m_speechRecognizer->NoMatch, fn);
    }
    else if (PAL::wcsnicmp(psz, L"canceled ", wcslen(L"canceled ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::SpeechRecognizer_CanceledHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"canceled "), m_speechRecognizer->Canceled, fn);
    }
    else
    {
        ConsoleWriteLine(L"\nUnknown method/event: '%ls'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_IntentRecognizer(const wchar_t* psz, std::shared_ptr<IntentRecognizer>& intentRecognizer)
{
    if (PAL::wcsicmp(psz, L"isenabled") == 0)
    {
        Recognizer_IsEnabled(intentRecognizer);
    }
    else if (PAL::wcsicmp(psz, L"enable") == 0)
    {
        Recognizer_Enable(intentRecognizer);
    }
    else if (PAL::wcsicmp(psz, L"disable") == 0)
    {
        Recognizer_Disable(intentRecognizer);
    }
    else if (PAL::wcsicmp(psz, L"recognize") == 0)
    {
        Recognizer_Recognize(intentRecognizer);
    }
    else if (PAL::wcsicmp(psz, L"startcontinuous") == 0)
    {
        Recognizer_StartContinuousRecognition(intentRecognizer);
    }
    else if (PAL::wcsicmp(psz, L"stopcontinuous") == 0)
    {
        Recognizer_StopContinuousRecognition(intentRecognizer);
    }
    else if (PAL::wcsnicmp(psz, L"sessionstarted ", wcslen(L"sessionstarted ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStartedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"sessionstarted "), m_intentRecognizer->SessionStarted, fn);
    }
    else if (PAL::wcsnicmp(psz, L"sessionstopped ", wcslen(L"sessionstopped ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStoppedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"sessionstopped "), m_intentRecognizer->SessionStopped, fn);
    }
    else if (PAL::wcsnicmp(psz, L"soundstarted ", wcslen(L"soundstarted ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SoundStartedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"soundstarted "), m_intentRecognizer->SoundStarted, fn);
    }
    else if (PAL::wcsnicmp(psz, L"soundstopped ", wcslen(L"soundstopped ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SoundStoppedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"soundstopped "), m_intentRecognizer->SoundStopped, fn);
    }
    else if (PAL::wcsnicmp(psz, L"intermediateresult ", wcslen(L"intermediateresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::IntentRecognizer_IntermediateResultHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"intermediateresult "), m_intentRecognizer->IntermediateResult, fn);
    }
    else if (PAL::wcsnicmp(psz, L"finalresult ", wcslen(L"finalresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::IntentRecognizer_FinalResultHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"finalresult "), m_intentRecognizer->FinalResult, fn);
    }
    else if (PAL::wcsnicmp(psz, L"nomatch ", wcslen(L"nomatch ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::IntentRecognizer_NoMatchHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"nomatch "), m_intentRecognizer->NoMatch, fn);
    }
    else if (PAL::wcsnicmp(psz, L"canceled ", wcslen(L"canceled ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::IntentRecognizer_CanceledHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + wcslen(L"canceled "), m_intentRecognizer->Canceled, fn);
    }
    else
    {
        ConsoleWriteLine(L"\nUnknown method/event: '%ls'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_CommandSystem(const wchar_t* psz)
{
    // TODO: ROBCH: Implement CarbonTestConsole::ConsoleInput_CommandSystem
}

template <class T>
void CarbonTestConsole::Recognizer_IsEnabled(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::ToWString(PAL::GetTypeName(*recognizer.get()));
    ConsoleWrite(L"\n%ls.IsEnabled == ", name.c_str());
    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine(L"%ls\n", enabled ? L"true" : L"false");
}

template <class T>
void CarbonTestConsole::Recognizer_Enable(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::ToWString(PAL::GetTypeName(*recognizer.get()));
    ConsoleWriteLine(L"\nEnabling %ls...", name.c_str());
    recognizer->Enable();
    ConsoleWriteLine(L"Enabling %ls... Done!\n", name.c_str());

    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine(L"%ls.IsEnabled == %ls\n", name.c_str(), enabled ? L"true" : L"false");
}

template <class T>
void CarbonTestConsole::Recognizer_Disable(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::ToWString(PAL::GetTypeName(*recognizer.get()));
    ConsoleWriteLine(L"\nDisabling %ls...", name.c_str());
    recognizer->Disable();
    ConsoleWriteLine(L"Disabling %ls... Done!\n", name.c_str());

    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine(L"%ls.IsEnabled == %ls\n", name.c_str(), enabled ? L"true" : L"false");
}

template <class T>
void CarbonTestConsole::Recognizer_Recognize(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::ToWString(PAL::GetTypeName(*recognizer.get()));
    ConsoleWriteLine(L"\nRecognizeAsync %ls...", name.c_str());
    auto future = recognizer->RecognizeAsync();
    ConsoleWriteLine(L"RecognizeAsync %ls... Waiting...", name.c_str());
    auto result = future.get();
    ConsoleWriteLine(L"RecognizeAsync %ls... Waiting... Done!\n", name.c_str());
}

void CarbonTestConsole::Recognizer_Recognize(std::shared_ptr<SpeechRecognizer>& recognizer)
{
    auto name = PAL::ToWString(PAL::GetTypeName(*recognizer.get()));
    ConsoleWriteLine(L"\nRecognizeAsync %ls...", name.c_str());
    auto future = recognizer->RecognizeAsync();
    ConsoleWriteLine(L"RecognizeAsync %ls... Waiting...", name.c_str());
    auto result = future.get();
    ConsoleWriteLine(L"RecognizeAsync %ls... Waiting... Done!\n", name.c_str());

    ConsoleWriteLine(L"SpeechRecognitionResult: ResultId=%d; Reason=%d; Text=%ls", result->ResultId.c_str(), result->Reason, result->Text.c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_StartContinuousRecognition(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::ToWString(PAL::GetTypeName(*recognizer.get()));
    ConsoleWriteLine(L"\nStartContinuousRecognitionAsync %ls...", name.c_str());
    auto future = recognizer->StartContinuousRecognitionAsync();
    ConsoleWriteLine(L"StartContinuousRecognitionAsync %ls... Waiting...", name.c_str());
    future.get();
    ConsoleWriteLine(L"StartContinuousRecognitionAsync %ls... Waiting... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_StopContinuousRecognition(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::ToWString(PAL::GetTypeName(*recognizer.get()));
    ConsoleWriteLine(L"\nStopContinuousRecognitionAsync %ls...", name.c_str());
    auto future = recognizer->StopContinuousRecognitionAsync();
    ConsoleWriteLine(L"StopContinuousRecognitionAsync %ls... Waiting...", name.c_str());
    future.get();
    ConsoleWriteLine(L"StopContinuousRecognitionAsync %ls... Waiting... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_Event(const wchar_t* psz, EventSignal<T>& recognizerEvent, typename::EventSignal<T>::CallbackFunction callback)
{
    if (PAL::wcsicmp(psz, L"connect") == 0)
    {
        recognizerEvent.Connect(callback);
    }
    else if (PAL::wcsicmp(psz, L"disconnect") == 0)
    {
        recognizerEvent.Disconnect(callback);
    }
    else if (PAL::wcsicmp(psz, L"disconnectall") == 0)
    {
        recognizerEvent.DisconnectAll();
    }
    else
    {
        ConsoleWriteLine(L"\nUnknown event method: '%ls'.\n\nUse 'HELP' for a list of valid commands.", psz);
    }
}

std::wstring CarbonTestConsole::ToString(const SpeechRecognitionEventArgs& e)
{
    static_assert(0 == (int)Reason::Recognized, "Reason::* enum values changed!");
    static_assert(1 == (int)Reason::IntermediateResult, "Reason::* enum values changed!");
    static_assert(2 == (int)Reason::NoMatch, "Reason::* enum values changed!");
    static_assert(3 == (int)Reason::Canceled, "Reason::* enum values changed!");
    static_assert(4 == (int)Reason::OtherRecognizer, "Reason::* enum values changed!");
    
    std::wstring reasons[] = {
        L"Recognized",
        L"IntermediateResult",
        L"NoMatch",
        L"Canceled",
        L"OtherRecognizer"
    };

    std::wstring str;
    str += L"SpeechRecognitionEventArgs = { \n";
    str += L"  SessionId = '" + e.SessionId + L"'\n";
    str += L"  Result = {\n";
    str += L"    ResultId = '" + e.Result.ResultId + L"'\n";
    str += L"    Reason = Reason::" + reasons[(int)e.Result.Reason] + L"\n";
    str += L"    Text = '" + e.Result.Text + L"'\n";
    str += L"  } \n";
    str += L"} \n";

    return str;
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
        InitRecognizer(pconsoleArgs->m_strRecognizerType, pconsoleArgs->m_strInput);
        InitCommandSystem();
    }
    catch (std::exception ex)
    {
        TermCarbon();
    }
}

void CarbonTestConsole::InitRecognizer(const std::string& recognizerType, const std::wstring& wavFileName)
{
    if (recognizerType == PAL::GetTypeName<SpeechRecognizer>())
    {
        m_speechRecognizer = wavFileName.length() == 0
            ? RecognizerFactory::CreateSpeechRecognizer() 
            : RecognizerFactory::CreateSpeechRecognizerWithFileInput(wavFileName);

        auto fn1 = std::bind(&CarbonTestConsole::SpeechRecognizer_FinalResultHandler, this, std::placeholders::_1);
        m_speechRecognizer->FinalResult.Connect(fn1);

        auto fn2 = std::bind(&CarbonTestConsole::SpeechRecognizer_IntermediateResultHandler, this, std::placeholders::_1);
        m_speechRecognizer->IntermediateResult.Connect(fn2);

        m_recognizer = BaseAsyncRecognizer::From(m_speechRecognizer);
    }
    else if (recognizerType == PAL::GetTypeName<IntentRecognizer>())
    {
        m_intentRecognizer = wavFileName.length() == 0
            ? RecognizerFactory::CreateIntentRecognizer()
            : RecognizerFactory::CreateIntentRecognizerWithFileInput(wavFileName);
        m_recognizer = BaseAsyncRecognizer::From(m_intentRecognizer);
    }
}

void CarbonTestConsole::InitCommandSystem()
{
    // TODO: ROBCH: Implement CarbonTestConsole::InitCommandSystem
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
    if (PAL::wcsicmp(strSampleName.c_str(), L"helloworld") == 0)
    {
        ConsoleWriteLine(L"Running sample: %ls\n", strSampleName.c_str());
        Sample_HelloWorld();
    }
    else if (PAL::wcsicmp(strSampleName.c_str(), L"helloworld with events") == 0)
    {
        ConsoleWriteLine(L"Running sample: %ls\n", strSampleName.c_str());
        Sample_HelloWorld_WithEvents();
    }
    else if (PAL::wcsicmp(strSampleName.c_str(), L"helloworld c") == 0)
    {
        ConsoleWriteLine(L"Running sample: %ls\n", strSampleName.c_str());
        Sample_HelloWorld_In_C();
    }
    else
    {
        ConsoleWriteLine(L"\nUnknown sample: '%ls'.\n", strSampleName.c_str());
    }
}

void CarbonTestConsole::RunInteractivePrompt()
{
    std::wstring strInput;
    while (GetConsoleInput(strInput))
    {
        if (PAL::wcsicmp(strInput.c_str(), L"exit") == 0) break;
        if (PAL::wcsicmp(strInput.c_str(), L"quit") == 0) break;

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

// TODO: why is cdecl needed here?
int /*__cdecl*/ wmain(int argc, const wchar_t* argv[])
{
    SPX_DBG_TRACE_SCOPE("\r\n=== CarbonX (Carbon Test Console) started\n\n", "\r\n=== CarbonX (carbon test console) ended");

    auto unhandledException = []() ->void { SPX_TRACE_ERROR("CarbonX: Unhandled exception!"); };
    std::set_terminate(unhandledException);

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

#ifndef _MSC_VER
#include <cstring>
#include <cassert>
/// UNIX main function converts arguments in UTF-8 encoding and passes to Visual-Studio style wmain() which takes wchar_t strings.
int main(int argc, char* argv[])
{
    // TODO: change to STL containers
    wchar_t** wargs = new wchar_t*[argc];
    for (int i = 0; i < argc; ++i)
    {
        wargs[i] = new wchar_t[strlen(argv[i]) + 1];
        size_t ans = ::mbstowcs(wargs[i], argv[i], strlen(argv[i]) + 1);
        assert(ans == strlen(argv[i]));
    }
    int ret = wmain(argc, const_cast<const wchar_t**>(wargs));
    for (int i = 0; i < argc; ++i)
        delete[] wargs[i];
    delete[] wargs;
    return ret;
}
#endif