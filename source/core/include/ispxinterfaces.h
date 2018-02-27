//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// ISpxInterfaces.h: Implementation declarations for all ISpx* C++ interface classes
//

#pragma once
#include <memory>
#include "spxcore_common.h"
#include "platform.h"
#include "asyncop.h"
#include "speechapi_cxx_common.h"
#include "speechapi_cxx_eventsignal.h"


using namespace CARBON_NAMESPACE_ROOT;


namespace CARBON_IMPL_NAMESPACE() {


class ISpxInterfaceBase : public std::enable_shared_from_this<ISpxInterfaceBase>
{
public:

    virtual ~ISpxInterfaceBase() = default;


protected:

    typedef std::enable_shared_from_this<ISpxInterfaceBase> base_type;

    std::shared_ptr<ISpxInterfaceBase> shared_from_this()
    {
        return base_type::shared_from_this();
    }
};


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


class ISpxSite : public ISpxInterfaceBaseFor<ISpxSite>
{
};


class ISpxObjectWithSite : public ISpxInterfaceBaseFor<ISpxObjectWithSite>
{
public:

    virtual void SetSite(std::weak_ptr<ISpxSite> site) = 0;
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

    void SetSite(std::weak_ptr<ISpxSite> site) override
    {
        auto shared = site.lock();
        auto ptr = std::dynamic_pointer_cast<T>(shared);
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


class ISpxAudioFile : public ISpxInterfaceBaseFor<ISpxAudioFile>
{
public:

    virtual void Open(const wchar_t* pszFileName) = 0;
    virtual void Close() = 0;

    virtual bool IsOpen() const = 0;
};


class ISpxAudioReader : public ISpxInterfaceBaseFor<ISpxAudioReader>
{
public:

    virtual uint16_t GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat) = 0;
    virtual uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer) = 0;
    virtual void Close() = 0;
};


class ISpxAudioWriter : public ISpxInterfaceBaseFor<ISpxAudioWriter>
{
public:

    using AudioData_Type = std::shared_ptr<uint8_t>;
    
    virtual void SetFormat(WAVEFORMATEX* pformat) = 0;
    virtual void Write(AudioData_Type data, uint32_t cbData) = 0;
    virtual void Close() = 0;
};


class ISpxAudioProcessor : public ISpxInterfaceBaseFor<ISpxAudioProcessor>
{
public:

    using AudioData_Type = std::shared_ptr<uint8_t>;

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


enum class Reason { Recognized, IntermediateResult, NoMatch, Canceled, OtherRecognizer };


class ISpxRecognitionResult : public ISpxInterfaceBaseFor<ISpxRecognitionResult>
{
public:

    virtual std::wstring GetResultId() = 0;
    virtual std::wstring GetText() = 0;

    virtual enum Reason GetReason() = 0;
};


class ISpxRecognitionResultInit : public ISpxInterfaceBaseFor<ISpxRecognitionResultInit>
{
public:

    virtual void InitIntermediateResult(const wchar_t* resultId, const wchar_t* text) = 0;
    virtual void InitFinalResult(const wchar_t* resultId, const wchar_t* text) = 0;
    virtual void InitNoMatch() = 0;
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


class ISpxRecognitionEventArgs : public ISpxSessionEventArgs, public ISpxInterfaceBaseFor<ISpxRecognitionEventArgs>
{
public:

    virtual std::shared_ptr<ISpxRecognitionResult> GetResult() = 0;
};


class ISpxRecognitionEventArgsInit : public ISpxInterfaceBaseFor<ISpxRecognitionEventArgsInit>
{
public:

    virtual void Init(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) = 0;
};


class ISpxRecognizerEvents : public ISpxInterfaceBaseFor<ISpxRecognizerEvents>
{
public:

    using RecoEvent_Type = EventSignal<std::shared_ptr<ISpxRecognitionEventArgs>>;
    using SessionEvent_Type = EventSignal<std::shared_ptr<ISpxSessionEventArgs>>;

    virtual void FireSessionStarted(const std::wstring& sessionId) = 0;
    virtual void FireSessionStopped(const std::wstring& sessionId) = 0;
    
    virtual void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) = 0;

    SessionEvent_Type SessionStarted;
    SessionEvent_Type SessionStopped;

    RecoEvent_Type IntermediateResult;
    RecoEvent_Type FinalResult;
    RecoEvent_Type NoMatch;
    RecoEvent_Type Canceled;


protected:

    ISpxRecognizerEvents(RecoEvent_Type::NotifyCallback_Type connectedCallback, RecoEvent_Type::NotifyCallback_Type disconnectedCallback) :
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
};


class ISpxAudioStreamSessionInit : public ISpxInterfaceBaseFor<ISpxAudioStreamSessionInit>
{
public:

    virtual void InitFromFile(const wchar_t* pszFileName) = 0;
    virtual void InitFromMicrophone() = 0;
};


class ISpxSessionFromRecognizer : public ISpxInterfaceBaseFor<ISpxSessionFromRecognizer>
{
public:

    virtual std::shared_ptr<ISpxSession> GetDefaultSession() = 0;
};


class ISpxRecoEngineAdapter : public ISpxAudioProcessor, public ISpxInterfaceBaseFor<ISpxRecoEngineAdapter>
{
};


class ISpxRecoEngineAdapterSite : virtual public ISpxSite
{
public:

    using ResultPayload_Type = std::shared_ptr<ISpxRecognitionResult>;
    using AdditionalMessagePayload_Type = void*;
    using ErrorPayload_Type = SPXHR;
    
    virtual void SpeechStartDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;
    virtual void SpeechEndDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;

    virtual void SoundStartDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;
    virtual void SoundEndDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;

    virtual void IntermediateResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, ResultPayload_Type payload) = 0;
    virtual void FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, ResultPayload_Type payload) = 0;

    virtual void DoneProcessingAudio(ISpxRecoEngineAdapter* adapter) = 0;

    virtual void AdditionalMessage(ISpxRecoEngineAdapter* adapter, uint64_t offset, AdditionalMessagePayload_Type payload) = 0;

    virtual void Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload) = 0;
};


class ISpxRecoResultFactory : public ISpxInterfaceBaseFor<ISpxRecoResultFactory>
{
public:

    virtual std::shared_ptr<ISpxRecognitionResult> CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text) = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateFinalResult(const wchar_t* resultId, const wchar_t* text) = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateNoMatchResult() = 0;
};


class ISpxEventArgsFactory : public ISpxInterfaceBaseFor<ISpxEventArgsFactory>
{
public:

    virtual std::shared_ptr<ISpxSessionEventArgs> CreateSessionEventArgs(const std::wstring& sessionId) = 0;
    virtual std::shared_ptr<ISpxRecognitionEventArgs> CreateRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) = 0;
};


class ISpxRecognizerSite : virtual public ISpxSite
{
public:

    virtual std::shared_ptr<ISpxSession> GetDefaultSession() = 0;
};


class ISpxRecognizerFactory : public ISpxInterfaceBaseFor<ISpxRecognizerFactory>
{
public:

    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer() = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(bool passiveListeningEnaled) = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(const std::wstring& language) = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName) = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) = 0;
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


} // CARBON_IMPL_NAMESPACE
