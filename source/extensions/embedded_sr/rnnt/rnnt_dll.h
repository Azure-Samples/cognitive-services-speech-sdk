//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rnnt_dll.h: Implementation definitions for RnntDll C++ class
//

#pragma once

#include <cstdint>

#include "stdafx.h"
#include "rnnt_decoder.h"
#include "unimic_runtime.h"

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_OSX
#include <dlfcn.h>
#endif
#else
#include <dlfcn.h>
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace RNNT {

class RnntDll
{
#define DECLARE_RNNT_FUNC_TYPE_NAME(x) x##Pointer
#define DECLARE_RNNT_FUNC_MEMBER_NAME(x) m_fn##x
#define DECLARE_RNNT_FUNC_MEMBER(x) \
    using DECLARE_RNNT_FUNC_TYPE_NAME(x) = decltype(&(x)); \
    DECLARE_RNNT_FUNC_TYPE_NAME(x) DECLARE_RNNT_FUNC_MEMBER_NAME(x);
#ifdef _MSC_VER
#define LOAD_RNNT_FUNC_IMPL(handle, x) DECLARE_RNNT_FUNC_MEMBER_NAME(x) = (DECLARE_RNNT_FUNC_TYPE_NAME(x))(GetProcAddress(handle, #x))
#elif defined(__APPLE__) && !TARGET_OS_OSX
#define LOAD_RNNT_FUNC_IMPL(handle, x) DECLARE_RNNT_FUNC_MEMBER_NAME(x) = (DECLARE_RNNT_FUNC_TYPE_NAME(x))&::x
#else
#define LOAD_RNNT_FUNC_IMPL(handle, x) DECLARE_RNNT_FUNC_MEMBER_NAME(x) = (DECLARE_RNNT_FUNC_TYPE_NAME(x))(dlsym(handle, #x))
#endif
#define FORWARD_RNNT_FUNC_CALL(x, ...) (GetRnntDll().DECLARE_RNNT_FUNC_MEMBER_NAME(x)(__VA_ARGS__))

public:

    static RnntDll& GetRnntDll()
    {
        static RnntDll wrapper;
        return wrapper;
    }

    ~RnntDll()
    {
        if (m_handle != nullptr)
        {
#ifdef _MSC_VER
            FreeLibrary(m_handle);
#elif !defined(__APPLE__) || (defined(__APPLE__) && TARGET_OS_OSX)
            dlclose(m_handle);
#endif
            m_handle = nullptr;
        }
    }

    static IUnimicDecoder* CreateUnimicDecoder(const wchar_t* spec) { return FORWARD_RNNT_FUNC_CALL(CreateUnimicDecoder, spec); }
    static size_t GetUnimicDecoderInputDim(IUnimicDecoder* decoder) { return FORWARD_RNNT_FUNC_CALL(GetUnimicDecoderInputDim, decoder); }
    static bool RunUnimicDecoder(IUnimicDecoder* decoder, bool continuousReco, uint32_t decoderInSilFrmCnt, uint32_t startTimeoutFrmCnt, uint32_t totalAudioLengthFrmCnt, bool& hypoChanged) { return FORWARD_RNNT_FUNC_CALL(RunUnimicDecoder, decoder, continuousReco, decoderInSilFrmCnt, startTimeoutFrmCnt, totalAudioLengthFrmCnt, hypoChanged); }
    static IUnimicDecoderNBest* GetUnimicDecoderNBest(IUnimicDecoder* decoder) { return FORWARD_RNNT_FUNC_CALL(GetUnimicDecoderNBest, decoder); }
    static void DeleteUnimicDecoder(IUnimicDecoder* decoder) { FORWARD_RNNT_FUNC_CALL(DeleteUnimicDecoder, decoder); }
    static void ResetUnimicDecoder(IUnimicDecoder* decoder, IUnimicSource* source, const IUnimicDecoderSFLM* unimicDecoderSFLM) { FORWARD_RNNT_FUNC_CALL(ResetUnimicDecoder, decoder, source, unimicDecoderSFLM); }
    static size_t GetUnimicDecoderNBestCount(IUnimicDecoderNBest* nbest) { return FORWARD_RNNT_FUNC_CALL(GetUnimicDecoderNBestCount, nbest); }
    static const uint32_t* GetUnimicDecoderNBestEntry(IUnimicDecoderNBest* nbest, size_t i, size_t* plen, float* scores, size_t scores_cnt)
    {
        return FORWARD_RNNT_FUNC_CALL(GetUnimicDecoderNBestEntry, nbest, i, plen, scores, scores_cnt);
    }
    static void DeleteUnimicDecoderNBest(IUnimicDecoderNBest* nbest) { FORWARD_RNNT_FUNC_CALL(DeleteUnimicDecoderNBest, nbest); }
    static IUnimicSource* CreateCustomUnimicSource(size_t channelCnt, float sampleRate, bool (*readCallback)(float*, void*), void* context)
    {
        return FORWARD_RNNT_FUNC_CALL(CreateCustomUnimicSource, channelCnt, sampleRate, readCallback, context);
    }
    static void DeleteUnimicSource(IUnimicSource* source) { FORWARD_RNNT_FUNC_CALL(DeleteUnimicSource, source); }
    static float GetUnimicSourceSampleRate(IUnimicSource* source) { return FORWARD_RNNT_FUNC_CALL(GetUnimicSourceSampleRate, source); }
    static size_t GetUnimicSourceChannelCount(IUnimicSource* source) { return FORWARD_RNNT_FUNC_CALL(GetUnimicSourceChannelCount, source); }
    static bool ReadUnimicSource(IUnimicSource* source, float* sample) { return FORWARD_RNNT_FUNC_CALL(ReadUnimicSource, source, sample); }
    static IUnimicFilter* CreateUnimicFilter(const wchar_t* spec) { return FORWARD_RNNT_FUNC_CALL(CreateUnimicFilter, spec); }
    static void DeleteUnimicFilter(IUnimicFilter* filter) { FORWARD_RNNT_FUNC_CALL(DeleteUnimicFilter, filter); }
    static void SetUnimicFilterInputPort(IUnimicFilter* filter, size_t index, IUnimicSource* source)
    {
        FORWARD_RNNT_FUNC_CALL(SetUnimicFilterInputPort, filter, index, source);
    }
    static size_t GetUnimicFilterOutputPortCount(IUnimicFilter* filter) { return FORWARD_RNNT_FUNC_CALL(GetUnimicFilterOutputPortCount, filter); }
    static IUnimicSource* GetUnimicFilterOutputPort(IUnimicFilter* filter, size_t index) { return FORWARD_RNNT_FUNC_CALL(GetUnimicFilterOutputPort, filter, index); }

private:

    RnntDll()
    {
        m_handle = nullptr;

#ifdef _MSC_VER
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
        m_handle = LoadLibraryA("unimic_runtime.dll");
#else // Windows Store WinRT app
        m_handle = LoadPackagedLibrary(L"unimic_runtime.dll", 0);
#endif
        SPX_TRACE_VERBOSE_IF(m_handle == nullptr, "Failed to load unimic_runtime.dll.");
        SPX_IFTRUE_THROW_HR(m_handle == nullptr, HRESULT_FROM_WIN32(GetLastError()));
#elif defined(__APPLE__)
#if TARGET_OS_OSX
        m_handle = dlopen("libpasco_runtime.dylib", RTLD_LAZY | RTLD_LOCAL);
        SPX_TRACE_VERBOSE_IF(m_handle == nullptr, "Failed to load libpasco_runtime.dylib: %s.", dlerror());
        SPX_IFTRUE_THROW_HR(m_handle == nullptr, SPXERR_INVALID_HANDLE);
#endif
#else
        m_handle = dlopen("libpasco_runtime.so", RTLD_LAZY | RTLD_LOCAL);
        SPX_TRACE_VERBOSE_IF(m_handle == nullptr, "Failed to load libpasco_runtime.so: %s.", dlerror());
        SPX_IFTRUE_THROW_HR(m_handle == nullptr, SPXERR_INVALID_HANDLE);
#endif

        LOAD_RNNT_FUNC_IMPL(m_handle, CreateUnimicDecoder);
        LOAD_RNNT_FUNC_IMPL(m_handle, GetUnimicDecoderInputDim);
        LOAD_RNNT_FUNC_IMPL(m_handle, RunUnimicDecoder);
        LOAD_RNNT_FUNC_IMPL(m_handle, GetUnimicDecoderNBest);
        LOAD_RNNT_FUNC_IMPL(m_handle, DeleteUnimicDecoder);
        LOAD_RNNT_FUNC_IMPL(m_handle, ResetUnimicDecoder);
        LOAD_RNNT_FUNC_IMPL(m_handle, GetUnimicDecoderNBestCount);
        LOAD_RNNT_FUNC_IMPL(m_handle, GetUnimicDecoderNBestEntry);
        LOAD_RNNT_FUNC_IMPL(m_handle, DeleteUnimicDecoderNBest);
        LOAD_RNNT_FUNC_IMPL(m_handle, CreateCustomUnimicSource);
        LOAD_RNNT_FUNC_IMPL(m_handle, DeleteUnimicSource);
        LOAD_RNNT_FUNC_IMPL(m_handle, GetUnimicSourceSampleRate);
        LOAD_RNNT_FUNC_IMPL(m_handle, GetUnimicSourceChannelCount);
        LOAD_RNNT_FUNC_IMPL(m_handle, ReadUnimicSource);
        LOAD_RNNT_FUNC_IMPL(m_handle, CreateUnimicFilter);
        LOAD_RNNT_FUNC_IMPL(m_handle, DeleteUnimicFilter);
        LOAD_RNNT_FUNC_IMPL(m_handle, SetUnimicFilterInputPort);
        LOAD_RNNT_FUNC_IMPL(m_handle, GetUnimicFilterOutputPortCount);
        LOAD_RNNT_FUNC_IMPL(m_handle, GetUnimicFilterOutputPort);
    }

#ifdef _MSC_VER
    HMODULE m_handle;
#else
    void* m_handle;
#endif

    DECLARE_RNNT_FUNC_MEMBER(CreateUnimicDecoder)
    DECLARE_RNNT_FUNC_MEMBER(GetUnimicDecoderInputDim)
    DECLARE_RNNT_FUNC_MEMBER(RunUnimicDecoder)
    DECLARE_RNNT_FUNC_MEMBER(GetUnimicDecoderNBest)
    DECLARE_RNNT_FUNC_MEMBER(DeleteUnimicDecoder)
    DECLARE_RNNT_FUNC_MEMBER(ResetUnimicDecoder)
    DECLARE_RNNT_FUNC_MEMBER(GetUnimicDecoderNBestCount)
    DECLARE_RNNT_FUNC_MEMBER(GetUnimicDecoderNBestEntry)
    DECLARE_RNNT_FUNC_MEMBER(DeleteUnimicDecoderNBest)
    DECLARE_RNNT_FUNC_MEMBER(CreateCustomUnimicSource)
    DECLARE_RNNT_FUNC_MEMBER(DeleteUnimicSource)
    DECLARE_RNNT_FUNC_MEMBER(GetUnimicSourceSampleRate)
    DECLARE_RNNT_FUNC_MEMBER(GetUnimicSourceChannelCount)
    DECLARE_RNNT_FUNC_MEMBER(ReadUnimicSource)
    DECLARE_RNNT_FUNC_MEMBER(CreateUnimicFilter)
    DECLARE_RNNT_FUNC_MEMBER(DeleteUnimicFilter)
    DECLARE_RNNT_FUNC_MEMBER(SetUnimicFilterInputPort)
    DECLARE_RNNT_FUNC_MEMBER(GetUnimicFilterOutputPortCount)
    DECLARE_RNNT_FUNC_MEMBER(GetUnimicFilterOutputPort)
};

}}}}
