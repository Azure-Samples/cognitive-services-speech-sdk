#pragma once

class IUnidecSearchGraphCombo;

// Deprecated, will be deleted soon!
// replaced by CreateUnidecSearchGraphCombo
extern "C" __declspec(dllexport)
IUnidecSearchGraphCombo* LoadUnidecSearchGraphCombo(
        const wchar_t* hclgPath,
        const wchar_t* lmsPath);

extern "C" __declspec(dllexport)
IUnidecSearchGraphCombo* CreateUnidecSearchGraphCombo(
        const wchar_t* spec);

extern "C" __declspec(dllexport)
IUnidecSearchGraphCombo* CreateUnidecSearchGraphComboEx(
        const wchar_t* spec,
        const IUnidecSearchGraphCombo* base);

extern "C" __declspec(dllexport)
void DeleteUnidecSearchGraphCombo(
        IUnidecSearchGraphCombo* graphs);


class IUnidecIntermediateResult;

extern "C" __declspec(dllexport)
size_t GetUnidecIntermediateResultWordCount(
        const IUnidecSearchGraphCombo* graphs,
        const IUnidecIntermediateResult* intermediateresult);

extern "C" __declspec(dllexport)
void GetUnidecIntermediateResultWords(
        const IUnidecSearchGraphCombo* graphs,
        const IUnidecIntermediateResult* intermediateResult,
        // TODO: use wchar_t
        const wchar_t** words,
        size_t wordsLen);


class IUnidecNBestList;

extern "C" __declspec(dllexport)
size_t GetUnidecNBestListSize(const IUnidecNBestList* nbest);

extern "C" __declspec(dllexport)
float GetUnidecNBestListCost(
        const IUnidecNBestList* nbest,
        size_t i);

extern "C" __declspec(dllexport)
float GetUnidecNBestListAMCost(
        const IUnidecNBestList* nbest,
        size_t i);

extern "C" __declspec(dllexport)
float GetUnidecNBestListLMCost(
        const IUnidecNBestList* nbest,
        size_t i);

extern "C" __declspec(dllexport)
float GetUnidecNBestListPrunedLMCost(
        const IUnidecNBestList* nbest,
        size_t i);

extern "C" __declspec(dllexport)
size_t GetUnidecNBestListWordCount(
        const IUnidecSearchGraphCombo* graphs,
        const IUnidecNBestList* nbest,
        size_t i);

extern "C" __declspec(dllexport)
void GetUnidecNBestListWords(
        const IUnidecSearchGraphCombo* graphs,
        const IUnidecNBestList* nbest,
        size_t i,
        // TODO: use wchar_t
        const wchar_t** words,
        size_t wordsLen);

extern "C" __declspec(dllexport)
float GetUnidecNBestListConfidence(
        const IUnidecSearchGraphCombo* graphs,
        const IUnidecNBestList* nbest,
        size_t i);

extern "C" __declspec(dllexport)
float GetUnidecNBestListReserved(
        const IUnidecNBestList* nbest,
        size_t i);


struct UnidecFramePosition
{
    UnidecFramePosition(size_t startIndex, size_t frameIndex, double milliseconds)
        : SentenceStartIndex(startIndex), SentenceStartMilliseconds(milliseconds / (frameIndex+1) * (startIndex+1)), Index(frameIndex), Milliseconds(milliseconds)
    {
    }

    size_t Index;
    double Milliseconds;
    size_t SentenceStartIndex;
    double SentenceStartMilliseconds;
};


class IUnidecEngine;

struct AudioStreamDescriptor;

typedef size_t(__stdcall *AudioStreamReadCallback)(
    AudioStreamDescriptor* pAudioStream,
    void *pBuffer,
    size_t maxSize);

typedef bool(__stdcall *AudioStreamIsEndCallback)(
    AudioStreamDescriptor* pAudioStream);

typedef bool(__stdcall *AudioStreamIsErrorCallback)(
    AudioStreamDescriptor* pAudioStream);

#pragma pack(push, 1)
struct AudioStreamDescriptor
{
    /* The stream Id, provided as wavId in the callbacks*/
    const wchar_t* StreamId;

    /* Callback context to be used by stream providers*/
    void* CallbackContext;

    /* WAVEFORMATEX equivalent (no windows dependencies) */
    size_t        wFormatTag;         /* format type */
    size_t        nChannels;          /* number of channels (i.e. mono, stereo...) */
    size_t        nSamplesPerSec;     /* sample rate */
    size_t        nAvgBytesPerSec;    /* for buffer estimation */
    size_t        nBlockAlign;        /* block size of data */
    size_t        wBitsPerSample;     /* number of bits per sample of mono data */

   /* Timeout and Segmentation configuration */
    size_t StartTimeout;
    size_t EndTimeout;
    size_t SilenceThreshold;
    size_t WordCountThreshold;
    size_t AudioLengthThreshold;

    /* A callback function to read the stream */
    AudioStreamReadCallback ReadCallback;

    /* A callback function to check for end of stream */
    AudioStreamIsEndCallback IsEndCallback;

    /* A callback function to check for stream error */
    AudioStreamIsErrorCallback IsErrorCallback;
};
#pragma pack(pop)

typedef bool(__stdcall *UnidecNextStreamCallback)(
    AudioStreamDescriptor** pAudioStream,
    bool* enableSegmentation,
    void* callbackContext);

typedef bool (__stdcall *UnidecNextCallback)(
        wchar_t* wavSpec,
        size_t wavSpecLen,
        bool* enableSegmentation,
        void* callbackContext);

typedef void (__stdcall *UnidecIntermediateCallback)(
        const wchar_t* wavId,
        size_t sentenceIndex,
        const UnidecFramePosition& framePos,
        const IUnidecIntermediateResult* intermediateResult,
        void* callbackContext);

typedef void (__stdcall *UnidecSentenceCallback)(
        const wchar_t* wavId,
        size_t sentenceIndex,
        const UnidecFramePosition& framePos,
        const IUnidecNBestList* nbest,
        void* callbackContext);

enum class UnidecEndReason
{
    EndOfAudio,
    StartTimeout,
    EndTimeout,
    OpenInputStreamFailed
};

typedef void (__stdcall *UnidecEndCallback)(
        const wchar_t* wavId,
        UnidecEndReason reason,
        void* callbackContext);

extern "C" __declspec(dllexport)
IUnidecEngine* CreateUnidecEngine(
        const IUnidecSearchGraphCombo* graphs,
        const wchar_t* in_feSpec,
        const wchar_t* in_dnnSpec,
        size_t beamSize,
        float beamThreshold,
        size_t nbestBeamSize,
        UnidecNextCallback nextCallback,
        UnidecIntermediateCallback intermediateCallback = nullptr,
        UnidecSentenceCallback sentenceCallback = nullptr,
        UnidecEndCallback endCallback = nullptr,
        void* callbackContext = nullptr);

extern "C" __declspec(dllexport)
IUnidecEngine* CreateUnidecStreamEngine(
    const IUnidecSearchGraphCombo* graphs,
    const wchar_t* in_feSpec,
    const wchar_t* in_dnnSpec,
    size_t beamSize,
    float beamThreshold,
    size_t nbestBeamSize,
    UnidecNextStreamCallback nextCallback,
    UnidecIntermediateCallback intermediateCallback = nullptr,
    UnidecSentenceCallback sentenceCallback = nullptr,
    UnidecEndCallback endCallback = nullptr,
    void* callbackContext = nullptr);

extern "C" __declspec(dllexport)
void StartUnidecEngine(
        IUnidecEngine* engine);

extern "C" __declspec(dllexport)
void JoinUnidecEngine(
        IUnidecEngine* engine);

extern "C" __declspec(dllexport)
bool GetUnidecEngineStarted(
        IUnidecEngine* engine);

extern "C" __declspec(dllexport)
void DeleteUnidecEngine(
        IUnidecEngine* engine);

extern "C" __declspec(dllexport)
void ReportUnidecEnginesPerfCounters(
        IUnidecEngine** engines,
        size_t engineCnt);
