// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "private-iot-cortana-sdk.h"
#include "SKP_Silk_SDK_API.h"

const char kSilkV3[] = "#!SILK_V3";
#define SILK_SAMPLESPERFRAME        320
#define SILK_MAXBYTESPERBLOCK       (SILK_SAMPLESPERFRAME * sizeof(uint16_t)) // VBR
#define MAX_BYTES_PER_FRAME         250 // Equals peak bitrate of 100 kbps 

static bool                         encoderSentHeader = false;
static AUDIO_ENCODER_ONENCODEDDATA  onEncodeDataCallback = NULL;
static void*                        pEncodeDataCallbackContext = NULL;

#define WAVE_FORMAT_SIZE            (2 + 2 + 4 + 4 + 2 + 2)
#define WAVE_FORMAT_PCM             1
#define AUDIO_CHANNEL               1
#define AUDIO_BITS                  16
#define AUDIO_BLOCK_ALIGN           (AUDIO_CHANNEL * (AUDIO_BITS >> 3))
#define AUDIO_BYTE_RATE             (AUDIO_SAMPLE_RATE * AUDIO_BLOCK_ALIGN)
#define MAX_SILK_FRAMES             5
#define PREBUFFERTIME_SEC           2
/*
 * For optimal network performance, segment audio chunks sent to the Speech service	
 * in CAPPED_SIZE chunks worth of uncompressed PCM audio. 
 */
#define CAPPED_SIZE                 (AUDIO_BYTE_RATE / 4) // 250ms worth of audio

static void QueueNextAudio(SPEECH_HANDLE hSpeech);

const AUDIO_WAVEFORMAT kSilkFormat =
{
    WAVE_FORMAT_PCM,
    AUDIO_CHANNEL,
    AUDIO_SAMPLE_RATE,
    AUDIO_BYTE_RATE,
    AUDIO_BLOCK_ALIGN,
    AUDIO_BITS
};

static void*   hEncoder = NULL;
static uint8_t encoderBuffer[SILK_MAXBYTESPERBLOCK];
static size_t  encoderBufferOffset = 0;
static BUFFER_HANDLE hEncodedBuffer = NULL;
static size_t silkBufferOffset = 0; // compressed offset
static size_t pcmSize = 0;          // uncompressed offset
const  SKP_SILK_SDK_EncControlStruct encControl = {
    AUDIO_SAMPLE_RATE,
    AUDIO_SAMPLE_RATE,
    SILK_SAMPLESPERFRAME,
    36000,
    0,
    0,
    0,
    0,
};

static void FlushEncoderBuffer()
{
    metrics_encoder_flush();
    if (silkBufferOffset)
    {
        onEncodeDataCallback(BUFFER_u_char(hEncodedBuffer), silkBufferOffset, pEncodeDataCallbackContext);
        silkBufferOffset = 0;
    }
}

// Zhou: it saves the data in buffer, and only when size is 0, fflush the buffer (FlushEncoderBuffer), then call onEncodeDataCallback indicates fflushed.
static void onEncodedData(const uint8_t* pdata, size_t size)
{
    if (!size)
    {
        FlushEncoderBuffer();
        onEncodeDataCallback(NULL, 0, pEncodeDataCallbackContext);
        return;
    }
    else if (NULL == hEncodedBuffer)
    {
        hEncodedBuffer = BUFFER_create(pdata, size);
        if (NULL == hEncodedBuffer)
        {
            return;
        }
        silkBufferOffset = size;
    }
    else
    {
        size_t avail = BUFFER_length(hEncodedBuffer) - silkBufferOffset;
        if (size > avail)
        {
            if (BUFFER_enlarge(hEncodedBuffer, size - avail))
            {
                return;
            }
        }
        memcpy(BUFFER_u_char(hEncodedBuffer) + silkBufferOffset, pdata, size);
        silkBufferOffset += size;
    }
}

// Zhou: encode the buffer into Silk format, then call onEncodedData().
static int audio_silk_encodeframe(
    const uint8_t* pBuffer)
{
    struct 
    {
        SKP_int16 nBytes;
        SKP_uint8 payload[MAX_BYTES_PER_FRAME * MAX_SILK_FRAMES];
    } outSample;

    SKP_int ret = 0;

    outSample.nBytes = sizeof(outSample.payload);
    ret = SKP_Silk_SDK_Encode(hEncoder, &encControl, (short*)pBuffer, SILK_SAMPLESPERFRAME, outSample.payload, &outSample.nBytes);
    if (ret)
    {
        LogInfo("failed to encode %d\n", ret);
        return ret;
    }

    if (outSample.nBytes)
    {
        onEncodedData((const uint8_t*)&outSample, sizeof(uint16_t) + (size_t)outSample.nBytes);
    }

    return ret;
}

// zhou: append the pBuffer to the data already there, and call audio_silk_encodeframe() if the buffer is full repeatly until the buffer is not full. 
// There might be data left in the buffer, which will be sent next time when this function is called.
int audio_encoder_write(
    const uint8_t* pBuffer,
    size_t         byteToWrite) 
{
    SKP_int ret = 0;
    SKP_SILK_SDK_EncControlStruct status;
    size_t    chunkBytes;

    if (NULL == onEncodeDataCallback)
    {
        LogError("audio_encoder_initialize not called");
        return -1;
    }
    else if (NULL == pBuffer)
    {
        LogError("No buffer passed in");
        return -1;
    }
    else if (0 == byteToWrite)
    {
        return 0;
    }

    // allocate the encoder
    if (!hEncoder)
    {
        SKP_int32 size = 0;
        ret = SKP_Silk_SDK_Get_Encoder_Size(&size);
        if (ret)
        {
            return ret;
        }

        hEncoder = malloc((size_t)size);
        if (!hEncoder)
        {
            return -1;
        }

        encoderSentHeader = false;
    }

    pcmSize += byteToWrite;
    if (pcmSize >= CAPPED_SIZE)
    {
        FlushEncoderBuffer();
        pcmSize -= CAPPED_SIZE;
    }

    // prepare the encoder
    if (!encoderSentHeader)
    {
        metrics_encoder_init();
        ret = SKP_Silk_SDK_InitEncoder(hEncoder, &status);
        if (ret)
        {
            return ret;
        }

        onEncodedData((uint8_t*)kSilkV3, sizeof(kSilkV3) - 1);
        encoderBufferOffset = 0;
        encoderSentHeader = true;
    }

    // fill any previous buffer if there is one outstanding.
    if (encoderBufferOffset)
    {
        chunkBytes = sizeof(encoderBuffer) - encoderBufferOffset;
        if (chunkBytes > byteToWrite)
        {
            chunkBytes = byteToWrite;
        }

        memcpy(encoderBuffer + encoderBufferOffset, pBuffer, chunkBytes);
        pBuffer += chunkBytes;
        byteToWrite -= chunkBytes;
        encoderBufferOffset += chunkBytes;

        if (encoderBufferOffset == SILK_MAXBYTESPERBLOCK)
        {
            encoderBufferOffset = 0;
            ret = audio_silk_encodeframe(encoderBuffer);
            if (ret)
            {
                return ret;
            }
        }
    }

    while (byteToWrite >= SILK_MAXBYTESPERBLOCK)
    {
        ret = audio_silk_encodeframe(pBuffer);
        if (ret)
        {
            return ret;
        }

        pBuffer += SILK_MAXBYTESPERBLOCK;
        byteToWrite -= SILK_MAXBYTESPERBLOCK;
    }

    // carry over any remaining bytes
    if (byteToWrite)
    {
        memcpy(encoderBuffer, pBuffer, byteToWrite);
        encoderBufferOffset = byteToWrite;
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////////

int audio_encoder_flush(void)
{
    if (NULL == onEncodeDataCallback)
    {
        LogError("audio_encoder_initialize not called");
        return -1;
    }

    pcmSize = 0;

    if (encoderSentHeader)
    {
        encoderSentHeader = false;
        const uint16_t silkTerm = 0xffff;
        onEncodedData((const uint8_t*)&silkTerm, sizeof(silkTerm));
    }

    // signal the end of the stream with a zero length array.
    onEncodedData(NULL, 0);
    return 0;
}

int audio_encoder_initialize(
    AUDIO_ENCODER_ONENCODEDDATA onDataCallback,
    void* pContext)
{
    if (NULL == onDataCallback)
    {
        return -1;
    }

    onEncodeDataCallback = onDataCallback;
    pEncodeDataCallbackContext = pContext;
    encoderSentHeader = false;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

#if defined(USE_SILK_DECODE)
static void*     hDecoder = NULL;

static int silk_decode_frame(
    const SKP_uint8 *inData,
    SKP_int         nBytesIn,
    SKP_int16*      outData,
    size_t          *nBytesOut)
{
    SKP_int16 len;
    int       tot_len = 0;
    SKP_int   ret;
    SKP_SILK_SDK_DecControlStruct DecControl;
    SKP_int   decodedBytes;

    DecControl.API_sampleRate = AUDIO_SAMPLE_RATE;

    // Loop through frames in packet
    do
    {
        // Decode one frame
        ret = SKP_Silk_SDK_Decode(hDecoder, &DecControl, 0, inData, nBytesIn, outData, &len);
        if (ret)
        {
            break;
        }

        outData += len;
        tot_len += len;

        decodedBytes = DecControl.frameSize * DecControl.framesPerPacket;
        if (nBytesIn >= decodedBytes)
        {
            inData += decodedBytes;
            nBytesIn -= decodedBytes;
            DecControl.moreInternalDecoderFrames = 1;
        }

    } while (DecControl.moreInternalDecoderFrames);

    // Convert short array count to byte array count
    *nBytesOut = (size_t)tot_len * sizeof(SKP_int16);

    return ret;
}

#endif

#if defined(USE_SILK_DECODE)
static int initdecoder()
{
    SKP_int ret;
    if (!hDecoder)
    {
        SKP_int32 decsize = 0;
        ret = SKP_Silk_SDK_Get_Decoder_Size(&decsize);
        if (ret)
        {
            return ret;
        }

        hDecoder = malloc((size_t)decsize);
        if (!hDecoder)
        {
            return -1;
        }
    }

    return 0;
}
#endif

const char *audio_decoder_tts_format(void)
{
    return "riff-16khz-16bit-mono-pcm";
}

void audio_decoder_uninitialize(void)
{
#if defined(USE_SILK_DECODE)
    if (hDecoder)
    {
        free(hDecoder);
        hDecoder = NULL;
    }
#endif
}

void audio_encoder_uninitialize(void)
{
    if (hEncoder)
    {
        free(hEncoder);
        hEncoder = NULL;
    }
    if (NULL != hEncodedBuffer)
    {
        BUFFER_delete(hEncodedBuffer);
		hEncodedBuffer = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////

typedef struct _AsyncAudio
{
    SPEECH_HANDLE   hSpeech;
    IOBUFFER*       pIoBuffer;
    BUFFER_HANDLE   hTmpDecodingBuffer;
    size_t          offset;
    PCONTENT_ASYNCCOMPLETE_CALLBACK pCB;
    void*           pContext;
    int             playing;
    uint64_t        decodeStartTime;
    uint64_t        playTime;
    uint64_t        totalAudioTime;
    uint64_t        audioStartPlayTime;
    char            requestId[37];
} AsyncAudio;

static void WaitAudioBufferring(IOBUFFER* pIoBffer, int nSeconds)
{
    BUFFER_HANDLE hBuffer = BUFFER_new();
    int nReadySeconds = 0;
    do
    {
        int nBlocks = 0;
        int nBytes = IOBUFFER_GetUnReadBytes(pIoBffer);
        size_t buffersize = 0;
        BUFFER_size(hBuffer, &buffersize);
        if (nBytes > (int)buffersize)
        {
            BUFFER_enlarge(hBuffer, nBytes - buffersize);
        }

        unsigned char* pTempBuf = BUFFER_u_char(hBuffer);
        if (IOBUFFER_PeekRead(pIoBffer, pTempBuf, 0, nBytes, TIMEOUT_STREAMCHUNK_READING) == -1)
        {
            break;
        }

        if (IOBUFFER_GetTotalBytes(pIoBffer) != -1)
        {
            break; // no new bytes will come, all received.
        }
        for (int i = 0; i < nBytes;)
        {
            i += sizeof(uint16_t) + *((uint16_t*)(pTempBuf + i));
            nBlocks++;
        }

        nReadySeconds = nBlocks / 10;
        if (nReadySeconds < nSeconds || nSeconds == -1)
        {
            if (IOBUFFER_WaitForNewBytes(pIoBffer, TIMEOUT_STREAMCHUNK_READING) == -1)
            {
                break;
            }
        }
    } while (nReadySeconds < nSeconds || nSeconds == -1);
    BUFFER_delete(hBuffer);
}

static int CheckUnreadAudioLengthMs(IOBUFFER* pIoBffer)
{
    BUFFER_HANDLE hBuffer = BUFFER_new();
    if (hBuffer == 0)
    {
        return 0;
    }

    int nMs = 0;
    int nBytes = IOBUFFER_GetUnReadBytes(pIoBffer);
    BUFFER_enlarge(hBuffer, (size_t)nBytes);
    unsigned char* pTempBuf = BUFFER_u_char(hBuffer);
    if (nBytes > 0 && pTempBuf)
    {
        IOBUFFER_PeekRead(pIoBffer, pTempBuf, 0, nBytes, TIMEOUT_STREAMCHUNK_READING);
        for (int i = 0; i < nBytes;)
        {
            i += sizeof(uint16_t) + *((uint16_t*)(pTempBuf + i));
            nMs += 100;
        }
    }

    BUFFER_delete(hBuffer);
    return nMs;
}

static void AsyncAudioBufferUnderRunCB(void* pContext)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;
    AsyncAudio *aAudio = 0;
    Lock(pSC->PlaylistLock);
    aAudio = queue_peek(pSC->hPlayList);
    Unlock(pSC->PlaylistLock);
    if (!aAudio)
    {
        return;
    }

    // sometime, audio buffer underrun is not caused by lack of data, 
    // maybe due to machine busy, so, check data readyness to filter such cases
    if (CheckUnreadAudioLengthMs(aAudio->pIoBuffer) < 100)
    {
        WaitAudioBufferring(aAudio->pIoBuffer, PREBUFFERTIME_SEC);
        uint64_t currDecodingTime = cortana_gettickcount() - aAudio->decodeStartTime;
        LogInfo("Sound Delay!!! Playing time: %dms, decoding time: %dms", (int)aAudio->playTime, (int)currDecodingTime);
        metrics_tts_audio_buffer_underrun((long)(currDecodingTime - aAudio->playTime));
        aAudio->playTime = 0;
        aAudio->decodeStartTime = 0;
    }
}

static int AsyncRead(void* pContext, uint8_t* pBuffer, size_t size)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;
    AsyncAudio *aAudio;
    size_t      nBytes;
    uint16_t len;
    int ret;

    Lock(pSC->PlaylistLock);
    aAudio = queue_peek(pSC->hPlayList);
    Unlock(pSC->PlaylistLock);

    if (!aAudio || aAudio->offset == (size_t)IOBUFFER_GetTotalBytes(aAudio->pIoBuffer))
    {
        return -1;
    }

    ret = initdecoder();
    if (ret)
    {
        return ret;
    }

    if (!aAudio->offset)
    {
        // for the first block, we need fill buffer first
        uint64_t t = cortana_gettickcount();
        WaitAudioBufferring(aAudio->pIoBuffer, PREBUFFERTIME_SEC);
        t = cortana_gettickcount() - t;
        if (t > PREBUFFERTIME_SEC * 1000)
        {
            // Network speed can't support real time play, wait for all received before playing
            LogInfo("First %d seconds audio consumed %dms network time.", PREBUFFERTIME_SEC, (int)t);
            LogInfo("Network speed can't support real time play, wait for all received before playing.");
            metrics_tts_audio_nonstreaming((long)t);
            WaitAudioBufferring(aAudio->pIoBuffer, -1);
        }

        metrics_tts_start(aAudio->requestId);
        // prepare the decoder for first time use
        ret = SKP_Silk_SDK_InitDecoder(hDecoder);
        if (ret)
        {
            return ret;
        }
    }

    ret = IOBUFFER_Read(aAudio->pIoBuffer, &len, 0, sizeof(len), TIMEOUT_STREAMCHUNK_READING);
    if (ret == -1)
    {
        return -1;
    }

    if (BUFFER_length(aAudio->hTmpDecodingBuffer) < len)
    {
        BUFFER_enlarge(aAudio->hTmpDecodingBuffer, len);
    }

    uint8_t* pTempBuf = BUFFER_u_char(aAudio->hTmpDecodingBuffer);
    ret = IOBUFFER_Read(aAudio->pIoBuffer, pTempBuf, 0, len, TIMEOUT_STREAMCHUNK_READING);
    if (ret == -1)
    {
        return -1;
    }

    nBytes = size;
    ret = silk_decode_frame(
        pTempBuf,
        len,
        (short*)(pBuffer),
        &nBytes);
    if (ret)
    {
        metrics_tts_decode_error(ret);
    }

    aAudio->offset += sizeof(len) + len;

    if (!aAudio->decodeStartTime)
    {
        aAudio->decodeStartTime = cortana_gettickcount();
    }

    aAudio->playTime += 100; // 100ms per frame
    aAudio->totalAudioTime += 100;
    if (aAudio->audioStartPlayTime == 0)
    {
        aAudio->audioStartPlayTime = cortana_gettickcount();
    }

    if (nBytes)
    {
        // resignal the TTS output flag while we have TTS to playback.
        // TODO: Address the removal of this flag entirely and corresponding state changes driven by AudioOutput_StateChanged.
        pSC->TTSOutput = 1;
    }

    return (int)nBytes;
}

static void QueueNextAudio(SPEECH_HANDLE hSpeech);

static void AsyncComplete(void* pContext)
{
    AsyncAudio *aAudio;
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;

    if (!pSC)
    {
        return;
    }

    Lock(pSC->PlaylistLock);

    aAudio = queue_dequeue(pSC->hPlayList);
    if (aAudio)
    {
        metrics_tts_stop(aAudio->requestId);
    }

    Unlock(pSC->PlaylistLock);

    if (aAudio)
    {
        uint64_t playtime = cortana_gettickcount() - aAudio->audioStartPlayTime;
        aAudio->totalAudioTime += CheckUnreadAudioLengthMs(aAudio->pIoBuffer);
        LogInfo("tts audio length: %d ms, approximate played: %d ms", (int)aAudio->totalAudioTime, (int)playtime);
        metrics_tts_audio_length((long)aAudio->totalAudioTime, (long)playtime);
        if (aAudio->pCB)
        {
            aAudio->pCB(aAudio->pContext);
        }

        if (aAudio->hTmpDecodingBuffer)
        {
            BUFFER_delete(aAudio->hTmpDecodingBuffer);
            aAudio->hTmpDecodingBuffer = 0;
        }

        free(aAudio);
    }

    QueueNextAudio(pSC);
}

SPEECH_RESULT AsyncAudioDecode(
    void*           hSpeech,
    const char*     pszPath,
    uint8_t*        pBuffer,
    size_t          bufferSize,
    IOBUFFER*       pIoBuffer,
    PCONTENT_ASYNCCOMPLETE_CALLBACK pCB,
    void*           pAsyncContext)
{
    (void)pszPath;
    (void)pBuffer;
    (void)bufferSize;

    AsyncAudio *aAudio;
    SPEECH_CONTEXT* pContext = (SPEECH_CONTEXT*)hSpeech;

    if (!hSpeech || !pCB)
    {
        return -1;
    }
    else if (cortana_get_mute(hSpeech))
    {
        return 0;
    }

    aAudio = (AsyncAudio*)malloc(sizeof(AsyncAudio));
    if (!aAudio)
    {
        return -1;
    }

    memset(aAudio, 0, sizeof(AsyncAudio));
    BUFFER_HANDLE hBuf = BUFFER_new();
    if (!hBuf)
    {
        return -1;
    }

    aAudio->hSpeech = hSpeech;
    aAudio->offset = 0;
    aAudio->pIoBuffer = pIoBuffer;
    aAudio->pCB = pCB;
    aAudio->pContext = pAsyncContext;
    aAudio->playing = 0;
    aAudio->hTmpDecodingBuffer = hBuf;
    IOBUFFER_AddRef(pIoBuffer);
    strcpy_s(aAudio->requestId, 37, transport_get_request_id(pContext->mSpeechRequest));

    Lock(pContext->PlaylistLock);
    queue_enqueue(pContext->hPlayList, aAudio);
    Unlock(pContext->PlaylistLock);

    QueueNextAudio(hSpeech);
    return CORTANA_ERROR_PENDING;
}

static void QueueNextAudio(SPEECH_HANDLE hSpeech)
{
    SPEECH_CONTEXT*  pContext = (SPEECH_CONTEXT*)hSpeech;
    AsyncAudio*      aAudio = NULL;
    AUDIO_SYS_HANDLE hAudioDevice;

    hAudioDevice = cortana_getaudiodevice(hSpeech, CORTANA_AUDIO_TYPE_VOICE);

    Lock(pContext->PlaylistLock);

    aAudio = queue_peek(pContext->hPlayList);
    if (aAudio)
    {
        if (!aAudio->playing)
        {
            // set the audio to playing
            pContext->TTSOutput = 1;
            aAudio->playing = 1;
        }
        else
        {
            // don't queue the audio up.
            aAudio = NULL;
        }
    }

    Unlock(pContext->PlaylistLock);

    if (NULL != aAudio)
    {
        (void)audio_output_stop(hAudioDevice);
        (void)audio_output_stop(cortana_getaudiodevice(hSpeech, CORTANA_AUDIO_TYPE_EARCON));

        if (audio_output_startasync(
            hAudioDevice,
            &kSilkFormat,
            AsyncRead,
            AsyncComplete,
            AsyncAudioBufferUnderRunCB,
            hSpeech))
        {
            AsyncComplete(hSpeech);
        }
    }
}

int tts_is_playing(SPEECH_CONTEXT* pSC)
{
	int         ret = 0;
	AsyncAudio* aAudio;

	Lock(pSC->PlaylistLock);
	aAudio = queue_peek(pSC->hPlayList);
	ret = pSC->TTSOutput || (aAudio && (aAudio->offset != (size_t)IOBUFFER_GetTotalBytes(aAudio->pIoBuffer)));
	Unlock(pSC->PlaylistLock);

    return ret;
}

///////////////////////////////////////////////////////////////////////////////
