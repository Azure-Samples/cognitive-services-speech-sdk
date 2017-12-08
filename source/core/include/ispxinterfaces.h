//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// ISpxInterfaces.h: Implementation declarations for all ISpx* C++ interface classes
//

#pragma once
#include <memory>
#include <spxcore_common.h>
#include "asyncop.h"
#include "speechapi_cxx_common.h"
#include "speechapi_cxx_eventsignal.h"


using namespace CARBON_NAMESPACE_ROOT;


namespace CARBON_IMPL_NAMESPACE() {


class ISpxInterfaceBase : public std::enable_shared_from_this<ISpxInterfaceBase>
{
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
    };

    void Term() override
    {
    };


protected:

    ISpxObjectWithSiteInitImpl() : m_hasSite(false) {};

    bool m_hasSite;
    std::weak_ptr<T> m_site;
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

    virtual uint32_t GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat) = 0;
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


class ISpxAudioReaderPump : public ISpxInterfaceBaseFor<ISpxAudioReaderPump>
{
public:

    virtual void SetAudioReader(std::shared_ptr<ISpxAudioReader>& reader) = 0;
};


class ISpxAudioPump : public ISpxInterfaceBaseFor<ISpxAudioPump>
{
public:

    virtual uint32_t GetFormat(WAVEFORMATEX* pformat, uint32_t cbFormat) = 0;
    virtual void SetFormat(const WAVEFORMATEX* pformat, uint32_t cbFormat) = 0;

    virtual void StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor) = 0;
    virtual void PausePump() = 0;
    virtual void StopPump() = 0;

    enum class State { NoInput, Idle, Paused, Processing };
    virtual State GetState() = 0;
};


enum class Reason { Recognized, IntermediateResult, NoMatch, Canceled, OtherRecognizer };


class ISpxRecognitionResult : public ISpxInterfaceBaseFor<ISpxRecognitionResult>
{
public:

    virtual std::wstring GetResultId() = 0;
    virtual std::wstring GetText() = 0;

    virtual enum class Reason GetReason() = 0;
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


class ISpxRecognitionEventArgs : public ISpxSessionEventArgs, public ISpxInterfaceBaseFor<ISpxRecognitionEventArgs>
{
public:

    virtual std::shared_ptr<ISpxRecognitionResult> GetResult() = 0;
};


class ISpxRecognizerEvents : public ISpxInterfaceBaseFor<ISpxRecognizerEvents>
{
public:

    using RecoEvent_Type = EventSignal<std::shared_ptr<ISpxRecognitionEventArgs>>;
    using SessionEvent_Type = EventSignal<std::shared_ptr<ISpxSessionEventArgs>>;

    virtual void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) = 0;

    RecoEvent_Type IntermediateResult;
    RecoEvent_Type FinalResult;
    RecoEvent_Type NoMatch;
    RecoEvent_Type Canceled;
    

protected:

    ISpxRecognizerEvents(RecoEvent_Type::NotifyCallback_Type connectedCallback, RecoEvent_Type::NotifyCallback_Type disconnectedCallback) :
        FinalResult(connectedCallback, disconnectedCallback)
    {
    };

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


class ISpxRecoEngineAdapter : public ISpxAudioProcessor, public ISpxInterfaceBaseFor<ISpxRecoEngineAdapter>
{
};


class ISpxRecoEngineAdapterSite : public ISpxSite
{
public:

    using ResultPayload_Type = std::shared_ptr<ISpxRecognitionResult>;
    using AdditionalMessagePayload_Type = void*;
    using ErrorPayload_Type = SPXHR;
    
    virtual void SpeechStartDetected(ISpxRecoEngineAdapter* adapter, uint32_t offset) = 0;
    virtual void SpeechEndDetected(ISpxRecoEngineAdapter* adapter, uint32_t offset) = 0;

    virtual void SoundStartDetected(ISpxRecoEngineAdapter* adapter, uint32_t offset) = 0;
    virtual void SoundEndDetected(ISpxRecoEngineAdapter* adapter, uint32_t offset) = 0;

    virtual void IntermediateResult(ISpxRecoEngineAdapter* adapter, uint32_t offset, ResultPayload_Type payload) = 0;
    virtual void FinalResult(ISpxRecoEngineAdapter* adapter, uint32_t offset, ResultPayload_Type payload) = 0;

    virtual void DoneProcessingAudio(ISpxRecoEngineAdapter* adapter) = 0;

    virtual void AdditionalMessage(ISpxRecoEngineAdapter* adapter, uint32_t offset, AdditionalMessagePayload_Type payload) = 0;

    virtual void Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload) = 0;
};


}; // CARBON_IMPL_NAMESPACE()
