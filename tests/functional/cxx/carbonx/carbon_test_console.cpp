//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_test_console.cpp: Definitions for the CarbonTestConsole tool C++ class
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdafx.h"
#include "speechapi_c.h"
#include "mock_controller.h"
#include <chrono>
#include <string>
#include <string_utils.h>
#include <file_utils.h>
#include <platform.h>


using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace Microsoft::CognitiveServices::Speech::Translation;

CarbonTestConsole::CarbonTestConsole()
{
}

CarbonTestConsole::~CarbonTestConsole()
{
    TermCarbon();
}

int CarbonTestConsole::Run(const std::vector<std::string>& args)
{
    DisplayConsoleHeader();

    ConsoleArgs consoleArgs;
    if (!ParseConsoleArgs(args, &consoleArgs) || !ValidateConsoleArgs(&consoleArgs))
    {
        DisplayConsoleUsage();
        return -1;
    }

    ProcessConsoleArgs(&consoleArgs);

    return 0;
}

bool CarbonTestConsole::ParseConsoleArgs(const std::vector<std::string>& args, ConsoleArgs* pconsoleArgs)
{
    std::string* pstrNextArg = nullptr;
    bool fNextArgRequired = false;
    bool fShowOptions = false;

    for (size_t i = 1; !fShowOptions && i < args.size(); i++)
    {
        const char *pszArg = args[i].c_str();
        if (PAL::stricmp(pszArg, "--debug") == 0)
        {
            WaitForDebugger();
            fShowOptions = pconsoleArgs->m_fWaitForDebugger || fNextArgRequired;
            pconsoleArgs->m_fWaitForDebugger = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--speech") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<SpeechRecognizer>();
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--intent") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<IntentRecognizer>();
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--translation") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<TranslationRecognizer>();
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--commands") == 0)
        {
            fShowOptions = (pconsoleArgs->m_strRecognizerType.length() > 0 && pconsoleArgs->m_strRecognizerType != PAL::GetTypeName<IntentRecognizer>()) || pconsoleArgs->m_fCommandSystem || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<IntentRecognizer>();
            pconsoleArgs->m_fCommandSystem = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::strnicmp(pszArg, "--input", strlen("--input")) == 0)
        {
            fShowOptions = pconsoleArgs->m_audioInput.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_audioInput;
            fNextArgRequired = true;
        }
        else if (PAL::stricmp(pszArg, "--mockmicrophone") == 0)
        {
            fShowOptions = pconsoleArgs->m_audioInput.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_audioInput = "mockmicrophone";
            pstrNextArg = &pconsoleArgs->m_strMockMicrophoneRealTimePercentage;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--mockwavfile") == 0)
        {
            fShowOptions = pconsoleArgs->m_mockWavFileName.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_mockWavFileName;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--kwsTable", strlen("--kwsTable")) == 0)
        {
            fShowOptions = pconsoleArgs->m_kwsTable.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_kwsTable;
            fNextArgRequired = true;
        }
        else if (PAL::stricmp(pszArg, "--mockkws") == 0)
        {
            fShowOptions = pconsoleArgs->m_useMockKws || fNextArgRequired;
            pconsoleArgs->m_useMockKws = true;
            fNextArgRequired = false;
        }
        else if (PAL::strnicmp(pszArg, "--offlineModelPathRoot", strlen("--offlineModelPathRoot")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strOfflineModelPathRoot.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strOfflineModelPathRoot;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--offlineModelLanguage", strlen("--offlineModelLanguage")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strOfflineModelLanguage.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strOfflineModelLanguage;
            fNextArgRequired = true;
        }
        else if (PAL::stricmp(pszArg, "--mockrecoengine") == 0)
        {
            fShowOptions = pconsoleArgs->m_strUseRecoEngineProperty.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strUseRecoEngineProperty = "CARBON-INTERNAL-UseRecoEngine-Mock";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--usp") == 0)
        {
            fShowOptions = pconsoleArgs->m_strUseRecoEngineProperty.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strUseRecoEngineProperty = "CARBON-INTERNAL-UseRecoEngine-Usp";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--luisdirect") == 0)
        {
            fShowOptions = pconsoleArgs->m_strUseLuEngineProperty.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strUseLuEngineProperty = "CARBON-INTERNAL-UseLuEngine-LuisDirect";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--mockluengine") == 0)
        {
            fShowOptions = pconsoleArgs->m_strUseLuEngineProperty.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strUseLuEngineProperty = "CARBON-INTERNAL-UseLuEngine-Mock";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::strnicmp(pszArg, "--endpoint", strlen("--endpoint")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strEndpointUri.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strEndpointUri;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--customSpeechModelId", strlen("--customSpeechModelId")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strCustomSpeechModelId.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strCustomSpeechModelId;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--subscription", strlen("--subscription")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strSubscriptionKey.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strSubscriptionKey;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--region", strlen("--region")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strRegion.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strRegion;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--intentAppId", strlen("--intentAppId")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strIntentAppId.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strIntentAppId;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--intentNames", strlen("--intentNames")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strIntentNames.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strIntentNames;
            fNextArgRequired = true;
        }
        else if (PAL::stricmp(pszArg, "--single") == 0)
        {
            fShowOptions = pconsoleArgs->m_fContinuousRecognition || fNextArgRequired;
            pconsoleArgs->m_fRecognizeOnceAsync = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::strnicmp(pszArg, "--continuous", strlen("--continuous")) == 0)
        {
            fShowOptions = pconsoleArgs->m_fRecognizeOnceAsync || fNextArgRequired;
            pconsoleArgs->m_fContinuousRecognition = true;
            pconsoleArgs->m_continuousRecognitionSeconds = UINT16_MAX;
            pstrNextArg = &pconsoleArgs->m_strContinuousRecognitionSeconds;
            fNextArgRequired = false;
        }
        else if (PAL::strnicmp(pszArg, "--times", strlen("--times")) == 0)
        {
            fShowOptions = fNextArgRequired;
            pconsoleArgs->m_runHowManyTimes = 1;
            pstrNextArg = &pconsoleArgs->m_strHowManyTimes;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--sample", strlen("--sample")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strRunSampleName.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strRunSampleName;
            fNextArgRequired = true;
        }
        else if (PAL::stricmp(pszArg, "--interactive") == 0)
        {
            fShowOptions = pconsoleArgs->m_fInteractivePrompt || fNextArgRequired;
            pconsoleArgs->m_fInteractivePrompt = true;
            pstrNextArg = NULL;
            fNextArgRequired = false;
        }
        else if (PAL::strnicmp(pszArg, "--devicename", strlen("--devicename")) == 0)
        {
            fShowOptions = fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strDeviceName;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--proxy-host", strlen("--proxy-host")) == 0)
        {
            fShowOptions = fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strProxyHost;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--proxy-port", strlen("--proxy-port")) == 0)
        {
            fShowOptions = fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strProxyPort;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--passthrough-certfile", strlen("--passthrough-certfile")) == 0)
        {
            fShowOptions = fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strPassthroughCertFile;
            fNextArgRequired = true;
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

        const char *pszArgInsideThisArg = strpbrk(pszArg, ":=");
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

    if (pconsoleArgs->m_audioInput.empty() || PAL::stricmp(pconsoleArgs->m_audioInput.c_str(), "microphone") == 0)
    {
        pconsoleArgs->m_useInteractiveMicrophone = true;
        pconsoleArgs->m_audioInput.clear();
    }
    else if (PAL::stricmp(pconsoleArgs->m_audioInput.c_str(), "mockmicrophone") == 0)
    {
        pconsoleArgs->m_useMockMicrophone = true;
        pconsoleArgs->m_audioInput.clear();

        if (!pconsoleArgs->m_strMockMicrophoneRealTimePercentage.empty())
        {
            pconsoleArgs->m_mockMicrophoneRealTimePercentage = (int16_t)atoi(pconsoleArgs->m_strMockMicrophoneRealTimePercentage.c_str());
            fValid = pconsoleArgs->m_mockMicrophoneRealTimePercentage >= 0 && pconsoleArgs->m_mockMicrophoneRealTimePercentage <= 400;
        }
    }
    else if (PAL::access(pconsoleArgs->m_audioInput.c_str(), 0) != 0)
    {
        SPX_DBG_TRACE_ERROR("File does not exist: %s", pconsoleArgs->m_audioInput.c_str());
        fValid = false;
    }

    if (!pconsoleArgs->m_kwsTable.empty() && PAL::access(pconsoleArgs->m_kwsTable.c_str(), 0) != 0)
    {
        SPX_DBG_TRACE_ERROR("File does not exist: %s", pconsoleArgs->m_kwsTable.c_str());
        fValid = false;
    }

    if (pconsoleArgs->m_fContinuousRecognition && pconsoleArgs->m_strContinuousRecognitionSeconds.length() > 0)
    {
        auto seconds = std::stoi(pconsoleArgs->m_strContinuousRecognitionSeconds.c_str());
        pconsoleArgs->m_continuousRecognitionSeconds = uint16_t(std::min(std::max(seconds, 0), 30));
    }

    if (pconsoleArgs->m_fRecognizeOnceAsync || pconsoleArgs->m_fContinuousRecognition)
    {
        if (pconsoleArgs->m_strRecognizerType.length() == 0)
        {
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<SpeechRecognizer>();
        }
    }
    else if (pconsoleArgs->m_strRecognizerType.length() > 0)
    {
        pconsoleArgs->m_fRecognizeOnceAsync = !pconsoleArgs->m_fCommandSystem && !pconsoleArgs->m_fInteractivePrompt;
    }

    if (!pconsoleArgs->m_strHowManyTimes.empty())
    {
        pconsoleArgs->m_runHowManyTimes = (int16_t)atoi(pconsoleArgs->m_strHowManyTimes.c_str());
    }

    return fValid;
}

void CarbonTestConsole::ProcessConsoleArgs(ConsoleArgs* pconsoleArgs)
{
    InitGlobalParameters(pconsoleArgs);

    if (pconsoleArgs->m_strRunSampleName.length() > 0)
    {
        auto count = pconsoleArgs->m_runHowManyTimes;
        while (count-- > 0)
        {
            RunSample(pconsoleArgs->m_strRunSampleName);
            ConsoleWrite(count > 0 ? "\n" : "");
        }
    }

    EnsureInitCarbon(pconsoleArgs);

    if (pconsoleArgs->m_fRecognizeOnceAsync)
    {
        auto count = pconsoleArgs->m_runHowManyTimes;
        while (count-- > 0)
        {
            RecognizeOnceAsync();
            ConsoleWrite(count > 0 ? "\n" : "");
        }
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
    ConsoleWriteLine("CarbonX - Carbon Test Console\nCopyright (C) 2017 Microsoft Corporation. All Rights Reserved.\n");
}

void CarbonTestConsole::DisplayConsoleUsage()
{
    ConsoleWriteLine("  carbonx [--speech | --intent | --commands | --translation ] {input} {auth} {additional}");
    ConsoleWriteLine("");
    ConsoleWriteLine("     Input: --input:[microphone | {waveFileName}]");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --input:microphone      Use the default microphone for audio input.");
    ConsoleWriteLine("       --input:{waveFileName}  Use WAV file for audio input.");
    ConsoleWriteLine("");
    ConsoleWriteLine("     Authentication and Service:");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --endpoint:{uri}              Use {uri} as the USP endpoint.");
    ConsoleWriteLine("       --subscription:{key}          Use {key} as the subscription key.");
    ConsoleWriteLine("       --region:{region}             Use {region} as the service region.");
    ConsoleWriteLine("       --customSpeechModelId:{id}    Use {id} as the Custom Speech Model ID.");
    ConsoleWriteLine("");
    ConsoleWriteLine("     Offline speech recognition:");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --offlineModelPathRoot:{path}  Use {path} as the root path for offline models.");
    ConsoleWriteLine("       --offlineModelLanguage:{lang}  Use {lang} as the language code, for example: en-US");
    ConsoleWriteLine("");
    ConsoleWriteLine("     Additional:");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --single                Use RecognizeOnceAsync for a single utterance.");
    ConsoleWriteLine("       --continuous:{seconds}  Use [Start/Stop]ContinuousRecognition, waiting");
    ConsoleWriteLine("                               {seconds} in between starting and stopping.");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --kwsTable:{fileName}   Use KWS table {fileName}.");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --sample:{sampleName}   Run the sample named {sampleName}.");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --debug                 Stops execution and waits (max 30s) for debugger.");
    ConsoleWriteLine("       --interactive           Allows interactive Carbon use via console window.");
    ConsoleWriteLine("");
}

void CarbonTestConsole::DisplayConsolePrompt()
{
    ConsoleWrite("carbonx> ");
}

void CarbonTestConsole::ConsoleWrite(const char* pszFormat, ...)
{
    va_list argptr;
    va_start(argptr, pszFormat);
    vfprintf(stdout, pszFormat, argptr);
    va_end(argptr);
}

void CarbonTestConsole::ConsoleWriteLine(const char* pszFormat, ...)
{
    std::string format(pszFormat);
    format += "\n";

    va_list argptr;
    va_start(argptr, pszFormat);
    vfprintf(stdout, format.c_str(), argptr);
    va_end(argptr);
}

bool CarbonTestConsole::ConsoleReadLine(std::string& str)
{
    const int cchMax = 1024;
    str.reserve(cchMax);
    str[0] = L'\0';

    char* data = (char*)str.data(); // should use CX17 .data(); VC2017 works fine; might not work cross-platform
    str = fgets(data, cchMax - 1, stdin);

    return str.length() > 0;
}

bool CarbonTestConsole::GetConsoleInput(std::string& str)
{
    DisplayConsolePrompt();

    ConsoleReadLine(str);
    auto lastLF = str.find_last_of(L'\n');

    if (lastLF != std::string::npos)
    {
        str.resize(lastLF);
    }

    return str.length() > 0;
}

void CarbonTestConsole::ProcessConsoleInput(const char* psz)
{
    if (PAL::stricmp(psz, "help") == 0 || PAL::stricmp(psz, "?") == 0)
    {
        ConsoleInput_Help();
    }
    else if (PAL::strnicmp(psz, "help ", strlen("help ")) == 0)
    {
        ConsoleInput_HelpOn(psz + strlen("help "));
    }
    else if (PAL::strnicmp(psz, "sample ", strlen("sample ")) == 0)
    {
        RunSample(psz + strlen("sample "));
    }
    else if (PAL::strnicmp(psz, "factory ", strlen("factory ")) == 0)
    {
        ConsoleInput_Factory(psz + strlen("factory "));
    }
    else if (PAL::strnicmp(psz, "recognizer ", strlen("recognizer ")) == 0)
    {
        ConsoleInput_Recognizer(psz + strlen("recognizer "), m_recognizer);
    }
    else if (PAL::strnicmp(psz, "speech ", strlen("speech ")) == 0)
    {
        ConsoleInput_SpeechRecognizer(psz + strlen("speech "), m_speechRecognizer);
    }
    else if (PAL::strnicmp(psz, "intent ", strlen("intent ")) == 0)
    {
        ConsoleInput_IntentRecognizer(psz + strlen("intent "), m_intentRecognizer);
    }
    else if (PAL::strnicmp(psz, "session ", strlen("session ")) == 0)
    {
        ConsoleInput_Session(psz + strlen("session "));
    }
    else if (PAL::strnicmp(psz, "commandsystem ", strlen("commandsystem ")) == 0)
    {
        ConsoleInput_CommandSystem(psz + strlen("commandsystem "));
    }
    else
    {
        ConsoleWriteLine("\nUnknown command: '%s'.\n\nUse 'HELP' for a list of valid commands.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_Help()
{
    ConsoleWriteLine("");
    ConsoleWriteLine("COMMANDs: ");
    ConsoleWriteLine("");
    ConsoleWriteLine("    global           Access methods/properties/events on the GLOBAL PARAMETERS object.");
    ConsoleWriteLine("    factory          Access methods/properties/events on the RECOGNIZER FACTORY object.");
    ConsoleWriteLine("    recognizer       Access methods/properties/events on the base RECOGNIZER object.");
    ConsoleWriteLine("    speech           Access methods/properties/events on the SPEECH recognizer object.");
    ConsoleWriteLine("    intent           Access methods/properties/events on the INTENT recognizer object.");
    ConsoleWriteLine("    session          Access methods/properties/events on the SESSION object.");
    ConsoleWriteLine("    commandsystem    Access methods/properties/events on the COMMAND SYSTEM object.");
    ConsoleWriteLine("    sample {name}    Run the sample named 'NAME'.");
    ConsoleWriteLine("    help {command}   Get help w/ 'recognizer', 'speech', 'intent', or 'commandsystem' commands.");
    ConsoleWriteLine("    exit             Exit interactive mode.");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOn(const char* psz)
{
    if (PAL::stricmp(psz, "globa") == 0)
    {
        ConsoleInput_HelpOnGlobal();
    }
    else if (PAL::stricmp(psz, "factory") == 0)
    {
        ConsoleInput_HelpOnFactory();
    }
    else if (PAL::stricmp(psz, "recognizer") == 0)
    {
        ConsoleInput_HelpOnRecognizer();
    }
    else if (PAL::stricmp(psz, "speech") == 0)
    {
        ConsoleInput_HelpOnSpeech();
    }
    else if (PAL::stricmp(psz, "intent") == 0)
    {
        ConsoleInput_HelpOnIntent();
    }
    else if (PAL::stricmp(psz, "session") == 0)
    {
        ConsoleInput_HelpOnSession();
    }
    else if (PAL::stricmp(psz, "commandsystem") == 0)
    {
        ConsoleInput_HelpOnCommandSystem();
    }
}

void CarbonTestConsole::ConsoleInput_HelpOnGlobal()
{
    ConsoleWriteLine("GLOBAL {method_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set string         {name} {value}");
    ConsoleWriteLine("    get string         {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set value          {name} {value}");
    ConsoleWriteLine("    get value          {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set bool           {name} {value}");
    ConsoleWriteLine("    get bool           {name}");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnFactory()
{
    ConsoleWriteLine("FACTORY {method_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    Create Speech Recognizer");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnRecognizer()
{
    ConsoleWriteLine("RECOGNIZER {method_command | event_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    IsEnabled");
    ConsoleWriteLine("    Enable");
    ConsoleWriteLine("    Disable");
    ConsoleWriteLine("");
    ConsoleWriteLine("    Recognize");
    ConsoleWriteLine("    StartContinuous");
    ConsoleWriteLine("    StopContinuous");
    ConsoleWriteLine("    StartKeyword");
    ConsoleWriteLine("    StopKeyword");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Events: ");
    ConsoleWriteLine("");
    ConsoleWriteLine("    SessionStarted     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SessionStopped     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechStartDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechEndDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Recognizing {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Recognized        {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Canceled           {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnSpeech()
{
    ConsoleWriteLine("SPEECH {method_command | event_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    IsEnabled");
    ConsoleWriteLine("    Enable");
    ConsoleWriteLine("    Disable");
    ConsoleWriteLine("");
    ConsoleWriteLine("    Recognize");
    ConsoleWriteLine("    StartContinuous");
    ConsoleWriteLine("    StopContinuous");
    ConsoleWriteLine("    StartKeyword");
    ConsoleWriteLine("    StopKeyword");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set string         {name} {value}");
    ConsoleWriteLine("    get string         {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set value          {name} {value}");
    ConsoleWriteLine("    get value          {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set bool           {name} {value}");
    ConsoleWriteLine("    get bool           {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Events: ");
    ConsoleWriteLine("");
    ConsoleWriteLine("    SessionStarted     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SessionStopped     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechStartDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechEndDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Recognizing {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Recognized        {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Canceled           {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnIntent()
{
    ConsoleWriteLine("INTENT {method_command | event_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    IsEnabled");
    ConsoleWriteLine("    Enable");
    ConsoleWriteLine("    Disable");
    ConsoleWriteLine("");
    ConsoleWriteLine("    Recognize");
    ConsoleWriteLine("    StartContinuous");
    ConsoleWriteLine("    StopContinuous");
    ConsoleWriteLine("    StartKeyword");
    ConsoleWriteLine("    StopKeyword");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Events: ");
    ConsoleWriteLine("");
    ConsoleWriteLine("    SessionStarted     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SessionStopped     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechStartDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechEndDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Recognizing {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Recognized        {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Canceled           {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnSession()
{
    ConsoleWriteLine("SESSION {method_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    from speech");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set string     {name} {value}");
    ConsoleWriteLine("    get string     {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set number     {name} {value}");
    ConsoleWriteLine("    get number     {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set bool       {name} {value}");
    ConsoleWriteLine("    get bool       {name}");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnCommandSystem()
{
    ConsoleWriteLine("COMMANDSYSTEM {method_command | event_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    NYI");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Events: ");
    ConsoleWriteLine("");
    ConsoleWriteLine("    NYI");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_Factory(const char* psz)
{
    if (PAL::strnicmp(psz, "create speech recognizer", strlen("create speech recognizer")) == 0)
    {
        //TODO: Check if required keys are given for other execution paths: this is not the only feature which needs subscriptionKey
        if (!m_subscriptionKey.empty() || !m_endpointUri.empty())
        {
            Factory_CreateSpeechRecognizer(psz + strlen("create speech recognizer"));
        }
        else
        {
            ConsoleWriteLine("\nsubscription ID is required to use this feature. Please re-run with valid subscription ID\n");
        }
    }
    else
    {
        ConsoleWriteLine("\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_Recognizer(const char* psz, std::shared_ptr<BaseAsyncRecognizer>& recognizer)
{
    if (PAL::stricmp(psz, "isenabled") == 0)
    {
        recognizer_is_enabled(recognizer);
    }
    else if (PAL::stricmp(psz, "enable") == 0)
    {
        recognizer_enable(recognizer);
    }
    else if (PAL::stricmp(psz, "disable") == 0)
    {
        recognizer_disable(recognizer);
    }
    else if (PAL::stricmp(psz, "recognize") == 0)
    {
        recognizer_recognize_once(recognizer);
    }
    else if (PAL::stricmp(psz, "startcontinuous") == 0)
    {
        recognizer_start_continuous_recognition(recognizer);
    }
    else if (PAL::stricmp(psz, "stopcontinuous") == 0)
    {
        recognizer_stop_continuous_recognition(recognizer);
    }
    else if (PAL::stricmp(psz, "startkeyword") == 0)
    {
        recognizer_start_keyword_recognition(recognizer);
    }
    else if (PAL::stricmp(psz, "stopkeyword ") == 0)
    {
        recognizer_stop_keyword_recognition(recognizer);
    }
    else if (PAL::strnicmp(psz, "sessionstarted ", strlen("sessionstarted ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStartedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstarted "), m_recognizer->SessionStarted, fn);
    }
    else if (PAL::strnicmp(psz, "sessionstopped ", strlen("sessionstopped ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStoppedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstopped "), m_recognizer->SessionStopped, fn);
    }
    else if (PAL::strnicmp(psz, "speechstartdetected ", strlen("speechstartdetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechStartDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechstartdetected "), m_recognizer->SpeechStartDetected, fn);
    }
    else if (PAL::strnicmp(psz, "speechenddetected ", strlen("speechenddetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechEndDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechenddetected "), m_recognizer->SpeechEndDetected, fn);
    }
    else if (PAL::strnicmp(psz, "intermediateresult ", strlen("intermediateresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_RecognizingHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("intermediateresult "), m_recognizer->Recognizing, fn);
    }
    else if (PAL::strnicmp(psz, "finalresult ", strlen("finalresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_RecognizedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("finalresult "), m_recognizer->Recognized, fn);
    }
    else if (PAL::strnicmp(psz, "canceled ", strlen("canceled ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_CanceledHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("canceled "), m_recognizer->Canceled, fn);
    }
    else
    {
        ConsoleWriteLine("\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_SpeechRecognizer(const char* psz, std::shared_ptr<SpeechRecognizer>& speechRecognizer)
{
    if (PAL::stricmp(psz, "isenabled") == 0)
    {
        recognizer_is_enabled(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "enable") == 0)
    {
        recognizer_enable(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "disable") == 0)
    {
        recognizer_disable(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "recognize") == 0)
    {
        recognizer_recognize_once(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "startcontinuous") == 0)
    {
        recognizer_start_continuous_recognition(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "stopcontinuous") == 0)
    {
        recognizer_stop_continuous_recognition(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "startkeyword") == 0)
    {
        recognizer_start_keyword_recognition(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "stopkeyword") == 0)
    {
        recognizer_stop_keyword_recognition(speechRecognizer);
    }
    else if (PAL::strnicmp(psz, "sessionstarted ", strlen("sessionstarted ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStartedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstarted "), m_speechRecognizer->SessionStarted, fn);
    }
    else if (PAL::strnicmp(psz, "sessionstopped ", strlen("sessionstopped ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStoppedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstopped "), m_speechRecognizer->SessionStopped, fn);
    }
    else if (PAL::strnicmp(psz, "speechstartdetected ", strlen("speechstartdetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechStartDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechstartdetected "), m_speechRecognizer->SpeechStartDetected, fn);
    }
    else if (PAL::strnicmp(psz, "speechenddetected ", strlen("speechenddetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechEndDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechenddetected "), m_speechRecognizer->SpeechEndDetected, fn);
    }
    else if (PAL::strnicmp(psz, "intermediateresult ", strlen("intermediateresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::SpeechRecognizer_RecognizingHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("intermediateresult "), m_speechRecognizer->Recognizing, fn);
    }
    else if (PAL::strnicmp(psz, "finalresult ", strlen("finalresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::SpeechRecognizer_RecognizedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("finalresult "), m_speechRecognizer->Recognized, fn);
    }
    else if (PAL::strnicmp(psz, "canceled ", strlen("canceled ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::SpeechRecognizer_CanceledHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("canceled "), m_speechRecognizer->Canceled, fn);
    }
    else if (PAL::strnicmp(psz, "set string ", strlen("set string ")) == 0)
    {
        Parameters_SetString(m_speechRecognizer->Properties, psz + strlen("set string "));
    }
    else if (PAL::strnicmp(psz, "get string ", strlen("get string ")) == 0)
    {
        Parameters_GetString(m_speechRecognizer->Properties, psz + strlen("get string "));
    }
    else if (PAL::strnicmp(psz, "set number ", strlen("set number ")) == 0)
    {
        Parameters_SetNumber(m_speechRecognizer->Properties, psz + strlen("set number "));
    }
    else if (PAL::strnicmp(psz, "get number ", strlen("get number ")) == 0)
    {
        Parameters_GetNumber(m_speechRecognizer->Properties, psz + strlen("get number "));
    }
    else if (PAL::strnicmp(psz, "set bool ", strlen("set bool ")) == 0)
    {
        Parameters_SetBool(m_speechRecognizer->Properties, psz + strlen("set bool "));
    }
    else if (PAL::strnicmp(psz, "get bool ", strlen("get bool ")) == 0)
    {
        Parameters_GetBool(m_speechRecognizer->Properties, psz + strlen("get bool "));
    }
    else
    {
        ConsoleWriteLine("\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_IntentRecognizer(const char* psz, std::shared_ptr<IntentRecognizer>& intentRecognizer)
{
    if (PAL::stricmp(psz, "isenabled") == 0)
    {
        recognizer_is_enabled(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "enable") == 0)
    {
        recognizer_enable(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "disable") == 0)
    {
        recognizer_disable(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "recognize") == 0)
    {
        recognizer_recognize_once(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "startcontinuous") == 0)
    {
        recognizer_start_continuous_recognition(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "stopcontinuous") == 0)
    {
        recognizer_stop_continuous_recognition(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "startkeyword") == 0)
    {
        recognizer_start_keyword_recognition(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "stopkeyword") == 0)
    {
        recognizer_stop_keyword_recognition(intentRecognizer);
    }
    else if (PAL::strnicmp(psz, "sessionstarted ", strlen("sessionstarted ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStartedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstarted "), m_intentRecognizer->SessionStarted, fn);
    }
    else if (PAL::strnicmp(psz, "sessionstopped ", strlen("sessionstopped ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStoppedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstopped "), m_intentRecognizer->SessionStopped, fn);
    }
    else if (PAL::strnicmp(psz, "speechstartdetected ", strlen("speechstartdetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechStartDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechstartdetected "), m_intentRecognizer->SpeechStartDetected, fn);
    }
    else if (PAL::strnicmp(psz, "speechenddetected ", strlen("speechenddetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechEndDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechenddetected "), m_intentRecognizer->SpeechEndDetected, fn);
    }
    else if (PAL::strnicmp(psz, "intermediateresult ", strlen("intermediateresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::IntentRecognizer_RecognizingHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("intermediateresult "), m_intentRecognizer->Recognizing, fn);
    }
    else if (PAL::strnicmp(psz, "finalresult ", strlen("finalresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::IntentRecognizer_RecognizedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("finalresult "), m_intentRecognizer->Recognized, fn);
    }
    else if (PAL::strnicmp(psz, "canceled ", strlen("canceled ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::IntentRecognizer_CanceledHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("canceled "), m_intentRecognizer->Canceled, fn);
    }
    else if (PAL::strnicmp(psz, "set string ", strlen("set string ")) == 0)
    {
        Parameters_SetString(m_intentRecognizer->Properties, psz + strlen("set string "));
    }
    else if (PAL::strnicmp(psz, "get string ", strlen("get string ")) == 0)
    {
        Parameters_GetString(m_intentRecognizer->Properties, psz + strlen("get string "));
    }
    else if (PAL::strnicmp(psz, "set number ", strlen("set number ")) == 0)
    {
        Parameters_SetNumber(m_intentRecognizer->Properties, psz + strlen("set number "));
    }
    else if (PAL::strnicmp(psz, "get number ", strlen("get number ")) == 0)
    {
        Parameters_GetNumber(m_intentRecognizer->Properties, psz + strlen("get number "));
    }
    else if (PAL::strnicmp(psz, "set bool ", strlen("set bool ")) == 0)
    {
        Parameters_SetBool(m_intentRecognizer->Properties, psz + strlen("set bool "));
    }
    else if (PAL::strnicmp(psz, "get bool ", strlen("get bool ")) == 0)
    {
        Parameters_GetBool(m_intentRecognizer->Properties, psz + strlen("get bool "));
    }
    else
    {
        ConsoleWriteLine("\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_CommandSystem(const char*)
{
    // TODO: ROBCH: Implement CarbonTestConsole::ConsoleInput_CommandSystem
}

void CarbonTestConsole::Factory_CreateSpeechRecognizer(const char* psz)
{
    m_speechRecognizer = nullptr;
    m_recognizer = nullptr;
    m_session = nullptr;

    auto sc = SpeechRecognizerConfig();

    std::string filename(psz + 1);
    m_speechRecognizer = SpeechRecognizer::FromConfig(sc, AudioConfig::FromWavFileInput(filename));

    auto fn1 = std::bind(&CarbonTestConsole::SpeechRecognizer_RecognizedHandler, this, std::placeholders::_1);
    m_speechRecognizer->Recognized.Connect(fn1);

    auto fn2 = std::bind(&CarbonTestConsole::SpeechRecognizer_RecognizingHandler, this, std::placeholders::_1);
    m_speechRecognizer->Recognizing.Connect(fn2);

    m_recognizer = BaseAsyncRecognizer::FromRecognizer(m_speechRecognizer);
    m_session = Session::FromRecognizer(m_speechRecognizer);
}

std::shared_ptr<SpeechConfig> CarbonTestConsole::SpeechRecognizerConfig()
{
    std::shared_ptr<SpeechConfig> sc = nullptr;

    if (!m_endpointUri.empty())
    {
        sc = SpeechConfig::FromEndpoint(m_endpointUri, m_subscriptionKey);
    }
    else
    {
        sc = SpeechConfig::FromSubscription(m_subscriptionKey, m_region);
    }

    if (!m_offlineModelPathRoot.empty())
    {
        // Test offline speech recognition without public API
        SpxSetMockParameterString(R"(CARBON-INTERNAL-UseRecoEngine-Unidec)", "true");
        SpxSetMockParameterString(R"(CARBON-INTERNAL-SPEECH-RecoLocalModelPathRoot)", m_offlineModelPathRoot.c_str());
        
        if (!m_offlineModelPathRoot.empty())
        {
            SpxSetMockParameterString(R"(CARBON-INTERNAL-SPEECH-RecoLocalModelLanguage)", m_offlineModelLanguage.c_str());
        }
        else
        {
            SpxSetMockParameterString(R"(CARBON-INTERNAL-SPEECH-RecoLocalModelLanguage)", "en-US");
        }
    }
    else
    {
        SpxSetMockParameterString(R"(CARBON-INTERNAL-UseRecoEngine-Unidec)", "false");
    }

    return sc;
}

template <class T>
void CarbonTestConsole::recognizer_is_enabled(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWrite("\n%s.IsEnabled == ", name.c_str());
    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine("%s\n", BoolToString(enabled).c_str());
}

template <class T>
void CarbonTestConsole::recognizer_enable(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nEnabling %s...", name.c_str());
    recognizer->Enable();
    ConsoleWriteLine("Enabling %s... Done!\n", name.c_str());

    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine("%s.IsEnabled == %s\n", name.c_str(), BoolToString(enabled).c_str());
}

template <class T>
void CarbonTestConsole::recognizer_disable(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nDisabling %s...", name.c_str());
    recognizer->Disable();
    ConsoleWriteLine("Disabling %s... Done!\n", name.c_str());

    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine("%s.IsEnabled == %s\n", name.c_str(), BoolToString(enabled).c_str());
}

template <class T>
void CarbonTestConsole::recognizer_recognize_once(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nRecognizeOnceAsync %s...", name.c_str());
    auto future = recognizer->RecognizeOnceAsync();
    ConsoleWriteLine("RecognizeOnceAsync %s... Waiting...", name.c_str());
    auto result = future.get();
    ConsoleWriteLine("RecognizeOnceAsync %s... Waiting... Done!\n", name.c_str());
}

void CarbonTestConsole::recognizer_recognize_once(std::shared_ptr<SpeechRecognizer>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nRecognizeOnceAsync %s...", name.c_str());
    auto future = recognizer->RecognizeOnceAsync();
    ConsoleWriteLine("RecognizeOnceAsync %s... Waiting...", name.c_str());
    auto result = future.get();
    ConsoleWriteLine("RecognizeOnceAsync %s... Waiting... Done!\n", name.c_str());
    ConsoleWriteLine("SpeechRecognitionResult: ResultId=%s; Reason=%d; Text=%s", result->ResultId.c_str(), result->Reason, result->Text.c_str());

    if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        ConsoleWriteLine("SpeechRecognitionResult: CancellationReason=%d, ErrorCode=%d, ErrorDetails=%s", cancellation->Reason, cancellation->ErrorCode, cancellation->ErrorDetails.c_str());
    }

}

void CarbonTestConsole::recognizer_recognize_once(std::shared_ptr<IntentRecognizer>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nRecognizeOnceAsync %s...", name.c_str());
    auto future = recognizer->RecognizeOnceAsync();
    ConsoleWriteLine("RecognizeOnceAsync %s... Waiting...", name.c_str());
    auto result = future.get();
    ConsoleWriteLine("RecognizeOnceAsync %s... Waiting... Done!\n", name.c_str());

    auto resultId = result->ResultId;
    auto reason = result->Reason;
    auto text = result->Text;

    auto intentId = result->IntentId;
    auto intentJson = result->Properties.GetProperty(PropertyId::LanguageUnderstandingServiceResponse_JsonResult);

    ConsoleWriteLine("IntentRecognitionResult: ResultId=%s; Reason=%d; Text=%s, IntentId=%s, Json=%s", resultId.c_str(), reason, text.c_str(), intentId.c_str(), intentJson.c_str());

    if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        ConsoleWriteLine("IntentRecognitionResult: CancellationReason=%d, ErrorCode=%d, ErrorDetails=%s", cancellation->Reason, cancellation->ErrorCode, cancellation->ErrorDetails.c_str());
    }
}

void CarbonTestConsole::recognizer_recognize_once(std::shared_ptr<TranslationRecognizer>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nRecognizeOnceAsync %s...", name.c_str());
    auto future = recognizer->RecognizeOnceAsync();
    ConsoleWriteLine("RecognizeOnceAsync %s... Waiting...", name.c_str());
    auto result = future.get();
    ConsoleWriteLine("RecognizeOnceAsync %s... Waiting... Done!\n", name.c_str());

    ConsoleWriteLine("TranslationRecognitionResult: ResultId=%s, RecognizedText=%s", result->TranslationRecognitionResult::ResultId.c_str(), result->Text.c_str());

    if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        ConsoleWriteLine("TranslationRecognitionResult: CancellationReason=%d, ErrorCode=%d, ErrorDetails=%s", cancellation->Reason, cancellation->ErrorCode, cancellation->ErrorDetails.c_str());
    }

    for (auto it : result->Translations)
    {
        ConsoleWriteLine("                Translation to %s: %s", it.first.c_str(), it.second.c_str());
    }
}

template <class T>
void CarbonTestConsole::recognizer_start_continuous_recognition(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nStartContinuousRecognitionAsync %s...", name.c_str());
    auto future = recognizer->StartContinuousRecognitionAsync();
    ConsoleWriteLine("StartContinuousRecognitionAsync %s... Waiting...", name.c_str());
    future.get();
    ConsoleWriteLine("StartContinuousRecognitionAsync %s... Waiting... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::recognizer_stop_continuous_recognition(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nStopContinuousRecognitionAsync %s...", name.c_str());
    auto future = recognizer->StopContinuousRecognitionAsync();
    ConsoleWriteLine("StopContinuousRecognitionAsync %s... Waiting...", name.c_str());
    future.get();
    ConsoleWriteLine("StopContinuousRecognitionAsync %s... Waiting... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::recognizer_start_keyword_recognition(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nStartKeywordRecognitionAsync %s...", name.c_str());

    auto model = KeywordRecognitionModel::FromFile(m_kwsTable);
    auto future = recognizer->StartKeywordRecognitionAsync(model);

    ConsoleWriteLine("StartKeywordRecognitionAsync %s... Waiting...", name.c_str());
    future.get();
    ConsoleWriteLine("StartKeywordRecognitionAsync %s... Waiting... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::recognizer_stop_keyword_recognition(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nStopKeywordRecognitionAsync %s...", name.c_str());
    auto future = recognizer->StopKeywordRecognitionAsync();
    ConsoleWriteLine("StopKeywordRecognitionAsync %s... Waiting...", name.c_str());
    future.get();
    ConsoleWriteLine("StopKeywordRecognitionAsync %s... Waiting... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_Event(const char* psz, EventSignal<T>& recognizerEvent, typename::EventSignal<T>::CallbackFunction callback)
{
    if (PAL::stricmp(psz, "connect") == 0)
    {
        recognizerEvent.Connect(callback);
    }
    else if (PAL::stricmp(psz, "disconnect") == 0)
    {
        recognizerEvent.Disconnect(callback);
    }
    else if (PAL::stricmp(psz, "disconnectal") == 0)
    {
        recognizerEvent.DisconnectAll();
    }
    else
    {
        ConsoleWriteLine("\nUnknown event method: '%s'.\n\nUse 'HELP' for a list of valid commands.", psz);
    }
}

void CarbonTestConsole::ConsoleInput_Session(const char* psz)
{
    if (PAL::stricmp(psz, "from speech") == 0)
    {
        Session_FromSpeechRecognizer();
    }
    // else if (PAL::stricmp(psz, "from intent") == 0)
    // {
    //     Session_FromIntentRecognizer();
    // }
    else if (PAL::strnicmp(psz, "set string ", strlen("set string ")) == 0)
    {
        Parameters_SetString(m_session->Properties, psz + strlen("set string "));
    }
    else if (PAL::strnicmp(psz, "get string ", strlen("get string ")) == 0)
    {
        Parameters_GetString(m_session->Properties, psz + strlen("get string "));
    }
    else if (PAL::strnicmp(psz, "set number ", strlen("set number ")) == 0)
    {
        Parameters_SetNumber(m_session->Properties, psz + strlen("set number "));
    }
    else if (PAL::strnicmp(psz, "get number ", strlen("get number ")) == 0)
    {
        Parameters_GetNumber(m_session->Properties, psz + strlen("get number "));
    }
    else if (PAL::strnicmp(psz, "set bool ", strlen("set bool ")) == 0)
    {
        Parameters_SetBool(m_session->Properties, psz + strlen("set bool "));
    }
    else if (PAL::strnicmp(psz, "get bool ", strlen("get bool ")) == 0)
    {
        Parameters_GetBool(m_session->Properties, psz + strlen("get bool "));
    }
    else
    {
        ConsoleWriteLine("\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::Session_FromSpeechRecognizer()
{
    auto name = PAL::GetTypeName(*m_speechRecognizer.get());
    ConsoleWriteLine("\nGetting Session from %s...", name.c_str());
    m_session = Session::FromRecognizer(m_speechRecognizer);
    ConsoleWriteLine("Getting Session from %s... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::Parameters_SetString(T &parameters, const char* psz)
{
    std::string input(psz);
    auto iSpace = input.find(' ');
    if (iSpace != std::string::npos && psz[iSpace + 1] != '\0')
    {
        std::string name(psz, iSpace);
        std::string value(psz + iSpace + 1);
        parameters.SetProperty(name, value);
        ConsoleWriteLine("Set string '%s' to '%s'!\n", name.c_str(), psz + iSpace + 1);
    }
    else
    {
        ConsoleWriteLine("\nInvalid usage: '%s'.\n\nUse 'HELP' for valid usage.\n", psz);
    }
}

template <class T>
void CarbonTestConsole::Parameters_GetString(T &parameters, const char* psz)
{
    auto value = parameters.GetProperty(psz);
    ConsoleWriteLine("Get string '%s' : '%s'\n", psz, value.c_str());
}

template <class T>
void CarbonTestConsole::Parameters_SetNumber(T &parameters, const char* psz)
{
    return Parameters_SetString(parameters, psz);
}

template <class T>
void CarbonTestConsole::Parameters_GetBool(T &parameters, const char* psz)
{
    auto value = parameters.GetProperty(psz);
    ConsoleWriteLine("Get bool '%s' : %s\n", psz, value.c_str());
}

template <class T>
void CarbonTestConsole::Parameters_SetBool(T &parameters, const char* psz)
{
    return Parameters_SetString(parameters, psz);
}

template <class T>
void CarbonTestConsole::Parameters_GetNumber(T &parameters, const char* psz)
{
    auto value = parameters.GetProperty(psz);
    ConsoleWriteLine("Get number '%s' : %d\n", psz, value.c_str());
}

bool CarbonTestConsole::ToBool(const char* psz)
{
    return PAL::ToBool(psz);
}

std::string CarbonTestConsole::BoolToString(bool f)
{
    return PAL::BoolToString(f);
}

std::string CarbonTestConsole::ToString(const SpeechRecognitionEventArgs& e)
{
    static_assert(0 == (int)ResultReason::NoMatch, "ResultReason::* enum values changed!");
    static_assert(1 == (int)ResultReason::Canceled, "ResultReason::* enum values changed!");
    static_assert(2 == (int)ResultReason::RecognizingSpeech, "ResultReason::* enum values changed!");
    static_assert(3 == (int)ResultReason::RecognizedSpeech, "ResultReason::* enum values changed!");
    static_assert(4 == (int)ResultReason::RecognizingIntent, "ResultReason::* enum values changed!");
    static_assert(5 == (int)ResultReason::RecognizedIntent, "ResultReason::* enum values changed!");
    static_assert(6 == (int)ResultReason::TranslatingSpeech, "ResultReason::* enum values changed!");
    static_assert(7 == (int)ResultReason::TranslatedSpeech, "ResultReason::* enum values changed!");
    static_assert(8 == (int)ResultReason::SynthesizingAudio, "ResultReason::* enum values changed!");
    static_assert(9 == (int)ResultReason::SynthesizingAudioCompleted, "ResultReason::* enum values changed!");
    static_assert(10 == (int)ResultReason::RecognizingKeyword, "ResultReason::* enum values changed!");
    static_assert(11 == (int)ResultReason::RecognizedKeyword, "ResultReason::* enum values changed!");
    static_assert(12 == (int)ResultReason::SynthesizingAudioStarted, "ResultReason::* enum values changed!");

    static_assert(1 == (int)CancellationReason::Error, "CancellationReason::* enum values changed!");
    static_assert(2 == (int)CancellationReason::EndOfStream, "CancellationReason::* enum values changed!");

    static_assert(0 == (int)CancellationErrorCode::NoError, "CancellationErrorCode::* enum values changed!");
    static_assert(1 == (int)CancellationErrorCode::AuthenticationFailure, "CancellationErrorCode::* enum values changed!");
    static_assert(2 == (int)CancellationErrorCode::BadRequest, "CancellationErrorCode::* enum values changed!");
    static_assert(3 == (int)CancellationErrorCode::TooManyRequests, "CancellationErrorCode::* enum values changed!");
    static_assert(4 == (int)CancellationErrorCode::Forbidden, "CancellationErrorCode::* enum values changed!");
    static_assert(5 == (int)CancellationErrorCode::ConnectionFailure, "CancellationErrorCode::* enum values changed!");
    static_assert(6 == (int)CancellationErrorCode::ServiceTimeout, "CancellationErrorCode::* enum values changed!");
    static_assert(7 == (int)CancellationErrorCode::ServiceError, "CancellationErrorCode::* enum values changed!");
    static_assert(8 == (int)CancellationErrorCode::ServiceUnavailable, "CancellationErrorCode::* enum values changed!");
    static_assert(9 == (int)CancellationErrorCode::RuntimeError, "CancellationErrorCode::* enum values changed!");

    static_assert(1 == (int)NoMatchReason::NotRecognized, "NoMatchReason::* enum values changed!");
    static_assert(2 == (int)NoMatchReason::InitialSilenceTimeout, "NoMatchReason::* enum values changed!");
    static_assert(3 == (int)NoMatchReason::InitialBabbleTimeout, "NoMatchReason::* enum values changed!");
    static_assert(4 == (int)NoMatchReason::KeywordNotRecognized, "NoMatchReason::* enum values changed!");

    std::string reasons[] = {
        "NoMatch",
        "Canceled",
        "RecognizingSpeech",
        "RecognizedSpeech",
        "RecognizingIntent",
        "RecognizedIntent",
        "TranslatingSpeech",
        "TranslatedSpeech",
        "SynthesizingAudio",
        "SynthesizingAudioCompleted",
        "RecognizingKeyword",
        "RecognizedKeyword",
        "SynthesizingAudioStarted"
    };

    std::string reasonsCanceled[] = {
        "",
        "Error",
        "EndOfStream",
    };

    std::string noMatchReasons[] = {
        "",
        "NotRecognized",
        "InitialSilenceTimeout",
        "InitialBabbleTimeout",
        "KeywordNotRecognized"
    };

    std::string str;
    str += "SpeechRecognitionEventArgs = { \n";
    str += "  SessionId = '" + e.SessionId + "'\n";
    str += "  Result = {\n";
    str += "    ResultId = '" + e.Result->ResultId + "'\n";
    str += "    Reason = ResultReason::" + reasons[(int)e.Result->Reason] + "\n";
    if (e.Result->Reason == ResultReason::NoMatch)
    {
        auto noMatch = NoMatchDetails::FromResult(e.Result);
        str += "    NoMatchReason = NoMatchReason::" + noMatchReasons[(int)noMatch->Reason] + "\n";
    }
    if (e.Result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(e.Result);
        str += "    CancellationReason = CancellationReason::" + reasonsCanceled[(int)cancellation->Reason] + "\n";
        if (!cancellation->ErrorDetails.empty())
        {
            str += "    ErrorDetails = '" + cancellation->ErrorDetails + "'\n";
        }
    }
    str += "    Text = '" + e.Result->Text + "'\n";
    str += "  } \n";
    str += "} \n";

    return str;
}

std::string CarbonTestConsole::ToString(const IntentRecognitionEventArgs& e)
{
    static_assert(0 == (int)ResultReason::NoMatch, "ResultReason::* enum values changed!");
    static_assert(1 == (int)ResultReason::Canceled, "ResultReason::* enum values changed!");
    static_assert(2 == (int)ResultReason::RecognizingSpeech, "ResultReason::* enum values changed!");
    static_assert(3 == (int)ResultReason::RecognizedSpeech, "ResultReason::* enum values changed!");
    static_assert(4 == (int)ResultReason::RecognizingIntent, "ResultReason::* enum values changed!");
    static_assert(5 == (int)ResultReason::RecognizedIntent, "ResultReason::* enum values changed!");
    static_assert(6 == (int)ResultReason::TranslatingSpeech, "ResultReason::* enum values changed!");
    static_assert(7 == (int)ResultReason::TranslatedSpeech, "ResultReason::* enum values changed!");
    static_assert(8 == (int)ResultReason::SynthesizingAudio, "ResultReason::* enum values changed!");
    static_assert(9 == (int)ResultReason::SynthesizingAudioCompleted, "ResultReason::* enum values changed!");
    static_assert(10 == (int)ResultReason::RecognizingKeyword, "ResultReason::* enum values changed!");
    static_assert(11 == (int)ResultReason::RecognizedKeyword, "ResultReason::* enum values changed!");
    static_assert(12 == (int)ResultReason::SynthesizingAudioStarted, "ResultReason::* enum values changed!");

    static_assert(1 == (int)NoMatchReason::NotRecognized, "NoMatchReason::* enum values changed!");
    static_assert(2 == (int)NoMatchReason::InitialSilenceTimeout, "NoMatchReason::* enum values changed!");
    static_assert(3 == (int)NoMatchReason::InitialBabbleTimeout, "NoMatchReason::* enum values changed!");
    static_assert(4 == (int)NoMatchReason::KeywordNotRecognized, "NoMatchReason::* enum values changed!");

    std::string reasons[] = {
        "NoMatch",
        "Canceled",
        "RecognizingSpeech",
        "RecognizedSpeech",
        "RecognizingIntent",
        "RecognizedIntent",
        "TranslatingSpeech",
        "TranslatedSpeech",
        "SynthesizingAudio",
        "SynthesizingAudioCompleted",
        "RecognizingKeyword",
        "RecognizedKeyword",
        "SynthesizingAudioStarted"
    };

    std::string reasonsCanceled[] = {
        "",
        "Error",
        "EndOfStream",
    };

    std::string noMatchReasons[] = {
        "",
        "NotRecognized",
        "InitialSilenceTimeout",
        "InitialBabbleTimeout",
        "KeywordNotRecognized"
    };

    std::string str;
    str += "IntentRecognitionEventArgs = { \n";
    str += "  SessionId = '" + e.SessionId + "'\n";
    str += "  Result = {\n";
    str += "    ResultId = '" + e.Result->ResultId + "'\n";
    str += "    Reason = ResultReason::" + reasons[(int)e.Result->Reason] + "\n";
    if (e.Result->Reason == ResultReason::NoMatch)
    {
        auto noMatch = NoMatchDetails::FromResult(e.Result);
        str += "    NoMatchReason = NoMatchReason::" + noMatchReasons[(int)noMatch->Reason] + "\n";
    }
    if (e.Result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(e.Result);
        str += "    CancellationReason = CancellationReason::" + reasonsCanceled[(int)cancellation->Reason] + "\n";
        if (!cancellation->ErrorDetails.empty())
        {
            str += "    ErrorDetails = '" + cancellation->ErrorDetails + "'\n";
        }
    }
    str += "    Text = '" + e.Result->Text + "'\n";
    str += "  } \n";
    str += "} \n";

    return str;
}

std::string CarbonTestConsole::ToString(const TranslationRecognitionEventArgs& e)
{
    static_assert(0 == (int)ResultReason::NoMatch, "ResultReason::* enum values changed!");
    static_assert(1 == (int)ResultReason::Canceled, "ResultReason::* enum values changed!");
    static_assert(2 == (int)ResultReason::RecognizingSpeech, "ResultReason::* enum values changed!");
    static_assert(3 == (int)ResultReason::RecognizedSpeech, "ResultReason::* enum values changed!");
    static_assert(4 == (int)ResultReason::RecognizingIntent, "ResultReason::* enum values changed!");
    static_assert(5 == (int)ResultReason::RecognizedIntent, "ResultReason::* enum values changed!");
    static_assert(6 == (int)ResultReason::TranslatingSpeech, "ResultReason::* enum values changed!");
    static_assert(7 == (int)ResultReason::TranslatedSpeech, "ResultReason::* enum values changed!");
    static_assert(8 == (int)ResultReason::SynthesizingAudio, "ResultReason::* enum values changed!");
    static_assert(9 == (int)ResultReason::SynthesizingAudioCompleted, "ResultReason::* enum values changed!");
    static_assert(10 == (int)ResultReason::RecognizingKeyword, "ResultReason::* enum values changed!");
    static_assert(11 == (int)ResultReason::RecognizedKeyword, "ResultReason::* enum values changed!");
    static_assert(12 == (int)ResultReason::SynthesizingAudioStarted, "ResultReason::* enum values changed!");

    static_assert(1 == (int)NoMatchReason::NotRecognized, "NoMatchReason::* enum values changed!");
    static_assert(2 == (int)NoMatchReason::InitialSilenceTimeout, "NoMatchReason::* enum values changed!");
    static_assert(3 == (int)NoMatchReason::InitialBabbleTimeout, "NoMatchReason::* enum values changed!");
    static_assert(4 == (int)NoMatchReason::KeywordNotRecognized, "NoMatchReason::* enum values changed!");

    std::string reasons[] = {
        "NoMatch",
        "Canceled",
        "RecognizingSpeech",
        "RecognizedSpeech",
        "RecognizingIntent",
        "RecognizedIntent",
        "TranslatingSpeech",
        "TranslatedSpeech",
        "SynthesizingAudio",
        "SynthesizingAudioCompleted",
        "RecognizingKeyword",
        "RecognizedKeyword",
        "SynthesizingAudioStarted"
    };
    std::string reasonsCanceled[] = {
        "",
        "Error",
        "EndOfStream",
    };

    std::string noMatchReasons[] = {
        "",
        "NotRecognized",
        "InitialSilenceTimeout",
        "InitialBabbleTimeout",
        "KeywordNotRecognized"
    };

    std::string str;
    str += "TranslationEventArgs<TranslationRecognitionResult> = { \n";
    str += "  SessionId = '" + e.SessionId + "'\n";
    str += "  Result = {\n";
    str += "    ResultId = '" + e.Result->ResultId + "'\n";
    str += "    RecognizedText = '" + e.Result->Text + "'\n";
    if (e.Result->Reason == ResultReason::NoMatch)
    {
        auto noMatch = NoMatchDetails::FromResult(e.Result);
        str += "    NoMatchReason = NoMatchReason::" + noMatchReasons[(int)noMatch->Reason] + "\n";
    }
    if (e.Result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(e.Result);
        str += "    CancellationReason = CancellationReason::" + reasonsCanceled[(int)cancellation->Reason] + "\n";
        if (!cancellation->ErrorDetails.empty())
        {
            str += "    ErrorDetails = '" + cancellation->ErrorDetails + "'\n";
        }
    }
    for (auto it : e.Result->Translations)
    {
        str += "    Translation to " + it.first + ": " + it.second + ".";
    }
    str += "  } \n";
    str += "} \n";


    return str;
}

std::string CarbonTestConsole::ToString(const TranslationSynthesisEventArgs& e)
{
    std::string str;
    str += "TranslationEventArgs<TranslationSynthesisResult> = { \n";
    str += "  SessionId = '" + e.SessionId + "'\n";
    str += "  Result = {\n";
    str += "    SizeOfAudioData = " + std::to_string(e.Result->Audio.size()) + "\n";
    str += "  } \n";
    str += "} \n";

    return str;
}

void CarbonTestConsole::InitGlobalParameters(ConsoleArgs* pconsoleArgs)
{
    m_region = R"(westus)";
    m_kwsTable = "kws.table";

    if (pconsoleArgs->m_useMockMicrophone)
    {
        SpxSetMockParameterBool(R"(CARBON-INTERNAL-MOCK-Microphone)", true);
        SpxSetMockParameterNumber(R"(CARBON-INTERNAL-MOCK-RealTimeAudioPercentage)", pconsoleArgs->m_mockMicrophoneRealTimePercentage);
        if (!pconsoleArgs->m_mockWavFileName.empty())
        {
            SpxSetMockParameterString(R"(CARBON-INTERNAL-MOCK-WavFileAudio)", pconsoleArgs->m_mockWavFileName.c_str());
            SpxSetMockParameterBool(R"(CARBON-INTERNAL-MOCK-ContinuousAudio)", true);
        }
    }

    if (pconsoleArgs->m_useMockKws)
    {
        SpxSetMockParameterBool(R"(CARBON-INTERNAL-MOCK-SdkKwsEngine)", true);
    }

    if (!pconsoleArgs->m_strUseRecoEngineProperty.empty())
    {
        SpxSetMockParameterBool(pconsoleArgs->m_strUseRecoEngineProperty.c_str(), true);
    }

    if (!pconsoleArgs->m_strUseLuEngineProperty.empty())
    {
        SpxSetMockParameterBool(pconsoleArgs->m_strUseLuEngineProperty.c_str(), true);
    }

    if (!pconsoleArgs->m_strSubscriptionKey.empty())
    {
        m_subscriptionKey = pconsoleArgs->m_strSubscriptionKey;
    }

    if (!pconsoleArgs->m_strRegion.empty())
    {
        m_region = pconsoleArgs->m_strRegion;
    }

    if (!pconsoleArgs->m_strEndpointUri.empty())
    {
        m_endpointUri = pconsoleArgs->m_strEndpointUri;
    }

    if (!pconsoleArgs->m_strOfflineModelPathRoot.empty())
    {
        m_offlineModelPathRoot = pconsoleArgs->m_strOfflineModelPathRoot;
    }

    if (!pconsoleArgs->m_strOfflineModelLanguage.empty())
    {
        m_offlineModelLanguage = pconsoleArgs->m_strOfflineModelLanguage;
    }

    if (!pconsoleArgs->m_kwsTable.empty())
    {
        m_kwsTable = pconsoleArgs->m_kwsTable;
    }

    if (!pconsoleArgs->m_strCustomSpeechModelId.empty())
    {
        m_customSpeechModelId = pconsoleArgs->m_strCustomSpeechModelId;
    }

    if (!pconsoleArgs->m_strIntentAppId.empty())
    {
        m_intentAppId = pconsoleArgs->m_strIntentAppId;
    }

    if (!pconsoleArgs->m_strIntentNames.empty())
    {
        std::string::size_type i, j;
        const std::string &s = pconsoleArgs->m_strIntentNames;

        for (i = j = 0; j = s.find(",", i), j < std::string::npos; i = j + 1)
        {
            if (i < j)
                m_intentNames.push_back(s.substr(i, j - i));
        }
        if (i < s.length())
            m_intentNames.push_back(s.substr(i));
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
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    m_recognizer = nullptr;
    m_speechRecognizer = nullptr;
    m_intentRecognizer = nullptr;
    m_commandSystem = nullptr;
}

void CarbonTestConsole::InitCarbon(ConsoleArgs* pconsoleArgs)
{
    try
    {
        InitRecognizer(pconsoleArgs->m_strRecognizerType, pconsoleArgs->m_audioInput, pconsoleArgs->m_strDeviceName, pconsoleArgs->m_strProxyHost, pconsoleArgs->m_strProxyPort, pconsoleArgs->m_strPassthroughCertFile);
        InitCommandSystem();
    }
    catch (std::exception ex)
    {
        TermCarbon();
    }
}

void *CarbonTestConsole::OpenCompressedFile(const std::string& compressedFileName)
{
    FILE *filep = NULL;
    {
#ifdef WIN32
        std::wstring filename(compressedFileName.begin(), compressedFileName.end());
        _wfopen_s(&filep, filename.c_str(), L"rb");
#else
        filep = fopen(compressedFileName.c_str(), "rb");
#endif
    }
    return filep;
}

void CarbonTestConsole::closeStream(void* fp)
{
    fclose((FILE*)fp);
}

int CarbonTestConsole::ReadCompressedBinaryData(void *stream, uint8_t *ptr, uint32_t buf_size)
{
    size_t  ret = 0;
    FILE* localRead = (FILE*)stream;

    if (localRead != NULL && !feof(localRead))
    {
        ret = fread(ptr, 1, buf_size, localRead);
    }
    return (int)ret;
}


void CarbonTestConsole::InitRecognizer(const std::string& recognizerType, const std::string& wavFileName, const std::string& deviceName, const std::string& proxyHost, const std::string& proxyPort, const std::string& passthroughCertFile)
{
    if (recognizerType == PAL::GetTypeName<SpeechRecognizer>())
    {
        auto sc = SpeechRecognizerConfig();

        if (!proxyHost.empty() && !proxyPort.empty())
        {
            sc->SetProxy(proxyHost, std::stoi(proxyPort));
        }

        if (!passthroughCertFile.empty())
        {
            std::ifstream certfile(passthroughCertFile);
            std::string cert((std::istreambuf_iterator<char>(certfile)),
                 std::istreambuf_iterator<char>());

            std::cout << "Setting certificate " << cert << "\n";
            sc->SetProperty("OPENSSL_SINGLE_TRUSTED_CERT", cert);
            sc->SetProperty("OPENSSL_SINGLE_TRUSTED_CERT_CRL_CHECK", "false");
        }
        if (deviceName.empty())
        {
            if (wavFileName.find(".mp3") == (wavFileName.size() - 4))
            {
                m_pullAudioStream = AudioInputStream::CreatePullStream(
                    AudioStreamFormat::GetCompressedFormat(AudioStreamContainerFormat::MP3),
                    OpenCompressedFile(wavFileName),
                    ReadCompressedBinaryData,
                    closeStream
                );
                m_speechRecognizer = SpeechRecognizer::FromConfig(sc, AudioConfig::FromStreamInput(m_pullAudioStream));
            }
            else if (wavFileName.find(".opus") == (wavFileName.size() - 5))
            {
                m_pullAudioStream = AudioInputStream::CreatePullStream(
                    AudioStreamFormat::GetCompressedFormat(AudioStreamContainerFormat::OGG_OPUS),
                    OpenCompressedFile(wavFileName),
                    ReadCompressedBinaryData,
                    closeStream
                );
                m_speechRecognizer = SpeechRecognizer::FromConfig(sc, AudioConfig::FromStreamInput(m_pullAudioStream));
            }
            else if (wavFileName.find(".alaw") == (wavFileName.size() - 5))
            {
                m_pullAudioStream = AudioInputStream::CreatePullStream(
                    AudioStreamFormat::GetCompressedFormat(AudioStreamContainerFormat::ALAW),
                    OpenCompressedFile(wavFileName),
                    ReadCompressedBinaryData,
                    closeStream
                );
                m_speechRecognizer = SpeechRecognizer::FromConfig(sc, AudioConfig::FromStreamInput(m_pullAudioStream));
            }
            else if (wavFileName.find(".mulaw") == (wavFileName.size() - 6))
            {
                m_pullAudioStream = AudioInputStream::CreatePullStream(
                    AudioStreamFormat::GetCompressedFormat(AudioStreamContainerFormat::MULAW),
                    OpenCompressedFile(wavFileName),
                    ReadCompressedBinaryData,
                    closeStream
                );
                m_speechRecognizer = SpeechRecognizer::FromConfig(sc, AudioConfig::FromStreamInput(m_pullAudioStream));
            }
            else if (wavFileName.find(".flac") == (wavFileName.size() - 5))
            {
                m_pullAudioStream = AudioInputStream::CreatePullStream(
                    AudioStreamFormat::GetCompressedFormat(AudioStreamContainerFormat::FLAC),
                    OpenCompressedFile(wavFileName),
                    ReadCompressedBinaryData,
                    closeStream
                );
                m_speechRecognizer = SpeechRecognizer::FromConfig(sc, AudioConfig::FromStreamInput(m_pullAudioStream));
            }
            else
            {
                m_speechRecognizer = SpeechRecognizer::FromConfig(sc, AudioConfig::FromWavFileInput(wavFileName));
            }
        }
        else
        {
            ConsoleWriteLine("Carbonx: Received device name is '%s'", deviceName.c_str());
            auto audioInput = AudioConfig::FromMicrophoneInput(deviceName);

            audioInput->SetProperty("AudioConfig_NumberOfChannels", "1");
            m_speechRecognizer = SpeechRecognizer::FromConfig(sc, audioInput);
        }

        auto fn1 = std::bind(&CarbonTestConsole::SpeechRecognizer_RecognizedHandler, this, std::placeholders::_1);
        m_speechRecognizer->Recognized.Connect(fn1);

        auto fn2 = std::bind(&CarbonTestConsole::SpeechRecognizer_RecognizingHandler, this, std::placeholders::_1);
        m_speechRecognizer->Recognizing.Connect(fn2);

        m_recognizer = BaseAsyncRecognizer::FromRecognizer(m_speechRecognizer);
        m_session = Session::FromRecognizer(m_speechRecognizer);
    }
    else if (recognizerType == PAL::GetTypeName<TranslationRecognizer>())
    {
        auto sc = !m_endpointUri.empty()
            ? SpeechTranslationConfig::FromEndpoint(m_endpointUri, m_subscriptionKey)
            : SpeechTranslationConfig::FromSubscription(m_subscriptionKey, m_region);

        sc->SetSpeechRecognitionLanguage("en-US");
        sc->AddTargetLanguage("de");
        sc->AddTargetLanguage("es");

        m_translationRecognizer = TranslationRecognizer::FromConfig(sc, AudioConfig::FromWavFileInput(wavFileName));

        auto fn1 = std::bind(&CarbonTestConsole::TranslationRecognizer_RecognizedHandler, this, std::placeholders::_1);
        m_translationRecognizer->Recognized.Connect(fn1);

        auto fn2 = std::bind(&CarbonTestConsole::TranslationRecognizer_RecognizingHandler, this, std::placeholders::_1);
        m_translationRecognizer->Recognizing.Connect(fn2);

        auto fn3 = std::bind(&CarbonTestConsole::TranslationRecognizer_SynthesisResultHandler, this, std::placeholders::_1);
        m_translationRecognizer->Synthesizing.Connect(fn3);

        // Todo: add error handler
        //auto fn4 = std::bind(&CarbonTestConsole::TranslationRecognizer_ErrorHandler, this, std::placeholders::_1);
        //m_translationRecognizer->OnTranslationError.Connect(fn4);
    }
    else if (recognizerType == PAL::GetTypeName<IntentRecognizer>())
    {
        auto sc = !m_endpointUri.empty()
            ? SpeechConfig::FromEndpoint(m_endpointUri, m_subscriptionKey)
            : SpeechConfig::FromSubscription(m_subscriptionKey, m_region);
        sc->SetSpeechRecognitionLanguage("en-US");

        m_intentRecognizer = IntentRecognizer::FromConfig(sc, AudioConfig::FromWavFileInput(wavFileName));

        auto fn1 = std::bind(&CarbonTestConsole::IntentRecognizer_RecognizedHandler, this, std::placeholders::_1);
        m_intentRecognizer->Recognized.Connect(fn1);

        auto fn2 = std::bind(&CarbonTestConsole::IntentRecognizer_RecognizingHandler, this, std::placeholders::_1);
        m_intentRecognizer->Recognizing.Connect(fn2);

        if (!m_intentAppId.empty())
        {
            auto model = LanguageUnderstandingModel::FromAppId(m_intentAppId);
            for (auto & intentName : m_intentNames)
            {
                m_intentRecognizer->AddIntent(model, intentName);
            }
        }

        m_recognizer = BaseAsyncRecognizer::FromRecognizer(m_intentRecognizer);
        m_session = Session::FromRecognizer(m_intentRecognizer);

        m_recognizer = BaseAsyncRecognizer::FromRecognizer(m_intentRecognizer);
        // TODO: RobCh: Intent: Hook up Session::FromRecognizer
        // m_session = Session::FromRecognizer(m_speechRecognizer);
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

void CarbonTestConsole::RecognizeOnceAsync()
{
    if (m_intentRecognizer != nullptr)
    {
        recognizer_recognize_once(m_intentRecognizer);
    }
    else if (m_speechRecognizer != nullptr)
    {
        recognizer_recognize_once(m_speechRecognizer);
    }
    else if (m_translationRecognizer != nullptr)
    {
        recognizer_recognize_once(m_translationRecognizer);
    }
    else if (m_recognizer != nullptr)
    {
        recognizer_recognize_once(m_recognizer);
    }
}

void CarbonTestConsole::ContinuousRecognition(uint16_t seconds)
{
    if (m_intentRecognizer != nullptr)
    {
        recognizer_start_continuous_recognition(m_intentRecognizer);

        ConsoleWrite("Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine("Done!");

        recognizer_stop_continuous_recognition(m_intentRecognizer);
    }
    else if (m_speechRecognizer != nullptr)
    {
        recognizer_start_continuous_recognition(m_speechRecognizer);

        ConsoleWrite("Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine("Done!");

        recognizer_stop_continuous_recognition(m_speechRecognizer);
    }
    else if (m_translationRecognizer != nullptr)
    {
        recognizer_start_continuous_recognition(m_translationRecognizer);

        ConsoleWrite("Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine("Done!");

        recognizer_stop_continuous_recognition(m_translationRecognizer);
    }
    else if (m_recognizer != nullptr)
    {
        recognizer_start_continuous_recognition(m_recognizer);

        ConsoleWrite("Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine("Done!");

        recognizer_stop_continuous_recognition(m_recognizer);
    }
}

void CarbonTestConsole::RunSample(const std::string& strSampleName)
{
    if (PAL::stricmp(strSampleName.c_str(), "helloworld") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld with reason") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_WithReasonInfo();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld microphone") == 0)
    {
        ConsoleWriteLine("Running sample: %ls\n", strSampleName.c_str());
        Sample_HelloWorld_Microphone();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld file") == 0)
    {
        ConsoleWriteLine("Running sample: %ls\n", strSampleName.c_str());
        Sample_HelloWorld_File();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld push stream") == 0)
    {
        ConsoleWriteLine("Running sample: %ls\n", strSampleName.c_str());
        Sample_HelloWorld_PushStream();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld pull stream") == 0)
    {
        ConsoleWriteLine("Running sample: %ls\n", strSampleName.c_str());
        Sample_HelloWorld_PullStream();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld with events") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_WithEvents();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld c") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_In_C();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld usp") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_PickEngine("Usp");
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld unidec") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_PickEngine("Unidec");
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld mockengine") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_PickEngine("Mock");
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld intent") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_Intent();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld subscription with cris") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_Subscription_With_CRIS();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld french") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_Language("fr-fr");
    }
    else if (PAL::stricmp(strSampleName.c_str(), "channel9") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        channel9();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_speech") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_speech();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_speech_intermediate") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_speech_intermediate();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_speech_continuous") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_speech_continuous();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_intent") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_intent();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_intent_continuous") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_intent_continuous();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_kws_speech") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_kws_speech();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_kws_speech_with_kwv") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_kws_speech_with_kwv();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_kws_intent") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_kws_intent();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_translation") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_translation();
    }
    else
    {
        ConsoleWriteLine("\nUnknown sample: '%s'.\n", strSampleName.c_str());
    }
}

void CarbonTestConsole::RunInteractivePrompt()
{
    std::string strInput;
    while (GetConsoleInput(strInput))
    {
        if (PAL::stricmp(strInput.c_str(), "exit") == 0) break;
        if (PAL::stricmp(strInput.c_str(), "quit") == 0) break;

        try
        {
            ProcessConsoleInput(strInput.c_str());
        }
        catch (std::exception ex)
        {
            SPX_TRACE_ERROR("CarbonX: Exception caught: %s!", ex.what());
        }
    }
}


int SafeMain(const std::vector<std::string>& args)
{
    SPX_DBG_TRACE_SCOPE("\r\n=== CarbonX (Carbon Test Console) started\n\n", "\r\n=== CarbonX (carbon test console) ended");

    auto unhandledException = []() ->void { SPX_TRACE_ERROR("CarbonX: Unhandled exception!"); };
    std::set_terminate(unhandledException);

    try
    {
        CarbonTestConsole test;
        auto result = test.Run(args);
        return result;
    }
    catch (SPXHR hr)
    {
        auto handle = reinterpret_cast<SPXERRORHANDLE>(hr);
        auto error = error_get_error_code(handle);
        if (error != SPX_NOERROR)
        {
            auto callstack = error_get_call_stack(handle);
            auto what = error_get_message(handle);
            SPX_TRACE_ERROR("Carbonx: got exception %s, %s", what, callstack);
        }
        else
        {
            SPX_TRACE_ERROR("Carbonx: got exception hr=0x%" PRIxPTR, hr);
        }
        exit(-1);
    }
    catch (const std::exception& e)
    {
        SPX_TRACE_ERROR("CarbonX: Unhandled exception on main thread! what=%s", e.what());
        exit(-1);
    }
    catch (...)
    {
        SPX_TRACE_ERROR("CarbonX: Unhandled exception on main thread!");
        exit(-1);
    }
}

#ifdef _MSC_VER

#ifdef _DEBUG
// in case of asserts in debug mode, print the message into stderr and throw exception
int HandleDebugAssert(int,               // reportType  - ignoring reportType, printing message and aborting for all reportTypes
    char *message,     // message     - fully assembled debug user message
    int * returnValue) // returnValue - retVal value of zero continues execution
{
    fprintf(stderr, "C-Runtime: %s\n", message);

    if (returnValue) {
        *returnValue = 0;   // return value of 0 will continue operation and NOT start the debugger
    }

    return 1;            // make sure no message box is displayed
}
#endif

int wmain(int argc, const wchar_t* argv[])
{
#ifdef _DEBUG
    // in case of asserts in debug mode, print the message into stderr and throw exception
    if (_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, HandleDebugAssert) == -1) {
        fprintf(stderr, "_CrtSetReportHook2 failed.\n");
        return -1;
    }
#endif
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(PAL::ToString(argv[i]));
    }

    return SafeMain(args);
}
#else
#include <cstring>
#include <cassert>

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    return SafeMain(args);
}
#endif
