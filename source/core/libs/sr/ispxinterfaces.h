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


namespace CARBON_IMPL_NAMESPACE() {


class ISpxInterfaceBase : public std::enable_shared_from_this<ISpxInterfaceBase>
{
protected:

    typedef std::enable_shared_from_this<ISpxInterfaceBase> base_type;

    template<typename T>
    friend struct ISpxInterfaceBaseFor;

    std::shared_ptr<ISpxInterfaceBase> shared_from_this()
    {
        return base_type::shared_from_this();
    }
};


template<typename T>
struct ISpxInterfaceBaseFor : virtual public ISpxInterfaceBase
{
public:

    std::shared_ptr<T> shared_from_this()
    {
        std::shared_ptr<T> result(base_type::shared_from_this(), static_cast<T*>(this));
        return result;
    }


private:

    typedef ISpxInterfaceBase base_type;
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


enum class Reason { Recognized, NoMatch, Canceled, OtherRecognizer };


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


class ISpxSession : public ISpxInterfaceBaseFor<ISpxSession>
{
public:

    virtual std::wstring GetSessionId() const = 0;

    virtual void AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer) = 0;
    virtual void RemoveRecognizer(ISpxRecognizer* precognzier) = 0;

    virtual CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync() = 0;
    virtual CSpxAsyncOp<void> StartContinuousRecognitionAsync() = 0;
    virtual CSpxAsyncOp<void> StopContinuousRecognitionAsync() = 0;
};


class ISpxAudioStreamSessionInit : public ISpxInterfaceBaseFor<ISpxAudioStreamSessionInit>
{
public:

    virtual void InitFromFile(const wchar_t* pszFileName) = 0;
};



}; // CARBON_IMPL_NAMESPACE()
