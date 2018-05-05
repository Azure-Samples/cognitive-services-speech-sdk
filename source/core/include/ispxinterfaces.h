//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// ISpxInterfaces.h: Implementation declarations for all ISpx* C++ interface classes
//

#pragma once
#include <memory>
#include <vector>
#include <map>
#include "spxcore_common.h"
#include "platform.h"
#include "asyncop.h"
#include "speechapi_cxx_eventsignal.h"
#include "shared_ptr_helpers.h"
#include "speechapi_cxx_audioinputstream.h"


using namespace Microsoft::CognitiveServices::Speech;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxInterfaceBase : public std::enable_shared_from_this<ISpxInterfaceBase>
{
public:

    virtual ~ISpxInterfaceBase() = default;

    template <class I>
    std::shared_ptr<I> QueryInterface()
    {
        return QueryInterfaceInternal<I>();
    }


protected:

    template <class I>
    std::shared_ptr<I> QueryInterfaceInternal()
    {
        // try to query for the interface via our virtual method... 
        auto ptr = QueryInterface(PAL::GetTypeName<I>().c_str());
        if (ptr != nullptr)
        {
            auto interfacePtr = reinterpret_cast<I*>(ptr);
            return interfacePtr->shared_from_this();
        }

        // if that fails, let the caller know
        return nullptr;
    }

    virtual void* QueryInterface(const char* /*interfaceName*/) { return nullptr; }

    typedef std::enable_shared_from_this<ISpxInterfaceBase> base_type;

    std::shared_ptr<ISpxInterfaceBase> shared_from_this()
    {
        return base_type::shared_from_this();
    }
};


template <class I>
std::shared_ptr<I> SpxQueryInterface(std::shared_ptr<ISpxInterfaceBase> from)
{
    if (from != nullptr)
    {
        #if defined(_MSC_VER) && defined(_DEBUG)
            std::shared_ptr<I> ptr1 = std::dynamic_pointer_cast<I>(from);
            std::shared_ptr<I> ptr2 = from->QueryInterface<I>();
            SPX_TRACE_ERROR_IF(ptr1 != nullptr && ptr2 == nullptr, "dynamic_pointer_cast() and QueryInterface() do not agree!! UNEXPECTED!");
            SPX_TRACE_ERROR_IF(ptr1 == nullptr && ptr2 != nullptr, "dynamic_pointer_cast() and QueryInterface() do not agree!! UNEXPECTED!");
            SPX_IFTRUE_THROW_HR(ptr1 != nullptr && ptr2 == nullptr, SPXERR_ABORT);
            SPX_IFTRUE_THROW_HR(ptr1 == nullptr && ptr2 != nullptr, SPXERR_ABORT);
            return ptr1;
        #elif defined(_MSC_VER)
            std::shared_ptr<I> ptr = std::dynamic_pointer_cast<I>(from);
            return ptr != nullptr ? ptr : from->QueryInterface<I>();
        #else
            std::shared_ptr<I> ptr = from->QueryInterface<I>();
            return ptr != nullptr ? ptr : std::dynamic_pointer_cast<I>(from);
        #endif
    }
    return nullptr;
}


template<typename T>
struct ISpxInterfaceBaseFor : virtual public ISpxInterfaceBase
{
public:

    virtual ~ISpxInterfaceBaseFor() = default;

    std::shared_ptr<T> shared_from_this()
    {
        std::shared_ptr<T> result(base_type::shared_from_this(), static_cast<T*>(this));
        return result;
    }


private:

    typedef ISpxInterfaceBase base_type;

    ISpxInterfaceBaseFor&& operator =(const ISpxInterfaceBaseFor&&) = delete;
};


class ISpxObjectInit : public ISpxInterfaceBaseFor<ISpxObjectInit>
{
public:

    virtual void Init() = 0;
    virtual void Term() = 0;
};


class ISpxGenericSite : public ISpxInterfaceBaseFor<ISpxGenericSite>
{
};


class ISpxObjectWithSite : public ISpxInterfaceBaseFor<ISpxObjectWithSite>
{
public:

    virtual void SetSite(std::weak_ptr<ISpxGenericSite> site) = 0;
};


template <class T, class I, class... Types>
inline std::shared_ptr<I> SpxCreateObjectInternal(Types&&... Args)
{
    SPX_DBG_TRACE_VERBOSE("Creating object via %s: %s as %s", __FUNCTION__, PAL::GetTypeName<T>(), PAL::GetTypeName<I>());
    std::shared_ptr<T> ptr = std::make_shared<T>(std::forward<Types>(Args)...);
    auto it = std::dynamic_pointer_cast<I>(ptr);
    return it;
}


class ISpxObjectFactory : public ISpxInterfaceBaseFor<ISpxObjectFactory>
{
public:

    template <class I>
    std::shared_ptr<I> CreateObject(const char* className)
    {
        // try to create the object from our interface virtual method... 
        auto obj = CreateObject(className, PAL::GetTypeName<I>().c_str());
        if (obj != nullptr)
        {
            auto ptr = reinterpret_cast<I*>(obj);
            return std::shared_ptr<I>(ptr);
        }

        // if we can't, return nullptr to let the caller know
        return nullptr;
    }

    template <class T, class I>
    std::shared_ptr<I> CreateObject()
    {
        auto obj = CreateObject<I>(PAL::GetTypeName<T>());
        if (obj != nullptr)
        {
            return obj;
        }

        // if that didn't work, just go ahead and delegate to our internal helper for this module
        return SpxCreateObjectInternal<T, I>();
    }

    virtual void* CreateObject(const char* className, const char* interfaceName) = 0;
};


template <class T>
class ISpxObjectWithSiteInitImpl : public ISpxObjectWithSite, public ISpxObjectInit
{
public:

    // --- ISpxObjectWithSite

    void SetSite(std::weak_ptr<ISpxGenericSite> site) override
    {
        auto shared = site.lock();
        auto ptr = SpxQueryInterface<T>(shared);
        SPX_IFFALSE_THROW_HR((bool)ptr == (bool)shared, SPXERR_INVALID_ARG);

        if (m_hasSite)
        {
            Term();
            m_site.reset();
            m_hasSite = false;
        }

        m_site = ptr;
        m_hasSite = ptr.get() != nullptr;

        if (m_hasSite)
        {
            Init();
        }
    }

    // --- ISpxObjectInit

    void Init() override
    {
    }

    void Term() override
    {
    }


protected:

    ISpxObjectWithSiteInitImpl() : m_hasSite(false) {}

    std::shared_ptr<T> GetSite()
    {
        return m_site.lock();
    }


private:

    bool m_hasSite;
    std::weak_ptr<T> m_site;
};

class ISpxServiceProvider : public ISpxInterfaceBaseFor<ISpxServiceProvider>
{
public:

    virtual std::shared_ptr<ISpxInterfaceBase> QueryService(const char* serviceName) = 0;
};


class ISpxAddServiceProvider : public ISpxInterfaceBaseFor<ISpxAddServiceProvider>
{
public:

    template <class T>
    void AddService(std::shared_ptr<T> service)
    {
        AddService(PAL::GetTypeName<T>(), service);
    }

    virtual void AddService(const char* serviceName, std::shared_ptr<ISpxInterfaceBase> service) = 0;
};


#pragma pack (push, 1)
struct WAVEFORMAT
{
    uint16_t wFormatTag;        /* format type */
    uint16_t nChannels;         /* number of channels (i.e. mono, stereo...) */
    uint32_t nSamplesPerSec;    /* sample rate */
    uint32_t nAvgBytesPerSec;   /* for buffer estimation */
    uint16_t nBlockAlign;       /* block size of data */
    uint16_t wBitsPerSample;    /* Number of bits per sample of mono data */
};

struct WAVEFORMATEX
{
    uint16_t wFormatTag;        /* format type */
    uint16_t nChannels;         /* number of channels (i.e. mono, stereo...) */
    uint32_t nSamplesPerSec;    /* sample rate */
    uint32_t nAvgBytesPerSec;   /* for buffer estimation */
    uint16_t nBlockAlign;       /* block size of data */
    uint16_t wBitsPerSample;    /* Number of bits per sample of mono data */
    uint16_t cbSize;            /* The count in bytes of the size of extra information (after cbSize) */
};
#pragma pack (pop)

#define WAVE_FORMAT_PCM 0x0001 // wFormatTag value for PCM data


using SpxWAVEFORMATEX_Type = std::shared_ptr<WAVEFORMATEX>;
inline SpxWAVEFORMATEX_Type SpxAllocWAVEFORMATEX(size_t sizeInBytes)
{
    return SpxAllocSharedBuffer<WAVEFORMATEX>(sizeInBytes);
}

using SpxSharedAudioBuffer_Type = SpxSharedUint8Buffer_Type;
inline SpxSharedAudioBuffer_Type SpxAllocSharedAudioBuffer(size_t sizeInBytes)
{
    return SpxAllocSharedUint8Buffer(sizeInBytes);
}


class ISpxAudioReader : public ISpxInterfaceBaseFor<ISpxAudioReader>
{
public:

    virtual uint16_t GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat) = 0;
    virtual uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer) = 0;
    virtual void Close() = 0;
};


class ISpxAudioReaderRealTime : public ISpxInterfaceBaseFor<ISpxAudioReaderRealTime>
{
public:

    virtual void SetRealTimePercentage(uint8_t percentage) = 0;
};


class ISpxAudioWriter : public ISpxInterfaceBaseFor<ISpxAudioWriter>
{
public:

    using AudioData_Type = SpxSharedAudioBuffer_Type;
    
    virtual void SetFormat(WAVEFORMATEX* pformat) = 0;
    virtual void Write(AudioData_Type data, uint32_t cbData) = 0;
    virtual void Close() = 0;
};


class ISpxAudioFile : public ISpxInterfaceBaseFor<ISpxAudioFile>
{
public:

    virtual void Open(const wchar_t* pszFileName) = 0;
    virtual void Close() = 0;

    virtual bool IsOpen() const = 0;

    virtual void SetContinuousLoop(bool value) = 0;
    virtual void SetIterativeLoop(bool value) = 0;
    virtual void SetRealTimePercentage(uint8_t percentage) = 0;
};


class ISpxAudioProcessor : public ISpxInterfaceBaseFor<ISpxAudioProcessor>
{
public:

    using AudioData_Type = SpxSharedAudioBuffer_Type;

    virtual void SetFormat(WAVEFORMATEX* pformat) = 0;
    virtual void ProcessAudio(AudioData_Type data, uint32_t cbData) = 0;
};


class ISpxAudioPump : public ISpxInterfaceBaseFor<ISpxAudioPump>
{
public:

    virtual uint16_t GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat) = 0;
    virtual void SetFormat(const WAVEFORMATEX* pformat, uint16_t cbFormat) = 0;

    virtual void StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor) = 0;
    virtual void PausePump() = 0;
    virtual void StopPump() = 0;

    enum class State { NoInput, Idle, Paused, Processing };
    virtual State GetState() = 0;
};


class ISpxAudioPumpReaderInit : public ISpxInterfaceBaseFor<ISpxAudioPumpReaderInit>
{
public:

    virtual void SetAudioReader(std::shared_ptr<ISpxAudioReader>& reader) = 0;
};

class ISpxStreamPumpReaderInit : public ISpxInterfaceBaseFor<ISpxStreamPumpReaderInit>
{
public:

    virtual void SetAudioStream(AudioInputStream* reader) = 0;
};


class ISpxKwsModel : public ISpxInterfaceBaseFor<ISpxKwsModel>
{
public:

    virtual void InitFromFile(const wchar_t* fileName) = 0;
    virtual std::wstring GetFileName() const = 0;
};


enum class Reason { Recognized, IntermediateResult, NoMatch, Canceled, OtherRecognizer };

enum class ResultType { Unknown, Speech, TranslationText, TranslationSynthesis };

class ISpxRecognitionResult : public ISpxInterfaceBaseFor<ISpxRecognitionResult>
{
public:

    virtual std::wstring GetResultId() = 0;
    virtual std::wstring GetText() = 0;
    virtual Reason GetReason() = 0;
    virtual ResultType GetType() = 0;
};


class ISpxRecognitionResultInit : public ISpxInterfaceBaseFor<ISpxRecognitionResultInit>
{
public:

    virtual void InitIntermediateResult(const wchar_t* resultId, const wchar_t* text, ResultType type) = 0;
    virtual void InitFinalResult(const wchar_t* resultId, const wchar_t* text, ResultType type) = 0;
    virtual void InitNoMatch(ResultType type) = 0;
    virtual void InitError(const wchar_t* text, ResultType type) = 0;
};


class ISpxRecognizer : public ISpxInterfaceBaseFor<ISpxRecognizer>
{
public:

    virtual bool IsEnabled() = 0;
    virtual void Enable() = 0;
    virtual void Disable() = 0;

    virtual CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync() = 0;
    virtual CSpxAsyncOp<void> StartContinuousRecognitionAsync() = 0;
    virtual CSpxAsyncOp<void> StopContinuousRecognitionAsync() = 0;

    virtual CSpxAsyncOp<void> StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model) = 0;
    virtual CSpxAsyncOp<void> StopKeywordRecognitionAsync() = 0;
};


class ISpxSessionEventArgs : public ISpxInterfaceBaseFor<ISpxSessionEventArgs>
{
public:

    virtual const std::wstring& GetSessionId() = 0;
};


class ISpxSessionEventArgsInit : public ISpxInterfaceBaseFor<ISpxSessionEventArgsInit>
{
public:

    virtual void Init(const std::wstring& sessionId) = 0;
};


class ISpxRecognitionEventArgs :
    public ISpxSessionEventArgs,
    public ISpxInterfaceBaseFor<ISpxRecognitionEventArgs>
{
public:

    virtual const uint64_t& GetOffset() = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> GetResult() = 0;
};


class ISpxRecognitionEventArgsInit : public ISpxInterfaceBaseFor<ISpxRecognitionEventArgsInit>
{
public:

    virtual void Init(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) = 0;
    virtual void Init(const std::wstring& sessionId, uint64_t offset) = 0;
};


class ISpxRecognizerEvents : public ISpxInterfaceBaseFor<ISpxRecognizerEvents>
{
public:

    using RecoEvent_Type = EventSignal<std::shared_ptr<ISpxRecognitionEventArgs>>;
    using SessionEvent_Type = EventSignal<std::shared_ptr<ISpxSessionEventArgs>>;

    virtual void FireSessionStarted(const std::wstring& sessionId) = 0;
    virtual void FireSessionStopped(const std::wstring& sessionId) = 0;

    virtual void FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset) = 0;
    virtual void FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset) = 0;
    
    virtual void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) = 0;

    SessionEvent_Type SessionStarted;
    SessionEvent_Type SessionStopped;

    RecoEvent_Type SpeechStartDetected;
    RecoEvent_Type SpeechEndDetected;

    RecoEvent_Type IntermediateResult;
    RecoEvent_Type FinalResult;
    RecoEvent_Type NoMatch;
    RecoEvent_Type Canceled;
    RecoEvent_Type TranslationSynthesisResult;

protected:

    ISpxRecognizerEvents(RecoEvent_Type::NotifyCallback_Type connectedCallback, RecoEvent_Type::NotifyCallback_Type disconnectedCallback) :
        SpeechStartDetected(connectedCallback, disconnectedCallback),
        SpeechEndDetected(connectedCallback, disconnectedCallback),
        IntermediateResult(connectedCallback, disconnectedCallback),
        FinalResult(connectedCallback, disconnectedCallback),
        NoMatch(connectedCallback, disconnectedCallback),
        Canceled(connectedCallback, disconnectedCallback)
    {
    }


private:

    ISpxRecognizerEvents() = delete;
};


class ISpxSession : public ISpxInterfaceBaseFor<ISpxSession>
{
public:

    virtual const std::wstring& GetSessionId() const = 0;

    virtual void AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer) = 0;
    virtual void RemoveRecognizer(ISpxRecognizer* recognizer) = 0;

    virtual CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync() = 0;
    virtual CSpxAsyncOp<void> StartContinuousRecognitionAsync() = 0;
    virtual CSpxAsyncOp<void> StopContinuousRecognitionAsync() = 0;

    virtual CSpxAsyncOp<void> StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model) = 0;
    virtual CSpxAsyncOp<void> StopKeywordRecognitionAsync() = 0;
};


class ISpxAudioStreamSessionInit : public ISpxInterfaceBaseFor<ISpxAudioStreamSessionInit>
{
public:

    virtual void InitFromFile(const wchar_t* pszFileName) = 0;
    virtual void InitFromMicrophone() = 0;
    virtual void InitFromStream(AudioInputStream* audioInputStream) = 0;
};


class ISpxSessionFromRecognizer : public ISpxInterfaceBaseFor<ISpxSessionFromRecognizer>
{
public:

    virtual std::shared_ptr<ISpxSession> GetDefaultSession() = 0;
};


class ISpxRecoEngineAdapter : 
    public ISpxAudioProcessor, 
    public ISpxInterfaceBaseFor<ISpxRecoEngineAdapter>
{
public:

    virtual void SetAdapterMode(bool singleShot) = 0;
};


class ISpxRecoEngineAdapterSite : public ISpxInterfaceBaseFor<ISpxRecoEngineAdapterSite>
{
public:

    using ResultPayload_Type = std::shared_ptr<ISpxRecognitionResult>;
    using AdditionalMessagePayload_Type = void*;
    using ErrorPayload_Type = const std::string&;

    virtual void GetScenarioCount(uint16_t* countSpeech, uint16_t* countIntent, uint16_t* countTranslation) = 0;

    virtual std::list<std::string> GetListenForList() = 0;
    virtual void GetIntentInfo(std::string& provider, std::string& id, std::string& key, std::string& region) = 0;

    virtual void AdapterStartingTurn(ISpxRecoEngineAdapter* adapter) = 0;
    virtual void AdapterStartedTurn(ISpxRecoEngineAdapter* adapter, const std::string& id) = 0;
    virtual void AdapterStoppedTurn(ISpxRecoEngineAdapter* adapter) = 0;

    virtual void AdapterDetectedSpeechStart(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;
    virtual void AdapterDetectedSpeechEnd(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;

    virtual void AdapterDetectedSoundStart(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;
    virtual void AdapterDetectedSoundEnd(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;

    virtual void FireAdapterResult_Intermediate(ISpxRecoEngineAdapter* adapter, uint64_t offset, ResultPayload_Type payload) = 0;
    virtual void FireAdapterResult_FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, ResultPayload_Type payload) = 0;
    virtual void FireAdapterResult_TranslationSynthesis(ISpxRecoEngineAdapter* adapter, ResultPayload_Type payload) = 0;

    virtual void AdapterRequestingAudioMute(ISpxRecoEngineAdapter* adapter, bool mute) = 0;
    virtual void AdapterCompletedSetFormatStop(ISpxRecoEngineAdapter* adapter) = 0;

    virtual void AdditionalMessage(ISpxRecoEngineAdapter* adapter, uint64_t offset, AdditionalMessagePayload_Type payload) = 0;

    virtual void Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload) = 0;
};


class ISpxKwsEngineAdapter : 
    public ISpxAudioProcessor, 
    public ISpxInterfaceBaseFor<ISpxKwsEngineAdapter>
{
};


class ISpxKwsEngineAdapterSite : public ISpxInterfaceBaseFor<ISpxKwsEngineAdapterSite>
{
public:

    virtual void KeywordDetected(ISpxKwsEngineAdapter* adapter, uint64_t startOffset, uint32_t size, SpxSharedAudioBuffer_Type audioBuffer) = 0;
    virtual void AdapterCompletedSetFormatStop(ISpxKwsEngineAdapter* adapter) = 0;
};


class ISpxRecoResultFactory : public ISpxInterfaceBaseFor<ISpxRecoResultFactory>
{
public:

    virtual std::shared_ptr<ISpxRecognitionResult> CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text, ResultType type) = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateFinalResult(const wchar_t* resultId, const wchar_t* text, ResultType type) = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateNoMatchResult(ResultType type) = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateErrorResult(const wchar_t* text, ResultType type) = 0;
};


class ISpxEventArgsFactory : public ISpxInterfaceBaseFor<ISpxEventArgsFactory>
{
public:

    virtual std::shared_ptr<ISpxSessionEventArgs> CreateSessionEventArgs(const std::wstring& sessionId) = 0;
    virtual std::shared_ptr<ISpxRecognitionEventArgs> CreateRecognitionEventArgs(const std::wstring& sessionId, uint64_t offset) = 0;
    virtual std::shared_ptr<ISpxRecognitionEventArgs> CreateRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) = 0;
};


class ISpxRecognizerSite : public ISpxInterfaceBaseFor<ISpxRecognizerSite>
{
public:

    virtual std::shared_ptr<ISpxSession> GetDefaultSession() = 0;
};


class ISpxSpeechApiFactory : public ISpxInterfaceBaseFor<ISpxSpeechApiFactory>
{
public:

    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer() = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithStream(AudioInputStream*) = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(const std::wstring& language) = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName) = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) = 0;

    virtual std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer() = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer(const std::wstring& language) = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName) = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) = 0;

    virtual std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizer(const std::wstring& sourcelanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice = L"") = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourcelanguae, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice = L"") = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerWithStream(AudioInputStream *stream, const std::wstring& sourcelanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice = L"") = 0;

};


class ISpxNamedProperties : public ISpxInterfaceBaseFor<ISpxNamedProperties>
{
public:

    virtual std::wstring GetStringValue(const wchar_t* name, const wchar_t* defaultValue = L"") = 0;
    virtual void SetStringValue(const wchar_t* name, const wchar_t* value) = 0;
    virtual bool HasStringValue(const wchar_t* name) = 0;

    virtual double GetNumberValue(const wchar_t* name, double defaultValue = 0) = 0;
    virtual void SetNumberValue(const wchar_t* name, double value) = 0;
    virtual bool HasNumberValue(const wchar_t* name) = 0;

    virtual bool GetBooleanValue(const wchar_t* name, bool defaultValue = false) = 0;
    virtual void SetBooleanValue(const wchar_t* name, bool value) = 0;
    virtual bool HasBooleanValue(const wchar_t* name) = 0;
};


class ISpxIntentRecognitionResult : public ISpxInterfaceBaseFor<ISpxIntentRecognitionResult>
{
public:

    virtual std::wstring GetIntentId() = 0;
};


class ISpxIntentRecognitionResultInit : public ISpxInterfaceBaseFor<ISpxIntentRecognitionResultInit>
{
public:

    virtual void InitIntentResult(const wchar_t* intentId, const wchar_t* jsonPayload) = 0;
};

enum class TranslationStatus { Success, Error };

class ISpxTranslationTextResult : public ISpxInterfaceBaseFor<ISpxTranslationTextResult>
{
public:
    virtual TranslationStatus GetTranslationStatus() const = 0;
    virtual const std::wstring& GetTranslationFailureReason() const = 0;
    virtual const std::map<std::wstring, std::wstring>& GetTranslationText() = 0;
};

class ISpxTranslationTextResultInit : public ISpxInterfaceBaseFor<ISpxTranslationTextResultInit>
{
public:
    virtual void InitTranslationTextResult(TranslationStatus status, const std::map<std::wstring, std::wstring>& translations, const std::wstring& failureReason) = 0;
};

enum class SynthesisStatus { Success, SynthesisEnd, Error };

class ISpxTranslationSynthesisResult : public ISpxInterfaceBaseFor<ISpxTranslationSynthesisResult>
{
public:
    virtual SynthesisStatus GetSynthesisStatus() = 0;
    virtual const std::wstring& GetSynthesisFailureReason() = 0;
    virtual const uint8_t* GetAudio() const = 0;
    virtual size_t GetLength() const = 0;
};

class ISpxTranslationSynthesisResultInit : public ISpxInterfaceBaseFor<ISpxTranslationSynthesisResultInit>
{
public:

    virtual void InitTranslationSynthesisResult(SynthesisStatus status, const uint8_t* audioData, size_t audioLength, const std::wstring& failureReason) = 0;
};

class ISpxLanguageUnderstandingModel : public ISpxInterfaceBaseFor<ISpxLanguageUnderstandingModel>
{
public:

    virtual void InitAppId(const wchar_t* appId) = 0;
    virtual void InitEndpoint(const wchar_t* uri) = 0;
    virtual void InitSubscription(const wchar_t* subscriptionKey, const wchar_t* appId, const wchar_t* region) = 0;

    virtual std::wstring GetEndpoint() const = 0;
    virtual std::wstring GetHostName() const = 0;
    virtual std::wstring GetPathAndQuery() const = 0;

    virtual std::wstring GetSubscriptionKey() const = 0;
    virtual std::wstring GetAppId() const = 0;
    virtual std::wstring GetRegion() const = 0;
};


class ISpxTrigger : public ISpxInterfaceBaseFor<ISpxTrigger>
{
public:

    virtual void InitPhraseTrigger(const wchar_t* phrase) = 0;
    virtual void InitLanguageUnderstandingModelTrigger(std::shared_ptr<ISpxLanguageUnderstandingModel> model, const wchar_t* intentName) = 0;

    virtual std::wstring GetPhrase() const = 0;

    virtual std::shared_ptr<ISpxLanguageUnderstandingModel> GetModel() const = 0;
    virtual std::wstring GetModelIntentName() const = 0;
};


class ISpxIntentTriggerService : public ISpxInterfaceBaseFor<ISpxIntentTriggerService>
{
public:

    virtual void AddIntentTrigger(const wchar_t* id, std::shared_ptr<ISpxTrigger> trigger) = 0;

    virtual std::list<std::string> GetListenForList() = 0;
    virtual void GetIntentInfo(std::string& provider, std::string& id, std::string& key, std::string& region) = 0;
};


class ISpxIntentRecognizer : public ISpxInterfaceBaseFor<ISpxIntentRecognizer>
{
public:

    virtual void AddIntentTrigger(const wchar_t* intentId, std::shared_ptr<ISpxTrigger> trigger) = 0;

    // TODO: RobCh: Add additional methods required... 
};

class ISpxTranslationRecognizer : public ISpxInterfaceBaseFor<ISpxTranslationRecognizer>
{

};


class ISpxRecognitionResultProcessor : public ISpxInterfaceBaseFor<ISpxRecognitionResultProcessor>
{
public:

    virtual void ProcessResult(std::shared_ptr<ISpxRecognitionResult> result) = 0;
};


class ISpxLuEngineAdapter :
    public ISpxRecognitionResultProcessor,
    public ISpxInterfaceBaseFor<ISpxLuEngineAdapter>
{
};


class ISpxLuEngineAdapterSite : public ISpxInterfaceBaseFor<ISpxLuEngineAdapterSite>
{
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
