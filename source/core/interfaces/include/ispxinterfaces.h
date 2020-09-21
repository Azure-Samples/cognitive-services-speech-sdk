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
#include <unordered_map>
#include <chrono>
#include "spxcore_common.h"
#include "platform.h"
#include "asyncop.h"
#include "speechapi_cxx_eventsignal.h"
#include "speechapi_cxx_enums.h"
#include "speechapi_cxx_string_helpers.h"
#include "shared_ptr_helpers.h"
#include "spxdebug.h"
#include <cstring>

#include <interfaces/aggregates.h>
#include <interfaces/audio.h>
#include <interfaces/base.h>
#include <interfaces/containers.h>
#include <interfaces/conversation.h>
#include <interfaces/conversation_translation.h>
#include <interfaces/data.h>
#include <interfaces/errors.h>
#include <interfaces/event_args.h>
#include <interfaces/keyword.h>
#include <interfaces/notify_me.h>
#include <interfaces/recognizers.h>
#include <interfaces/results.h>
#include <interfaces/types.h>

#include <interfaces/ispxobjectinit.h>
#include <interfaces/ispxobjectwithsite.h>
#include <interfaces/ispxgenericsite.h>
#include <interfaces/ispxsession2.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <class T, class I, class... Types>
std::shared_ptr<I> SpxCreateObjectInternal(Types&&... Args)
{
    SPX_DBG_TRACE_VERBOSE("Creating object via %s: %s as %s", __FUNCTION__, SpxTypeName(T), SpxTypeName(I));
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
        auto obj = CreateObject(className, SpxTypeName(I));
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
        auto obj = CreateObject<I>(SpxTypeName(T));
        if (obj != nullptr)
        {
            return obj;
        }

        // if that didn't work, just go ahead and delegate to our internal helper for this module
        return SpxCreateObjectInternal<T, I>();
    }

    virtual void* CreateObject(const char* className, const char* interfaceName) = 0;
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
        AddService(SpxTypeName(T), service);
    }

    virtual void AddService(const char* serviceName, std::shared_ptr<ISpxInterfaceBase> service) = 0;
};

using SpxWAVEFORMATEX_Type = std::shared_ptr<SPXWAVEFORMATEX>;
inline SpxWAVEFORMATEX_Type SpxAllocWAVEFORMATEX(size_t sizeInBytes)
{
    return SpxAllocSharedBuffer<SPXWAVEFORMATEX>(sizeInBytes);
}

inline SpxWAVEFORMATEX_Type SpxCopyWAVEFORMATEX(const SPXWAVEFORMATEX* format)
{
    SPX_IFTRUE_RETURN_X(format == nullptr, nullptr);

    auto size = sizeof(SPXWAVEFORMATEX) + format->cbSize;
    auto copy = SpxAllocWAVEFORMATEX(size);
    memcpy(copy.get(), format, size);

    return copy;
}

inline SpxWAVEFORMATEX_Type SpxCopyWAVEFORMATEX(const SPXWAVEFORMATEX& format)
{
    return SpxCopyWAVEFORMATEX(&format);
}

inline SpxWAVEFORMATEX_Type SpxCopyWAVEFORMATEX(const SpxWAVEFORMATEX_Type& format)
{
    return SpxCopyWAVEFORMATEX(format.get());
}

inline uint16_t SpxCopyWAVEFORMATEX(SpxWAVEFORMATEX_Type source, SPXWAVEFORMATEX* dest, uint16_t destSize)
{
    auto sourceSize = uint16_t(sizeof(SPXWAVEFORMATEX) + source->cbSize);
    if (dest != nullptr)
    {
        auto copySize = std::min<uint16_t>(destSize, sourceSize);
        memcpy(dest, source.get(), copySize);
        return copySize;
    }
    return sourceSize;
}

#define SPX_TRACE_VERBOSE_WAVEFORMAT(format) \
    SPX_TRACE_VERBOSE_IF(format == nullptr, "%s - format == nullptr", __FUNCTION__); \
    SPX_TRACE_VERBOSE_IF(format != nullptr, "%s\n  wFormatTag:      %s\n  nChannels:       %d\n  nSamplesPerSec:  %d\n  nAvgBytesPerSec: %d\n  nBlockAlign:     %d\n  wBitsPerSample:  %d\n  cbSize:          %d", \
        __FUNCTION__, \
        format->wFormatTag == WAVE_FORMAT_PCM ? "PCM" : std::to_string(format->wFormatTag).c_str(), \
        format->nChannels, \
        format->nSamplesPerSec, \
        format->nAvgBytesPerSec, \
        format->nBlockAlign, \
        format->wBitsPerSample, \
        format->cbSize);

using SpxSharedAudioBuffer_Type = SpxSharedUint8Buffer_Type;
inline SpxSharedAudioBuffer_Type SpxAllocSharedAudioBuffer(size_t sizeInBytes)
{
    return SpxAllocSharedUint8Buffer(sizeInBytes);
}



class ISpxAudioOutputFormat : public ISpxInterfaceBaseFor<ISpxAudioOutputFormat>
{
public:
    virtual bool HasHeader() = 0;
    virtual std::string GetFormatString() = 0;
    virtual std::string GetRawFormatString() = 0;
};

class ISpxAudioOutputInitFormat : public ISpxInterfaceBaseFor<ISpxAudioOutputInitFormat>
{
public:
    virtual void SetHeader(bool hasHeader) = 0;
    virtual void SetFormatString(const std::string& formatString) = 0;
    virtual void SetRawFormatString(const std::string& rawFormatString) = 0;
};

class ISpxAudioStreamReaderInitCallbacks : public ISpxInterfaceBaseFor<ISpxAudioStreamReaderInitCallbacks>
{
public:

    using ReadCallbackFunction_Type = std::function<int(uint8_t*, uint32_t)>;
    using CloseCallbackFunction_Type = std::function<void()>;
    using GetPropertyCallbackFunction_Type = std::function<void(PropertyId, uint8_t*, uint32_t)>;
    using GetPropertyCallbackFunction_Type2 = std::function<SPXSTRING(PropertyId)>;

    virtual void SetCallbacks(ReadCallbackFunction_Type readCallback, CloseCallbackFunction_Type closeCallback) = 0;

    virtual void SetPropertyCallback(GetPropertyCallbackFunction_Type getPropertyCallBack) { UNUSED(getPropertyCallBack); }
    virtual void SetPropertyCallback2(GetPropertyCallbackFunction_Type2 getPropertyCallBack) { UNUSED(getPropertyCallBack); }
};

class ISpxAudioStreamWriterInitCallbacks : public ISpxInterfaceBaseFor<ISpxAudioStreamWriterInitCallbacks>
{
public:
    using WriteCallbackFunction_Type = std::function<int(uint8_t*, uint32_t)>;
    using CloseCallbackFunction_Type = std::function<void()>;

    virtual void SetCallbacks(WriteCallbackFunction_Type writeCallback, CloseCallbackFunction_Type closeCallback) = 0;
};

class ISpxAudioStreamReader : public ISpxInterfaceBaseFor<ISpxAudioStreamReader>
{
public:
    virtual uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat) = 0;
    virtual uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer) = 0;
    // We pull this so that the user can give us timestamp or speaker id that are associated with the audio data.
    virtual SPXSTRING GetProperty(PropertyId propertyId) { UNUSED(propertyId); return ""; }

    virtual void Close() = 0;
};

class ISpxAudioStreamReaderFactory : public ISpxInterfaceBaseFor<ISpxAudioStreamReaderFactory>
{
public:
    virtual std::shared_ptr<ISpxAudioStreamReader> CreateReader() = 0;
};

class ISpxSingleToManyStreamReaderAdapter : public ISpxInterfaceBaseFor<ISpxSingleToManyStreamReaderAdapter>
{
public:
    // The singleton is expected to implement ISpxAudioStreamReader and ISpxObjectInit so it can be reopened
    virtual void SetSingletonReader(std::shared_ptr<ISpxAudioStreamReader> singletonReader) = 0;
};

class ISpxSingleToManyStreamReaderAdapterSite : public ISpxInterfaceBaseFor<ISpxSingleToManyStreamReaderAdapterSite>
{
public:
    virtual void ReconnectClient(long clientId, std::shared_ptr<ISpxAudioStreamReader>&& reader) = 0;
    virtual void DisconnectClient(long clientId) = 0;
};

class ISpxSetErrorInfo : public ISpxInterfaceBaseFor<ISpxSetErrorInfo>
{
public:
    virtual void SetError(const std::string& error) = 0;
};

class ISpxAudioStreamWriter : public ISpxInterfaceBaseFor<ISpxAudioStreamWriter>
{
public:
    virtual void Write(uint8_t* buffer, uint32_t size) = 0;
    virtual void SetProperty(PropertyId propertyId, const SPXSTRING& value) = 0;
    virtual void SetProperty(const SPXSTRING& name, const SPXSTRING& value) = 0;
};

class ISpxAudioFile : public ISpxInterfaceBaseFor<ISpxAudioFile>
{
public:
    virtual void Open(const wchar_t* pszFileName) = 0;
    virtual void Close() = 0;

    virtual bool IsOpen() const = 0;

    virtual void SetContinuousLoop(bool value) = 0;
    virtual void SetIterativeLoop(bool value) = 0;
};

class ISpxAudioOutput : public ISpxInterfaceBaseFor<ISpxAudioOutput>
{
public:
    virtual uint32_t Write(uint8_t* buffer, uint32_t size) = 0;
    virtual void WaitUntilDone() = 0;
    virtual void ClearUnread() = 0;
    virtual void Close() = 0;
};

class ISpxAudioRender : public ISpxInterfaceBaseFor<ISpxAudioOutput>
{
public:
    virtual void StartPlayback() = 0;
    virtual void PausePlayback() = 0;
    virtual void StopPlayback() = 0;
};

class ISpxAudioOutputReader : public ISpxInterfaceBaseFor<ISpxAudioOutputReader>
{
public:
    virtual uint32_t Read(uint8_t* buffer, uint32_t bufferSize) = 0;
    virtual uint32_t AvailableSize() = 0;
};

class ISpxUser : public ISpxInterfaceBaseFor<ISpxUser>
{
public:
    virtual void InitFromUserId(const char* pszUserId) = 0;
    virtual std::string GetId() const = 0;
};

class ISpxAudioConfig : public ISpxInterfaceBaseFor<ISpxAudioConfig>
{
public:
    virtual void InitFromDefaultDevice() = 0;
    virtual void InitFromFile(const wchar_t* pszFileName) = 0;
    virtual void InitFromStream(std::shared_ptr<ISpxAudioStream> stream) = 0;

    virtual std::wstring GetFileName() const = 0;
    virtual std::shared_ptr<ISpxAudioStream> GetStream() = 0;
};

class ISpxInternalAudioCodecAdapter : public ISpxInterfaceBaseFor <ISpxInternalAudioCodecAdapter>
{
public:
    using SPXCompressedDataCallback = std::function<void(const uint8_t * outData, size_t nBytesOut)>;

    virtual SPXHR Load(const std::string& modulename, const std::string& codec, SPXCompressedDataCallback dataCallback) = 0;
    virtual void InitCodec(const SPXWAVEFORMATEX* inputFormat) = 0;
    virtual std::string GetContentType() = 0;
    virtual void Encode(const uint8_t* buffer, size_t bufferSize) = 0;
    virtual void Flush() = 0;
    virtual void CloseEncodingStream() = 0;
};


class ISpxReadWriteBufferInit : public ISpxInterfaceBaseFor<ISpxReadWriteBufferInit>
{
public:

    virtual size_t SetSize(size_t size) = 0;
    virtual void SetInitPos(uint64_t pos) = 0;
    virtual void AllowOverflow(bool allow) = 0;

    virtual void SetName(const std::string& name) = 0;

    virtual void Term() = 0;
};

class ISpxReadWriteBuffer : public ISpxInterfaceBaseFor<ISpxReadWriteBuffer>
{
public:

    virtual size_t GetSize() const = 0;
    virtual uint64_t GetInitPos() const = 0;
    virtual std::string GetName() const = 0;

    virtual void Write(const void* data, size_t dataSizeInBytes, size_t* bytesWritten = nullptr) = 0;
    virtual uint64_t GetWritePos() const = 0;

    virtual void Read(void* data, size_t dataSizeInBytes, size_t* bytesRead = nullptr) = 0;
    virtual uint64_t GetReadPos() const = 0;
    virtual uint64_t ResetReadPos() = 0;

    virtual void ReadAtBytePos(uint64_t pos, void* data, size_t dataSizeInBytes, size_t* bytesRead = nullptr) = 0;

    virtual std::shared_ptr<uint8_t> ReadShared(size_t dataSizeInBytes, size_t* bytesRead = nullptr) = 0;
    virtual std::shared_ptr<uint8_t> ReadSharedAtBytePos(uint64_t pos, size_t dataSizeInBytes, size_t* bytesRead = nullptr) = 0;

    template <class T>
    std::shared_ptr<T> ReadShared(size_t dataSizeInBytes, size_t* bytesRead = nullptr)
    {
        auto shared = ReadShared(dataSizeInBytes, bytesRead);
        return SpxReinterpretPointerCast<T>(shared);
    }

    template <class T>
    std::shared_ptr<T> ReadSharedAtBytePos(uint64_t pos, size_t dataSizeInBytes, size_t* bytesRead = nullptr)
    {
        auto shared = ReadSharedAtBytePos(pos, dataSizeInBytes, bytesRead);
        return SpxReinterpretPointerCast<T>(shared);
    }

    uint64_t GetBytesReadReady() { return GetWritePos() - GetReadPos(); }

    // TODO: more checks on pos
    uint64_t GetBytesReadReadyAtPos(uint64_t pos) { return GetWritePos() - pos; }

};

class ISpxReadWriteItemBuffer : public ISpxInterfaceBaseFor<ISpxReadWriteItemBuffer>
{
public:

    virtual size_t GetItemSize() const = 0;

    virtual void WriteItems(const void* itemData, size_t itemCount, size_t* itemsWritten = nullptr) = 0;
    virtual uint64_t GetItemWritePos() const = 0;

    virtual void ReadItems(void* itemData, size_t itemCount, size_t* itemsRead = nullptr) = 0;
    virtual uint64_t GetItemReadPos() const = 0;

    virtual void ReadItemsAtPos(uint64_t itemPos, void* itemData, size_t itemCount, size_t* itemsRead = nullptr) = 0;
};

class ISpxAudioPump : public ISpxInterfaceBaseFor<ISpxAudioPump>
{
public:
    virtual uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat) const = 0;
    virtual void SetFormat(const SPXWAVEFORMATEX* pformat, uint16_t cbFormat) = 0;

    virtual void StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor) = 0;
    virtual void PausePump() = 0;
    virtual void StopPump() = 0;

    enum class State { NoInput, Idle, Paused, Processing };
    virtual State GetState() const = 0;

    virtual std::string GetPropertyValue(const std::string& key) const = 0;
};

class ISpxAudioPumpInit : public ISpxInterfaceBaseFor<ISpxAudioPumpInit>
{
public:
    virtual void SetReader(std::shared_ptr<ISpxAudioStreamReader> reader) = 0;
};

#define REASON_CANCELED_NONE static_cast<CancellationReason>(0)
#define NO_MATCH_REASON_NONE static_cast<NoMatchReason>(0)
#define VOICE_PROFILE_TYPE_NONE static_cast<VoiceProfileType>(0)

class ISpxKeywordRecognitionResultInit : public ISpxInterfaceBaseFor<ISpxKeywordRecognitionResultInit>
{
public:
    virtual void InitKeywordResult(const double confidence, const uint64_t offset, const uint64_t duration, const wchar_t* keyword, ResultReason reason, std::shared_ptr<ISpxAudioDataStream> stream) = 0;
};

class ISpxSynthesizerEvents;

class ISpxSynthesisResult : public ISpxInterfaceBaseFor<ISpxSynthesisResult>
{
public:
    virtual std::string GetResultId() = 0;
    virtual std::string GetRequestId() = 0;
    virtual std::shared_ptr<ISpxSynthesizerEvents> GetEvents() = 0;
    virtual std::shared_ptr<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>> GetFutureResult() = 0;
    virtual ResultReason GetReason() = 0;
    virtual CancellationReason GetCancellationReason() = 0;
    virtual const std::shared_ptr<ISpxErrorInformation>& GetError() = 0;
    virtual uint32_t GetAudioLength() = 0;
    virtual std::shared_ptr<std::vector<uint8_t>> GetAudioData() = 0;
    virtual std::shared_ptr<std::vector<uint8_t>> GetRawAudioData() = 0;
    virtual std::shared_ptr<ISpxAudioDataStream> GetAudioDataStream() = 0;
    virtual uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat) = 0;
    virtual bool HasHeader() = 0;
};

class ISpxSynthesisResultInit : public ISpxInterfaceBaseFor<ISpxSynthesisResultInit>
{
public:
    virtual void InitSynthesisResult(const std::string& requestId, ResultReason reason,
        CancellationReason cancellation, const std::shared_ptr<ISpxErrorInformation>& error,
        uint8_t* audio_buffer, size_t audio_length, SPXWAVEFORMATEX* format, bool hasHeader) = 0;
    virtual void SetEvents(const std::shared_ptr<ISpxSynthesizerEvents>& events) = 0;
    virtual void SetFutureResult(std::shared_ptr<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>> futureResult) = 0;
    virtual void Reset() = 0;
};

enum class InteractionIdPurpose { Speech = 0, Activity };

class ISpxInteractionIdProvider: public ISpxInterfaceBaseFor<ISpxInteractionIdProvider>
{
public:
    virtual std::string NextInteractionId(InteractionIdPurpose purpose) noexcept = 0;
    virtual std::string GetInteractionId(InteractionIdPurpose purpose) const noexcept = 0;
};

class ISpxDialogServiceConnector : public ISpxInterfaceBaseFor<ISpxDialogServiceConnector>
{
public:
    virtual CSpxAsyncOp<void> ConnectAsync() = 0;
    virtual CSpxAsyncOp<void> DisconnectAsync() = 0;

    virtual CSpxAsyncOp<std::string> SendActivityAsync(std::string activity) = 0;

    virtual CSpxAsyncOp<void> StartContinuousListeningAsync() = 0;
    virtual CSpxAsyncOp<void> StopContinuousListeningAsync() = 0;

    virtual CSpxAsyncOp<void> StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model) = 0;
    virtual CSpxAsyncOp<void> StopKeywordRecognitionAsync() = 0;

    /* TODO: Change promise type back to void */
    virtual CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> ListenOnceAsync() = 0;
};

class ISpxSynthesizer : public ISpxInterfaceBaseFor<ISpxSynthesizer>
{
public:
    virtual bool IsEnabled() = 0;
    virtual void Enable() = 0;
    virtual void Disable() = 0;

    virtual void SetOutput(std::shared_ptr<ISpxAudioOutput> output) = 0;
    virtual std::shared_ptr<ISpxSynthesisResult> Speak(const std::string& text, bool isSsml) = 0;
    virtual CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>> SpeakAsync(const std::string& text, bool isSsml) = 0;
    virtual std::shared_ptr<ISpxSynthesisResult> StartSpeaking(const std::string& text, bool isSsml) = 0;
    virtual CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>> StartSpeakingAsync(const std::string& text, bool isSsml) = 0;
    virtual void StopSpeaking() = 0;
    virtual CSpxAsyncOp<void> StopSpeakingAsync() = 0;

    virtual void Close() = 0;
};

class ISpxMessageParamFromUser : public ISpxInterfaceBaseFor<ISpxMessageParamFromUser>
{
public:
    virtual void SetParameter(std::string&& path, std::string&& name, std::string&& value) = 0;
    virtual void SendNetworkMessage(std::string&& path, std::string&& payload) = 0;
    virtual void SendNetworkMessage(std::string&& path, std::vector<uint8_t>&& payload) = 0;
};

using CSpxStringMap = std::unordered_map<std::string, std::string>;
class ISpxGetUspMessageParamsFromUser : public ISpxInterfaceBaseFor<ISpxGetUspMessageParamsFromUser>
{
public:
    virtual CSpxStringMap GetParametersFromUser(std::string&& path) = 0;
};

class ISpxConnection : public ISpxInterfaceBaseFor<ISpxConnection>
{
public:
    virtual void Open(bool forContinuousRecognition) = 0;
    virtual void Close() = 0;
    virtual std::shared_ptr<ISpxRecognizer> GetRecognizer() = 0;
};

class ISpxConnectionInit : public ISpxInterfaceBaseFor<ISpxConnectionInit>
{
public:
    virtual void Init(std::weak_ptr<ISpxRecognizer> recognizer, std::weak_ptr<ISpxMessageParamFromUser> setter) = 0;
};

class ISpxConnectionFromRecognizer : public ISpxInterfaceBaseFor<ISpxConnectionFromRecognizer>
{
public:
    virtual std::shared_ptr<ISpxConnection> GetConnection() = 0;
};


class ISpxActivityEventArgsInit : public ISpxInterfaceBaseFor<ISpxActivityEventArgsInit>
{
public:

    virtual void Init(std::string activity) = 0;
    virtual void Init(std::string activity, std::shared_ptr<ISpxAudioOutput> audio) = 0;
};


class ISpxConnectionMessage : public ISpxInterfaceBaseFor<ISpxConnectionMessage>
{
public:

    virtual const std::string& GetHeaders() const = 0;
    virtual const std::string& GetPath() const = 0;

    virtual const uint8_t* GetBuffer() const = 0;
    virtual uint32_t GetBufferSize() const = 0;
    virtual bool IsBufferBinary() const = 0;
};

class ISpxConnectionMessageInit : public ISpxInterfaceBaseFor<ISpxConnectionMessageInit>
{
public:
    virtual void Init(const std::string& headers, const std::string& path, const uint8_t* buffer, uint32_t bufferSize, bool bufferIsBinary) = 0;
};

class ISpxConnectionMessageEventArgs : public ISpxInterfaceBaseFor<ISpxConnectionMessageEventArgs>
{
public:
    virtual std::shared_ptr<ISpxConnectionMessage> GetMessage() const = 0;
};

class ISpxConnectionMessageEventArgsInit : public ISpxInterfaceBaseFor<ISpxConnectionMessageEventArgsInit>
{
public:

    virtual void Init(std::shared_ptr<ISpxConnectionMessage> message) = 0;
};



class ISpxActivityEventArgs :
    public ISpxInterfaceBaseFor<ISpxActivityEventArgs>
{
public:

    virtual const std::string& GetActivity() const = 0;
    virtual bool HasAudio() const = 0;
    virtual std::shared_ptr<ISpxAudioOutput> GetAudio() const = 0;
};


class ISpxRecognizerEvents : public ISpxInterfaceBaseFor<ISpxRecognizerEvents>
{
public:
    using RecoEvent_Type = EventSignal<std::shared_ptr<ISpxRecognitionEventArgs>>;
    using SessionEvent_Type = EventSignal<std::shared_ptr<ISpxSessionEventArgs>>;
    using ConnectionEvent_Type = EventSignal<std::shared_ptr<ISpxConnectionEventArgs>>;
    using ConnectionMessageEvent_Type = EventSignal<std::shared_ptr<ISpxConnectionMessageEventArgs>>;

    virtual void FireSessionStarted(const std::wstring& sessionId) = 0;
    virtual void FireSessionStopped(const std::wstring& sessionId) = 0;

    virtual void FireConnected(const std::wstring& sessionId) = 0;
    virtual void FireDisconnected(const std::wstring& sessionId) = 0;

    virtual void FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset) = 0;
    virtual void FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset) = 0;
    virtual void FireConnectionMessageReceived(const std::string& headers, const std::string& path, const uint8_t* buffer, uint32_t bufferSize, bool isBufferBinary) = 0;

    virtual void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) = 0;

    SessionEvent_Type SessionStarted;
    SessionEvent_Type SessionStopped;

    ConnectionEvent_Type Connected;
    ConnectionEvent_Type Disconnected;
    ConnectionMessageEvent_Type ConnectionMessageReceived;

    RecoEvent_Type SpeechStartDetected;
    RecoEvent_Type SpeechEndDetected;

    RecoEvent_Type IntermediateResult;
    RecoEvent_Type FinalResult;
    RecoEvent_Type Canceled;
    RecoEvent_Type TranslationSynthesisResult;

protected:
    ISpxRecognizerEvents(RecoEvent_Type::NotifyCallback_Type connectedCallback, RecoEvent_Type::NotifyCallback_Type disconnectedCallback) :
        SessionStarted(nullptr, nullptr, true),
        SessionStopped(nullptr, nullptr, true),
        Connected(nullptr, nullptr, true),
        Disconnected(nullptr, nullptr, true),
        SpeechStartDetected(connectedCallback, disconnectedCallback, true),
        SpeechEndDetected(connectedCallback, disconnectedCallback, true),
        IntermediateResult(connectedCallback, disconnectedCallback, true),
        FinalResult(connectedCallback, disconnectedCallback, true),
        Canceled(connectedCallback, disconnectedCallback, true),
        TranslationSynthesisResult(connectedCallback, disconnectedCallback, true)
    {
    }

private:
    ISpxRecognizerEvents() = delete;
};

class ISpxDialogServiceConnectorEvents : public ISpxInterfaceBaseFor<ISpxDialogServiceConnectorEvents>
{
public:
    using ActivityReceivedEvent_Type = EventSignal<std::shared_ptr<ISpxActivityEventArgs>>;

    ActivityReceivedEvent_Type ActivityReceived;

    virtual void FireActivityReceived(const std::wstring& sessionId, std::string activity, std::shared_ptr<ISpxAudioOutput> audio) = 0;

protected:

    ISpxDialogServiceConnectorEvents(ActivityReceivedEvent_Type::NotifyCallback_Type connectedCallback, ActivityReceivedEvent_Type::NotifyCallback_Type disconnectedCallback) :
        ActivityReceived(connectedCallback, disconnectedCallback, true)
    {
    }

private:
    ISpxDialogServiceConnectorEvents() = delete;

};

class ISpxSynthesisEventArgs : public ISpxInterfaceBaseFor<ISpxSynthesisEventArgs>
{
public:
    virtual std::shared_ptr<ISpxSynthesisResult> GetResult() = 0;
};

class ISpxSynthesisEventArgsInit : public ISpxInterfaceBaseFor<ISpxSynthesisEventArgsInit>
{
public:
    virtual void Init(std::shared_ptr<ISpxSynthesisResult> result) = 0;
};

class ISpxWordBoundaryEventArgs : public ISpxInterfaceBaseFor<ISpxWordBoundaryEventArgs>
{
public:
    virtual uint64_t GetAudioOffset() = 0;
    virtual uint32_t GetTextOffset() = 0;
    virtual uint32_t GetWordLength() = 0;
};

class ISpxWordBoundaryEventArgsInit : public ISpxInterfaceBaseFor<ISpxWordBoundaryEventArgsInit>
{
public:
    virtual void Init(uint64_t audioOffset, uint32_t textOffset, uint32_t wordLength) = 0;
};

class ISpxSynthesizerEvents : public ISpxInterfaceBaseFor<ISpxSynthesizerEvents>
{
public:
    using SynthEvent_Type = EventSignal<std::shared_ptr<ISpxSynthesisEventArgs>>;
    using WordBoundaryEvent_Type = EventSignal<std::shared_ptr<ISpxWordBoundaryEventArgs>>;
    using SynthesisCallbackFunction_Type = std::function<void(std::shared_ptr<ISpxSynthesisEventArgs>)>;

    virtual void ConnectSynthesisStartedCallback(void* object, SynthesisCallbackFunction_Type callback) = 0;
    virtual void ConnectSynthesizingCallback(void* object, SynthesisCallbackFunction_Type callback) = 0;
    virtual void ConnectSynthesisCompletedCallback(void* object, SynthesisCallbackFunction_Type callback) = 0;
    virtual void ConnectSynthesisCanceledCallback(void* object, SynthesisCallbackFunction_Type callback) = 0;
    virtual void DisconnectSynthesisStartedCallback(void* object, SynthesisCallbackFunction_Type callback) = 0;
    virtual void DisconnectSynthesizingCallback(void* object, SynthesisCallbackFunction_Type callback) = 0;
    virtual void DisconnectSynthesisCompletedCallback(void* object, SynthesisCallbackFunction_Type callback) = 0;
    virtual void DisconnectSynthesisCanceledCallback(void* object, SynthesisCallbackFunction_Type callback) = 0;
    virtual void FireSynthesisStarted(std::shared_ptr<ISpxSynthesisResult> result) = 0;
    virtual void FireSynthesizing(std::shared_ptr<ISpxSynthesisResult> result) = 0;
    virtual void FireSynthesisCompleted(std::shared_ptr<ISpxSynthesisResult> result) = 0;
    virtual void FireSynthesisCanceled(std::shared_ptr<ISpxSynthesisResult> result) = 0;
    virtual void FireWordBoundary(uint64_t audioOffset, uint32_t textOffset, uint32_t wordLength) = 0;

    std::list<std::pair<void*, std::shared_ptr<SynthEvent_Type>>> SynthesisStarted;
    std::list<std::pair<void*, std::shared_ptr<SynthEvent_Type>>> Synthesizing;
    std::list<std::pair<void*, std::shared_ptr<SynthEvent_Type>>> SynthesisCompleted;
    std::list<std::pair<void*, std::shared_ptr<SynthEvent_Type>>> SynthesisCanceled;
    WordBoundaryEvent_Type WordBoundary;
};

class ISpxConversationWithImpl : public ISpxInterfaceBaseFor<ISpxConversationWithImpl>
{
public:
    virtual std::shared_ptr<ISpxConversation> GetConversationImpl() = 0;
};


class ISpxSession : public ISpxInterfaceBaseFor<ISpxSession>
{
public:
    virtual const std::wstring& GetSessionId() const = 0;
    virtual bool IsStreaming() = 0;

    virtual void AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer) = 0;
    virtual void RemoveRecognizer(ISpxRecognizer* recognizer) = 0;

    virtual CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync() = 0;
    virtual CSpxAsyncOp<void> StartContinuousRecognitionAsync() = 0;
    virtual CSpxAsyncOp<void> StopContinuousRecognitionAsync() = 0;

    virtual CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync(std::shared_ptr<ISpxKwsModel> model) = 0;
    virtual CSpxAsyncOp<void> StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model) = 0;
    virtual CSpxAsyncOp<void> StopKeywordRecognitionAsync() = 0;

    virtual void OpenConnection(bool forContinuousRecognition) = 0;
    virtual void CloseConnection() = 0;

    virtual CSpxAsyncOp<std::string> SendActivityAsync(std::string activity) = 0;

    virtual void WriteTelemetryLatency(uint64_t latencyInTicks, bool isPhraseLatency) = 0;
    virtual void SendSpeechEventMessage(std::string&& payload) = 0;
    virtual void SendNetworkMessage(std::string&& path, std::string&& payload, bool alwaysSend = true) = 0;
    virtual void SendNetworkMessage(std::string&& path, std::vector<uint8_t>&& payload, bool alwaysSend = true) = 0;

    virtual void SetConversation(std::shared_ptr<ISpxConversation> conversation) = 0;
};

class ISpxAudioStreamSessionInit : public ISpxInterfaceBaseFor<ISpxAudioStreamSessionInit>
{
public:
    virtual void InitFromFile(const wchar_t* pszFileName) = 0;
    virtual void InitFromMicrophone() = 0;
    virtual void InitFromStream(std::shared_ptr<ISpxAudioStream> stream) = 0;
};

class ISpxAudioSourceControl : public ISpxInterfaceBaseFor<ISpxAudioSourceControl>
{
public:

    virtual void StartAudio(std::shared_ptr<ISpxAudioSourceNotifyMe> target) = 0;
    virtual void StopAudio() = 0;
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
    virtual void OpenConnection(bool) {};
    virtual void CloseConnection() {};
    virtual void SendAgentMessage(const std::string &) {};

    virtual void WriteTelemetryLatency(uint64_t, bool) {};
    virtual void SendSpeechEventMessage(std::string&&) {};
    virtual std::future<bool> SendNetworkMessage(std::string&&, std::string&&) {
        std::promise<bool> p; p.set_value(false); return p.get_future();
    }
    virtual std::future<bool> SendNetworkMessage(std::string&&, std::vector<uint8_t>&&) {
        std::promise<bool> p; p.set_value(false); return p.get_future();
    }
};

class ISpxRecoEngineAdapterSite : public ISpxInterfaceBaseFor<ISpxRecoEngineAdapterSite>
{
public:
    using ResultPayload_Type = std::shared_ptr<ISpxRecognitionResult>;
    using AdditionalMessagePayload_Type = void*;
    using ErrorPayload_Type = std::shared_ptr<ISpxErrorInformation>;

    virtual void GetScenarioCount(uint16_t* countSpeech, uint16_t* countIntent, uint16_t* countTranslation, uint16_t* countDialog, uint16_t* countTranscriber ) = 0;

    virtual std::list<std::string> GetListenForList() = 0;
    virtual void GetIntentInfo(std::string& provider, std::string& id, std::string& key, std::string& region) = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> GetSpottedKeywordResult() = 0;

    virtual void AdapterStartingTurn(ISpxRecoEngineAdapter* adapter) = 0;
    virtual void AdapterStartedTurn(ISpxRecoEngineAdapter* adapter, const std::string& id) = 0;
    virtual void AdapterStoppedTurn(ISpxRecoEngineAdapter* adapter) = 0;

    virtual void AdapterDetectedSpeechStart(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;
    virtual void AdapterDetectedSpeechEnd(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;

    virtual void AdapterDetectedSoundStart(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;
    virtual void AdapterDetectedSoundEnd(ISpxRecoEngineAdapter* adapter, uint64_t offset) = 0;

    virtual void FireAdapterResult_Intermediate(ISpxRecoEngineAdapter* adapter, uint64_t offset, ResultPayload_Type payload) = 0;
    virtual void FireAdapterResult_KeywordResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, ResultPayload_Type payload, bool isAccepted) = 0;
    virtual void FireAdapterResult_FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, ResultPayload_Type payload) = 0;
    virtual void FireAdapterResult_TranslationSynthesis(ISpxRecoEngineAdapter* adapter, ResultPayload_Type payload) = 0;
    virtual void FireAdapterResult_ActivityReceived(ISpxRecoEngineAdapter* adapter, std::string activity, std::shared_ptr<ISpxAudioOutput> audio) = 0;
    virtual void AdapterEndOfDictation(ISpxRecoEngineAdapter* adapter, uint64_t offset, uint64_t duration) = 0;
    virtual void FireConnectedEvent() = 0;
    virtual void FireDisconnectedEvent() = 0;
    virtual void FireConnectionMessageReceived(const std::string& headers, const std::string& path, const uint8_t* buffer, uint32_t bufferSize, bool isBufferBinary) = 0;

    virtual void AdapterRequestingAudioMute(ISpxRecoEngineAdapter* adapter, bool mute) = 0;
    virtual void AdapterCompletedSetFormatStop(ISpxRecoEngineAdapter* adapter) = 0;

    virtual void AdditionalMessage(ISpxRecoEngineAdapter* adapter, uint64_t offset, AdditionalMessagePayload_Type payload) = 0;

    virtual void Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload) = 0;
};

class ISpxTtsEngineAdapterSite;

class ISpxTtsEngineAdapter : public ISpxInterfaceBaseFor<ISpxTtsEngineAdapter>
{
public:
    virtual void SetOutput(std::shared_ptr<ISpxAudioOutput> output) = 0;
    virtual std::shared_ptr<ISpxSynthesisResult> Speak(const std::string& text, bool isSsml, const std::string& requestId, bool retry) = 0;
    virtual void StopSpeaking() = 0;
};

class ISpxTtsEngineAdapterSite : public ISpxInterfaceBaseFor<ISpxTtsEngineAdapterSite>
{
public:
    virtual uint32_t Write(ISpxTtsEngineAdapter* adapter, const std::string& requestId, uint8_t* buffer, uint32_t size, std::shared_ptr<std::unordered_map<std::string, std::string>> properties) = 0;
    virtual std::shared_ptr<ISpxSynthesizerEvents> GetEventsSite() = 0;
    virtual std::shared_ptr<ISpxSynthesisResult> CreateEmptySynthesisResult() = 0;
};

class ISpxAudioPumpSite : public ISpxInterfaceBaseFor<ISpxAudioPumpSite>
{
public:
    virtual void Error(const std::string& msg) = 0;
};

class ISpxSpeechAudioProcessorAdapter :
    public ISpxAudioProcessor,
    public ISpxInterfaceBaseFor<ISpxSpeechAudioProcessorAdapter>
{
public:
    virtual void SetSpeechDetectionThreshold(uint32_t threshold) = 0;
    virtual void SetSpeechDetectionSilenceMs(uint32_t duration) = 0;
    virtual void SetSpeechDetectionSkipMs(uint32_t duration) = 0;
    virtual void SetSpeechDetectionBaselineMs(uint32_t duration) = 0;
};

class ISpxSpeechAudioProcessorAdapterSite : public ISpxInterfaceBaseFor<ISpxSpeechAudioProcessorAdapterSite>
{
public:
    virtual void SpeechStartDetected(uint64_t offset) = 0;
    virtual void SpeechEndDetected(uint64_t offset) = 0;
};

class ISpxKwsEngineAdapter :
    public ISpxAudioProcessor,
    public ISpxInterfaceBaseFor<ISpxKwsEngineAdapter>
{
};

class ISpxKwsEngineAdapterSite : public ISpxInterfaceBaseFor<ISpxKwsEngineAdapterSite>
{
public:
    virtual void KeywordDetected(ISpxKwsEngineAdapter* adapter, uint64_t offset, uint64_t duration, double confidence, const std::string& keyword, const DataChunkPtr& audioChunk) = 0;
    virtual void AdapterCompletedSetFormatStop(ISpxKwsEngineAdapter* adapter) = 0;
};

class ISpxRecoResultFactory : public ISpxInterfaceBaseFor<ISpxRecoResultFactory>
{
public:
    virtual std::shared_ptr<ISpxRecognitionResult> CreateIntermediateResult(const wchar_t* text, uint64_t offset, uint64_t duration) = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateFinalResult(
        ResultReason reason,
        NoMatchReason noMatchReason,
        const wchar_t* text,
        uint64_t offset,
        uint64_t duration,
        const wchar_t* userId = nullptr) = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateKeywordResult(const double confidence, const uint64_t offset, const uint64_t duration, const wchar_t* keyword, ResultReason reason, std::shared_ptr<ISpxAudioDataStream> stream) = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateErrorResult(const std::shared_ptr<ISpxErrorInformation>& error) = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateEndOfStreamResult() = 0;
};

class ISpxKeywordRecognitionResult : public ISpxInterfaceBaseFor<ISpxKeywordRecognitionResult>
{
public:
    virtual double GetConfidence() = 0;
};

class ISpxEventArgsFactory : public ISpxInterfaceBaseFor<ISpxEventArgsFactory>
{
public:
    virtual std::shared_ptr<ISpxSessionEventArgs> CreateSessionEventArgs(const std::wstring& sessionId) = 0;
    virtual std::shared_ptr<ISpxConnectionEventArgs> CreateConnectionEventArgs(const std::wstring& sessionId) = 0;
    virtual std::shared_ptr<ISpxConnectionMessageEventArgs> CreateConnectionMessageEventArgs(const std::string& headers, const std::string& path, const uint8_t* buffer, uint32_t bufferSize, bool isBufferBinary) = 0;
    virtual std::shared_ptr<ISpxRecognitionEventArgs> CreateRecognitionEventArgs(const std::wstring& sessionId, uint64_t offset) = 0;
    virtual std::shared_ptr<ISpxRecognitionEventArgs> CreateRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) = 0;
    virtual std::shared_ptr<ISpxActivityEventArgs> CreateActivityEventArgs(std::string activity, std::shared_ptr<ISpxAudioOutput> audio) = 0;
};

class ISpxRecognizerSite : public ISpxInterfaceBaseFor<ISpxRecognizerSite>
{
public:
    virtual std::shared_ptr<ISpxSession> GetDefaultSession() = 0;
};

class ISpxVoiceProfile : public ISpxInterfaceBaseFor<ISpxVoiceProfile>
{
public:
    virtual void SetProfileId(std::string&& id) = 0;
    virtual std::string GetProfileId() const = 0;

    virtual VoiceProfileType GetType() const = 0;
};

class ISpxSIModel : public ISpxInterfaceBaseFor<ISpxSIModel>
{
public:
    virtual void AddProfile(const std::shared_ptr<ISpxVoiceProfile>& profile) = 0;
    virtual std::vector<std::shared_ptr<ISpxVoiceProfile>> GetProfiles() const = 0;
};

class ISpxSVModel : public ISpxInterfaceBaseFor<ISpxSVModel>
{
public:
    virtual void InitModel(const std::shared_ptr<ISpxVoiceProfile>& profile) = 0;
    virtual std::shared_ptr<ISpxVoiceProfile> GetProfile() const = 0;
};

//todo:
class ISpxVoiceProfileResult
{
public:

};

class ISpxVoiceProfileClient :public ISpxInterfaceBaseFor<ISpxVoiceProfileClient>
{
public:
    enum class Action {Verify, Delete, Reset, Enroll};
    virtual std::shared_ptr<ISpxVoiceProfile> Create(VoiceProfileType voiceProfileType, std::string&& locale) = 0;
    virtual RecognitionResultPtr Identify(std::vector<std::shared_ptr<ISpxVoiceProfile>>&& profileIds) = 0;
    virtual RecognitionResultPtr ProcessProfileAction(Action action, VoiceProfileType type, std::string&& profileId) = 0;
};

class ISpxHttpAudioStreamSession :public ISpxInterfaceBaseFor<ISpxHttpAudioStreamSession>
{
public:
    virtual std::string CreateVoiceProfile(VoiceProfileType type, std::string&& locale) = 0;
    virtual RecognitionResultPtr StartStreamingAudioAndWaitForResult(bool enroll, VoiceProfileType type, std::vector<std::string>&& ids) = 0;
    virtual RecognitionResultPtr ModifyVoiceProfile(bool reset, VoiceProfileType type, std::string&& id) = 0;
};

class ISpxSpeechApiFactory : public ISpxInterfaceBaseFor<ISpxSpeechApiFactory>
{
public:
    virtual std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerFromConfig(std::shared_ptr<ISpxAudioConfig> audioInput) = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerFromConfig(std::shared_ptr<ISpxAudioConfig> audioInput) = 0;
    virtual std::shared_ptr<ISpxDialogServiceConnector> CreateDialogServiceConnectorFromConfig(std::shared_ptr<ISpxAudioConfig> audioInput) = 0;
    virtual std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerFromConfig(std::shared_ptr<ISpxAudioConfig> audioInput) = 0;
    virtual std::shared_ptr<ISpxConversation> CreateConversationFromConfig(const char* id) = 0;
    virtual void InitSessionFromAudioInputConfig(std::shared_ptr<ISpxAudioStreamSessionInit> session, std::shared_ptr<ISpxAudioConfig> audioInput) = 0;
    virtual std::shared_ptr<ISpxVoiceProfileClient> CreateVoiceProfileClientFromConfig() = 0;
    virtual std::shared_ptr<ISpxVoiceProfileClient> CreateSpeakerRecognizerFromConfig(std::shared_ptr<ISpxAudioConfig> audioInput) = 0;
};

class ISpxSpeechSynthesisApiFactory : public ISpxInterfaceBaseFor<ISpxSpeechSynthesisApiFactory>
{
public:
    virtual std::shared_ptr<ISpxSynthesizer> CreateSpeechSynthesizerFromConfig(std::shared_ptr<ISpxAudioConfig> audioConfig) = 0;
};

// class ISpxNamedValues : public ISpxInterfaceBaseFor<ISpxNamedValues>
// {
// public:
//     virtual uint32_t ValueCount() const = 0;
//     virtual std::string ValueName(int index) const = 0;

//     virtual bool HasStringValue(const char* name) const = 0;
//     virtual std::string GetStringValue(const char* name, const char* defaultValue = "") const = 0;

//     virtual bool HasBinaryValue(const char* name) const = 0;
//     virtual std::vector<uint8_t> GetBinaryValue(const char* name, const uint8_t* defaultValue = null, uint32_t defaultValueSize = 0) const = 0;
// };

// class ISpxNamedValuesWriter : public ISpxInterfaceBaseFor<ISpxNamedValuesWriter>
// {
// public:

//     virtual void AddValue(const char* name, const char* value) = 0;
//     virtual void AddValue(const char* name, const uint8_t* value, uint32_t valueSize) = 0;

//     virtual void RemoveValue(const char* name) = 0;

//     virtual void ReplaceValue(const char* name, const char* value) = 0;
//     virtual void ReplaceValue(const char* name, const uint8_t* value, uint32_t valueSize) = 0;
// };

class ISpxNamedProperties : public ISpxInterfaceBaseFor<ISpxNamedProperties>
{
public:
    virtual std::string GetStringValue(const char* name, const char* defaultValue = "") const = 0;
    virtual void SetStringValue(const char* name, const char* value) = 0;
    virtual bool HasStringValue(const char* name) const = 0;
    virtual void Copy(ISpxNamedProperties* from) = 0;
    virtual CSpxStringMap FindPrefix(const char* prefix) const {UNUSED(prefix); return CSpxStringMap{};}
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



class ISpxTranslationSynthesisResult : public ISpxInterfaceBaseFor<ISpxTranslationSynthesisResult>
{
public:
    virtual const uint8_t* GetAudio() const = 0;
    virtual size_t GetLength() const = 0;
    virtual std::string GetRequestId() const = 0;
};

class ISpxTranslationSynthesisResultInit : public ISpxInterfaceBaseFor<ISpxTranslationSynthesisResultInit>
{
public:
    virtual void InitTranslationSynthesisResult(const uint8_t* audioData, size_t audioLength, const std::string& requestId) = 0;
};

class ISpxPhrase : public ISpxInterfaceBaseFor<ISpxPhrase>
{
public:

    virtual void InitPhrase(const wchar_t* phrase) = 0;
    virtual std::wstring GetPhrase() const = 0;
};

class ISpxPhraseList : public ISpxInterfaceBaseFor<ISpxPhraseList>
{
public:

    virtual void InitPhraseList(const wchar_t* name) = 0;
    virtual std::wstring GetName() = 0;

    virtual void AddPhrase(std::shared_ptr<ISpxPhrase> phrase) = 0;
    virtual void Clear() = 0;
};

class ISpxGrammar : public ISpxInterfaceBaseFor<ISpxGrammar>
{
public:

    virtual std::list<std::string> GetListenForList() = 0;
};

// Represents a grammar that is persisted in (cloud) storage and has a storage ID available.
class ISpxStoredGrammar : public ISpxInterfaceBaseFor<ISpxStoredGrammar>
{
public:
    virtual void InitStoredGrammar(const wchar_t* id) = 0;
};

class ISpxClassLanguageModel : public ISpxInterfaceBaseFor<ISpxClassLanguageModel>
{
public:

    virtual void InitClassLanguageModel(const wchar_t* id) = 0;
    virtual void AssignClass(const wchar_t* className, std::shared_ptr<ISpxGrammar> grammar) = 0;
};

class ISpxGrammarList : public ISpxInterfaceBaseFor<ISpxGrammarList>
{
public:

    virtual std::shared_ptr<ISpxGrammar> GetPhraseListGrammar(const wchar_t* name) = 0;
    virtual void AddGrammar(std::shared_ptr<ISpxGrammar> grammar) = 0;
    virtual void SetRecognitionFactor(double factor) = 0;
};

class ISpxLanguageUnderstandingModel : public ISpxInterfaceBaseFor<ISpxLanguageUnderstandingModel>
{
public:
    virtual void InitAppId(const wchar_t* appId) = 0;
    virtual void InitEndpoint(const wchar_t* uri) = 0;
    virtual void InitSubscription(const wchar_t* subscriptionKey, const wchar_t* appId, const wchar_t* region) = 0;
    virtual void UpdateSubscription(const wchar_t* subscriptionKey, const wchar_t* region) = 0;

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

class ISpxConversationTranscriber : public ISpxInterfaceBaseFor<ISpxConversationTranscriber>
{
public:
    virtual void JoinConversation(std::weak_ptr<ISpxConversation> conversation) = 0;
    virtual void LeaveConversation() = 0;
};

class ISpxObjectWithAudioConfig : public ISpxInterfaceBaseFor<ISpxObjectWithAudioConfig>
{
public:
    virtual void SetAudioConfig(std::weak_ptr<ISpxAudioConfig> audioConfig) = 0;
    virtual std::shared_ptr<ISpxAudioConfig> GetAudioConfig() = 0;
};

class ISpxSpeechEventPayloadProvider : public ISpxInterfaceBaseFor<ISpxSpeechEventPayloadProvider>
{
public:
    virtual std::string GetSpeechEventPayload(bool startMeeting) = 0;
};

class ISpxTranslationRecognizer : public ISpxInterfaceBaseFor<ISpxTranslationRecognizer>
{
public:
    virtual void AddTargetLanguage(const std::string& lang) = 0;
    virtual void RemoveTargetLanguage(const std::string& lang) = 0;
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

class ISpxSpeechConfig : public ISpxInterfaceBaseFor<ISpxSpeechConfig>
{
public:
    virtual void InitFromSubscription(const char * subscription, const char* region) = 0;
    virtual void InitFromEndpoint(const char * endpoint, const char* subscription) = 0;
    virtual void InitFromHost(const char * host, const char* subscription) = 0;
    virtual void InitAuthorizationToken(const char * authToken, const char * region) = 0;
    virtual void SetServiceProperty(const std::string& name, const std::string& value, ServicePropertyChannel channel) = 0;
    virtual void SetProfanity(ProfanityOption profanity) = 0;
};

class ISpxHttpRecoEngineAdapter : public ISpxInterfaceBaseFor<ISpxHttpRecoEngineAdapter>
{
public:
    virtual std::string CreateVoiceProfile(VoiceProfileType type, std::string&& locale) const = 0;
    virtual RecognitionResultPtr ModifyVoiceProfile(bool reset, VoiceProfileType type, std::string&& id) = 0;
    virtual void SetFormat(const SPXWAVEFORMATEX* pformat, VoiceProfileType type, std::vector<std::string>&& profileIds, bool enroll) = 0;
    virtual void ProcessAudio(const DataChunkPtr& audioChunk) = 0;
    virtual void FlushAudio() = 0;
    virtual RecognitionResultPtr GetResult() = 0;
};

class ISpxHttpRecoEngineAdapterSite : public ISpxInterfaceBaseFor<ISpxHttpRecoEngineAdapterSite>
{
};

class ISpxSpeechTranslationConfig : public ISpxInterfaceBaseFor<ISpxSpeechTranslationConfig>
{
public:
    virtual void AddTargetLanguage(const std::string& lang) = 0;
    virtual void RemoveTargetLanguage(const std::string& lang) = 0;
};

class ISpxSourceLanguageConfig : public ISpxInterfaceBaseFor<ISpxSourceLanguageConfig>
{
public:
    virtual void InitFromLanguage(const char* language) = 0;
    virtual void InitFromLanguageAndEndpointId(const char* language, const char* endpointId) = 0;
    virtual std::string GetLanguage() = 0;
    virtual std::string GetEndpointId() = 0;
};

class ISpxAutoDetectSourceLangConfig : public ISpxInterfaceBaseFor<ISpxAutoDetectSourceLangConfig>
{
public:
    virtual void InitFromOpenRange() = 0;
    virtual void InitFromLanguages(const char* languages) = 0;
    virtual void AddSourceLanguageConfig(std::shared_ptr<ISpxSourceLanguageConfig> sourceLanguageConfig) = 0;
};

class ISpxPronunciationAssessmentConfig : public ISpxInterfaceBaseFor<ISpxPronunciationAssessmentConfig>
{
public:
    virtual void InitWithParameters(const char* referenceText, PronunciationAssessmentGradingSystem gradingSystem,
                      PronunciationAssessmentGranularity granularity, bool enableMiscue) = 0;
    virtual void InitFromJson(const char* json) = 0;
    virtual void UpdateJson() = 0;
};

class ISpxThreadService : public ISpxInterfaceBaseFor<ISpxThreadService>
{
public:
    using TaskId = int;

    enum class Affinity
    {
        User = 0,
        Background = 1
    };

    // Asynchronously execute a task on a thread. All tasks scheduled with the same affinity using this function
    // are executed in FIFO order.
    //
    // Optional 'executed' promise can be used to be notified about task execution:
    //    true: if the task has been successfully executed
    //    false: if the task has been cancelled
    //    exception: if there was an exception during scheduling
    virtual TaskId ExecuteAsync(std::packaged_task<void()>&& task,
        Affinity affinity = Affinity::Background,
        std::promise<bool>&& executed = std::promise<bool>()) = 0;

    // Asynchronously execute a task on a thread with a delay 'count' number of times.
    //
    // Optional 'executed' promise can be used to be notified about task execution:
    //    true: if the task has been successfully executed
    //    false: if the task has been cancelled
    //    exception: if there was an exception during scheduling
    virtual TaskId ExecuteAsync(std::packaged_task<void()>&& task,
        std::chrono::milliseconds delay,
        Affinity affinity = Affinity::Background,
        std::promise<bool>&& executed = std::promise<bool>()) = 0;

    // Execute a task on a thread synchronously blocking the caller.
    virtual void ExecuteSync(std::packaged_task<void()>&& task,
        Affinity affinity = Affinity::Background) = 0;

    // Cancels the task. If the task is canceled,
    // the corresponding 'canceled' promise is fulfilled.
    virtual bool Cancel(TaskId id) = 0;

    // Cancels all tasks.
    virtual void CancelAllTasks() = 0;
};




class ISpxRetrievable: public ISpxInterfaceBaseFor<ISpxRetrievable>
{
public:
    virtual void MarkAsRetrieved() noexcept = 0;
    virtual bool WasRetrieved() const noexcept = 0;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
