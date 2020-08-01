//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// unidec_runtime_dll.h: Implementation definitions for UnidecDll C++ class
//

#pragma once

#include <cstdint>

#include "stdafx.h"
#include "UnidecRuntime.h"

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class UnidecRuntimeDll
{
#define DECLARE_UNIDEC_FUNC_TYPE_NAME(x) x##Pointer
#define DECLARE_UNIDEC_FUNC_MEMBER_NAME(x) m_fn##x
#define DECLARE_UNIDEC_FUNC_MEMBER(x) \
    using DECLARE_UNIDEC_FUNC_TYPE_NAME(x) = decltype(&(x)); \
    DECLARE_UNIDEC_FUNC_TYPE_NAME(x) DECLARE_UNIDEC_FUNC_MEMBER_NAME(x);
#ifdef _MSC_VER
#define LOAD_UNIDEC_FUNC_IMPL(handle, x) DECLARE_UNIDEC_FUNC_MEMBER_NAME(x) = (DECLARE_UNIDEC_FUNC_TYPE_NAME(x))(GetProcAddress(handle, #x))
#else
#define LOAD_UNIDEC_FUNC_IMPL(handle, x) DECLARE_UNIDEC_FUNC_MEMBER_NAME(x) = (DECLARE_UNIDEC_FUNC_TYPE_NAME(x))(dlsym(handle, #x))
#endif
#define FORWARD_UNIDEC_FUNC_CALL(x, ...) (GetUnidecRuntimeDll().DECLARE_UNIDEC_FUNC_MEMBER_NAME(x)(__VA_ARGS__))

public:

    static UnidecRuntimeDll& GetUnidecRuntimeDll()
    {
        static UnidecRuntimeDll wrapper;
        return wrapper;
    }

    ~UnidecRuntimeDll()
    {
        if (m_handle != nullptr)
        {
#ifdef _MSC_VER
            FreeLibrary(m_handle);
#else
            dlclose(m_handle);
#endif
            m_handle = nullptr;
        }
    }

    static IConfig* CreateConfig(const WCHAR* iniFilePath) { return FORWARD_UNIDEC_FUNC_CALL(CreateConfig, iniFilePath); }
    static IUnidecSearchGraphCombo* CreateUnidecSearchGraphCombo(const WCHAR* spec) { return FORWARD_UNIDEC_FUNC_CALL(CreateUnidecSearchGraphCombo, spec); }
    static IUnidecSearchGraphCombo* CreateUnidecSearchGraphComboEx(
        const WCHAR* spec,
        const IUnidecSearchGraphCombo* base)
    {
        return FORWARD_UNIDEC_FUNC_CALL(CreateUnidecSearchGraphComboEx, spec, base);
    }
    static void DeleteUnidecSearchGraphCombo(IUnidecSearchGraphCombo* graphs) { return FORWARD_UNIDEC_FUNC_CALL(DeleteUnidecSearchGraphCombo, graphs); }
    static size_t GetUnidecIntermediateResultWordCount(
        const IUnidecSearchGraphCombo* graphs,
        const IUnidecIntermediateResult* intermediateresult,
        bool outputMetadata)
    {
        return FORWARD_UNIDEC_FUNC_CALL(GetUnidecIntermediateResultWordCount, graphs, intermediateresult, outputMetadata);
    }
    static void GetUnidecIntermediateResultWords(
        const IUnidecSearchGraphCombo* graphs,
        const IUnidecIntermediateResult* intermediateResult,
        bool outputMetadata,
        const WCHAR** words,
        size_t wordsLen)
    {
        return FORWARD_UNIDEC_FUNC_CALL(GetUnidecIntermediateResultWords, graphs, intermediateResult, outputMetadata, words, wordsLen);
    }
    static size_t GetUnidecNBestListSize(const IUnidecNBestList* nbest) { return FORWARD_UNIDEC_FUNC_CALL(GetUnidecNBestListSize, nbest); }
    static float GetUnidecNBestListCost(
        const IUnidecNBestList* nbest,
        size_t i)
    {
        return FORWARD_UNIDEC_FUNC_CALL(GetUnidecNBestListCost, nbest, i);
    }
    static float GetUnidecNBestListAMCost(
        const IUnidecNBestList* nbest,
        size_t i)
    {
        return FORWARD_UNIDEC_FUNC_CALL(GetUnidecNBestListAMCost, nbest, i);
    }
    static float GetUnidecNBestListLMCost(
        const IUnidecNBestList* nbest,
        size_t i)
    {
        return FORWARD_UNIDEC_FUNC_CALL(GetUnidecNBestListLMCost, nbest, i);
    }
    static float GetUnidecNBestListPrunedLMCost(
        const IUnidecNBestList* nbest,
        size_t i)
    {
        return FORWARD_UNIDEC_FUNC_CALL(GetUnidecNBestListPrunedLMCost, nbest, i);
    }
    static size_t GetUnidecNBestListWordCount(
        const IUnidecSearchGraphCombo* graphs,
        const IUnidecNBestList* nbest,
        size_t i,
        bool outputMetadata)
    {
        return FORWARD_UNIDEC_FUNC_CALL(GetUnidecNBestListWordCount, graphs, nbest, i, outputMetadata);
    }
    static void GetUnidecNBestListWords(
        const IUnidecSearchGraphCombo* graphs,
        const IUnidecNBestList* nbest,
        size_t i,
        bool outputMetadata,
        const WCHAR** words,
        size_t wordsLen)
    {
        return FORWARD_UNIDEC_FUNC_CALL(GetUnidecNBestListWords, graphs, nbest, i, outputMetadata, words, wordsLen);
    }
    static float GetUnidecNBestListConfidence(
        const IUnidecSearchGraphCombo* graphs,
        const IUnidecNBestList* nbest,
        size_t i)
    {
        return FORWARD_UNIDEC_FUNC_CALL(GetUnidecNBestListConfidence, graphs, nbest, i);
    }
    static float GetUnidecNBestListReserved(
        const IUnidecNBestList* nbest,
        size_t i)
    {
        return FORWARD_UNIDEC_FUNC_CALL(GetUnidecNBestListReserved, nbest, i);
    }
    static IUnidecEngine* CreateUnidecStreamEngine(
        const IConfig* config,
        UnidecNextStreamCallback nextCallback,
        UnidecIntermediateCallback intermediateCallback = nullptr,
        UnidecSentenceCallback sentenceCallback = nullptr,
        UnidecEndCallback endCallback = nullptr,
        void* callbackContext = nullptr)
    {
        return FORWARD_UNIDEC_FUNC_CALL(CreateUnidecStreamEngine, config, nextCallback, intermediateCallback, sentenceCallback, endCallback, callbackContext);
    }
    static void StartUnidecEngine(IUnidecEngine* engine) { return FORWARD_UNIDEC_FUNC_CALL(StartUnidecEngine, engine); }
    static void JoinUnidecEngine(IUnidecEngine* engine) { return FORWARD_UNIDEC_FUNC_CALL(JoinUnidecEngine, engine); }
    static bool GetUnidecEngineStarted(IUnidecEngine* engine) { return FORWARD_UNIDEC_FUNC_CALL(GetUnidecEngineStarted, engine); }
    static void DeleteUnidecEngine(IUnidecEngine* engine) { return FORWARD_UNIDEC_FUNC_CALL(DeleteUnidecEngine, engine); }

private:

    UnidecRuntimeDll()
    {
        m_handle = nullptr;

#ifdef _MSC_VER
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
        m_handle = LoadLibraryA("UnidecRuntime.dll");
#else // Windows Store WinRT app
        m_handle = LoadPackagedLibrary(L"UnidecRuntime.dll", 0);
#endif
        SPX_TRACE_VERBOSE_IF(m_handle == nullptr, "Failed to load UnidecRuntime.dll.");
        SPX_IFTRUE_THROW_HR(m_handle == nullptr, HRESULT_FROM_WIN32(GetLastError()));
#else
        m_handle = dlopen("libSpeechToText.so", RTLD_LAZY | RTLD_LOCAL);
        SPX_TRACE_VERBOSE_IF(m_handle == nullptr, "Failed to load libSpeechToText.so: %s.", dlerror());
        SPX_IFTRUE_THROW_HR(m_handle == nullptr, SPXERR_INVALID_HANDLE);
#endif

        LOAD_UNIDEC_FUNC_IMPL(m_handle, CreateConfig);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, CreateUnidecSearchGraphCombo);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, CreateUnidecSearchGraphComboEx);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, DeleteUnidecSearchGraphCombo);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecIntermediateResultWordCount);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecIntermediateResultWords);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecNBestListSize);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecNBestListCost);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecNBestListAMCost);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecNBestListLMCost);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecNBestListPrunedLMCost);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecNBestListWordCount);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecNBestListWords);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecNBestListConfidence);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecNBestListReserved);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, CreateUnidecStreamEngine);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, StartUnidecEngine);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, JoinUnidecEngine);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, GetUnidecEngineStarted);
        LOAD_UNIDEC_FUNC_IMPL(m_handle, DeleteUnidecEngine);
    }

#ifdef _MSC_VER
    HMODULE m_handle;
#else
    void* m_handle;
#endif

    DECLARE_UNIDEC_FUNC_MEMBER(CreateConfig)
    DECLARE_UNIDEC_FUNC_MEMBER(CreateUnidecSearchGraphCombo)
    DECLARE_UNIDEC_FUNC_MEMBER(CreateUnidecSearchGraphComboEx)
    DECLARE_UNIDEC_FUNC_MEMBER(DeleteUnidecSearchGraphCombo)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecIntermediateResultWordCount)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecIntermediateResultWords)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecNBestListSize)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecNBestListCost)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecNBestListAMCost)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecNBestListLMCost)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecNBestListPrunedLMCost)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecNBestListWordCount)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecNBestListWords)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecNBestListConfidence)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecNBestListReserved)
    DECLARE_UNIDEC_FUNC_MEMBER(CreateUnidecStreamEngine)
    DECLARE_UNIDEC_FUNC_MEMBER(StartUnidecEngine)
    DECLARE_UNIDEC_FUNC_MEMBER(JoinUnidecEngine)
    DECLARE_UNIDEC_FUNC_MEMBER(GetUnidecEngineStarted)
    DECLARE_UNIDEC_FUNC_MEMBER(DeleteUnidecEngine)
};

}}}}
