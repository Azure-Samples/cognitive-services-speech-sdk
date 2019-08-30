//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_speech_synthesis_result.h: Public API declarations for SpeechSynthesisResult C++ class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_cxx_enums.h>
#include <speechapi_cxx_properties.h>
#include <speechapi_c_result.h>
#include <speechapi_c_synthesizer.h>
#include <speechapi_c_audio_stream.h>


#define AUDIO_OUTPUT_BUFFER_SIZE 0x1000000

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


/// <summary>
/// Contains information about result from text-to-speech synthesis.
/// Added in version 1.4.0
/// </summary>
class SpeechSynthesisResult
{
private:

    /// <summary>
    /// Internal member variable that holds the tts result handle.
    /// </summary>
    SPXRESULTHANDLE m_hresult;

    /*! \cond PRIVATE */

    class PrivatePropertyCollection : public PropertyCollection
    {
    public:
        PrivatePropertyCollection(SPXRESULTHANDLE hresult) :
            PropertyCollection(
                [=]() {
            SPXPROPERTYBAGHANDLE hpropbag = SPXHANDLE_INVALID;
            synth_result_get_property_bag(hresult, &hpropbag);
            return hpropbag;
        }())
        {
        }
    };

    /// <summary>
    /// Internal member variable that holds the properties associating to the tts result.
    /// </summary>
    PrivatePropertyCollection m_properties;

    /*! \endcond */

public:

    /// <summary>
    /// Creates a new instance using the provided handle.
    /// </summary>
    /// <param name="hresult">Result handle.</param>
    explicit SpeechSynthesisResult(SPXRESULTHANDLE hresult) :
        m_hresult(hresult),
        m_properties(hresult),
        ResultId(m_resultId),
        Reason(m_reason),
        Properties(m_properties)
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

        const size_t maxCharCount = 1024;
        char sz[maxCharCount + 1];

        SPXSTRING resultId;
        SPX_THROW_ON_FAIL(synth_result_get_result_id(hresult, sz, maxCharCount));
        m_resultId = Utils::ToSPXString(sz);

        Result_Reason resultReason;
        SPX_THROW_ON_FAIL(synth_result_get_reason(hresult, &resultReason));
        m_reason = (ResultReason)resultReason;
    }

    /// <summary>
    /// Gets the length of synthesized audio
    /// </summary>
    /// <returns>Length of synthesized audio</returns>
    uint32_t GetAudioLength()
    {
        uint32_t audioLength = 0;
        synth_result_get_audio_length(m_hresult, &audioLength);
        return audioLength;
    }

    /// <summary>
    /// Gets the synthesized audio
    /// </summary>
    /// <returns>Synthesized audio data</returns>
    std::shared_ptr<std::vector<uint8_t>> GetAudioData()
    {
        uint32_t audioLength = 0;
        SPX_THROW_ON_FAIL(synth_result_get_audio_length(m_hresult, &audioLength));

        m_audiodata.resize(audioLength);

        if (audioLength > 0)
        {
            uint32_t filledSize = 0;
            SPX_THROW_ON_FAIL(synth_result_get_audio_data(m_hresult, m_audiodata.data(), audioLength, &filledSize));
        }

        return std::make_shared<std::vector<uint8_t>>(m_audiodata);
    }

    /// <summary>
    /// Explicit conversion operator.
    /// </summary>
    /// <returns>A handle.</returns>
    explicit operator SPXRESULTHANDLE() { return m_hresult; }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~SpeechSynthesisResult()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        synthesizer_result_handle_release(m_hresult);
    }

    /// <summary>
    /// Unique result id.
    /// </summary>
    const SPXSTRING& ResultId;

    /// <summary>
    /// Reason of the synthesis result.
    /// </summary>
    const ResultReason& Reason;

    /// <summary>
    /// Collection of additional SpeechSynthesisResult properties.
    /// </summary>
    const PropertyCollection& Properties;

private:

    DISABLE_DEFAULT_CTORS(SpeechSynthesisResult);

    /// <summary>
    /// Internal member variable that holds the result ID.
    /// </summary>
    SPXSTRING m_resultId;

    /// <summary>
    /// Internal member variable that holds the result reason.
    /// </summary>
    ResultReason m_reason;

    /// <summary>
    /// Internal member variable that holds the audio data
    /// </summary>
    std::vector<uint8_t> m_audiodata;

    template <class T>
    static std::shared_ptr<T> SpxAllocSharedBuffer(size_t sizeInBytes)
    {
        auto ptr = reinterpret_cast<T*>(new uint8_t[sizeInBytes]);
        auto deleter = [](T* p) { delete[] reinterpret_cast<uint8_t*>(p); };

        std::shared_ptr<T> buffer(ptr, deleter);
        return buffer;
    }
};


/// <summary>
/// Represents audio data stream used for operating audio data as a stream.
/// Added in version 1.4.0
/// </summary>
class AudioDataStream : public std::enable_shared_from_this<AudioDataStream>
{
private:

    /// <summary>
    /// Internal member variable that holds the smart handle.
    /// </summary>
    SmartHandle<SPXAUDIOSTREAMHANDLE, &audio_data_stream_release> m_haudioStream;

    /*! \cond PRIVATE */

    class PrivatePropertyCollection : public PropertyCollection
    {
    public:
        PrivatePropertyCollection(SPXAUDIOSTREAMHANDLE hstream) :
            PropertyCollection(
                [=]() {
            SPXPROPERTYBAGHANDLE hpropbag = SPXHANDLE_INVALID;
            audio_data_stream_get_property_bag(hstream, &hpropbag);
            return hpropbag;
        }())
        {
        }
    };

    /// <summary>
    /// Internal member variable that holds the properties associating to the audio data stream.
    /// </summary>
    PrivatePropertyCollection m_properties;

    /*! \endcond */

public:

    /// <summary>
    /// Creates a memory backed AudioDataStream from given speech synthesis result.
    /// </summary>
    /// <param name="result">The speech synthesis result.</param>
    /// <returns>A shared pointer to AudioDataStream</returns>
    static std::shared_ptr<AudioDataStream> FromResult(std::shared_ptr<SpeechSynthesisResult> result)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        if (result != nullptr)
        {
            hresult = (SPXRESULTHANDLE)(*result.get());
        }

        SPXAUDIOSTREAMHANDLE hstream = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_data_stream_create_from_result(&hstream, hresult));

        auto stream = new AudioDataStream(hstream);
        return std::shared_ptr<AudioDataStream>(stream);
    }

    /// <summary>
    /// Get current status of the audio data stream.
    /// </summary>
    /// <returns>Current status</returns>
    StreamStatus GetStatus()
    {
        Stream_Status status;
        SPX_THROW_ON_FAIL(audio_data_stream_get_status(m_haudioStream, &status));
        return (StreamStatus)status;
    }

    /// <summary>
    /// Check whether the stream has enough data to be read.
    /// </summary>
    /// <param name="bytesRequested">The requested data size in bytes.</param>
    /// <returns>A bool indicating whether the stream has enough data to be read.</returns>
    bool CanReadData(uint32_t bytesRequested)
    {
        return audio_data_stream_can_read_data(m_haudioStream, bytesRequested);
    }

    /// <summary>
    /// Check whether the stream has enough data to be read, starting from the specified position.
    /// </summary>
    /// <param name="pos">The position counting from start of the stream.</param>
    /// <param name="bytesRequested">The requested data size in bytes.</param>
    /// <returns>A bool indicating whether the stream has enough data to be read.</returns>
    bool CanReadData(uint32_t pos, uint32_t bytesRequested)
    {
        return audio_data_stream_can_read_data_from_position(m_haudioStream, bytesRequested, pos);
    }

    /// <summary>
    /// Reads a chunk of the audio data and fill it to given buffer
    /// </summary>
    /// <param name="buffer">A buffer to receive read data.</param>
    /// <param name="bufferSize">Size of the buffer.</param>
    /// <returns>Size of data filled to the buffer, 0 means end of stream</returns>
    uint32_t ReadData(uint8_t* buffer, uint32_t bufferSize)
    {
        uint32_t filledSize = 0;
        SPX_THROW_ON_FAIL(audio_data_stream_read(m_haudioStream, buffer, bufferSize, &filledSize));

        return filledSize;
    }

    /// <summary>
    /// Reads a chunk of the audio data and fill it to given buffer, starting from the specified position.
    /// </summary>
    /// <param name="pos">The position counting from start of the stream.</param>
    /// <param name="buffer">A buffer to receive read data.</param>
    /// <param name="bufferSize">Size of the buffer.</param>
    /// <returns>Size of data filled to the buffer, 0 means end of stream</returns>
    uint32_t ReadData(uint32_t pos, uint8_t* buffer, uint32_t bufferSize)
    {
        uint32_t filledSize = 0;
        SPX_THROW_ON_FAIL(audio_data_stream_read_from_position(m_haudioStream, buffer, bufferSize, pos, &filledSize));

        return filledSize;
    }

    /// <summary>
    /// Save the audio data to a file, synchronously.
    /// </summary>
    /// <param name="fileName">The file name with full path.</param>
    void SaveToWavFile(const SPXSTRING& fileName)
    {
        SPX_THROW_ON_FAIL(audio_data_stream_save_to_wave_file(m_haudioStream, Utils::ToUTF8(fileName).c_str()));
    }

    /// <summary>
    /// Save the audio data to a file, asynchronously.
    /// </summary>
    /// <param name="fileName">The file name with full path.</param>
    /// <returns>An asynchronous operation representing the saving.</returns>
    std::future<void> SaveToWavFileAsync(const SPXSTRING& fileName)
    {
        auto keepAlive = this->shared_from_this();

        auto future = std::async(std::launch::async, [keepAlive, this, fileName]() -> void {
            SPX_THROW_ON_FAIL(audio_data_stream_save_to_wave_file(m_haudioStream, Utils::ToUTF8(fileName).c_str()));
        });

        return future;
    }

    /// <summary>
    /// Get current position of the audio data stream.
    /// </summary>
    /// <returns>Current position</returns>
    uint32_t GetPosition()
    {
        uint32_t position = 0;
        SPX_THROW_ON_FAIL(audio_data_stream_get_position(m_haudioStream, &position));
        return position;
    }

    /// <summary>
    /// Set current position of the audio data stream.
    /// </summary>
    /// <param name="pos">Position to be set.</param>
    void SetPosition(uint32_t pos)
    {
        SPX_THROW_ON_FAIL(audio_data_stream_set_position(m_haudioStream, pos));
    }

    /// <summary>
    /// Explicit conversion operator.
    /// </summary>
    /// <returns>A handle.</returns>
    explicit operator SPXAUDIOSTREAMHANDLE() { return m_haudioStream; }

    /// <summary>
    /// Collection of additional SpeechSynthesisResult properties.
    /// </summary>
    const PropertyCollection& Properties;

private:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit AudioDataStream(SPXAUDIOSTREAMHANDLE haudioStream) :
        m_haudioStream(haudioStream),
        m_properties(haudioStream),
        Properties(m_properties)
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }
};


/// <summary>
/// Contains detailed information about why a result was canceled.
/// Added in version 1.4.0
/// </summary>
class SpeechSynthesisCancellationDetails
{
private:

    CancellationReason m_reason;
    CancellationErrorCode m_errorCode;

public:

    /// <summary>
    /// Creates an instance of SpeechSynthesisCancellationDetails object for the canceled SpeechSynthesisResult.
    /// </summary>
    /// <param name="result">The result that was canceled.</param>
    /// <returns>A shared pointer to CancellationDetails.</returns>
    static std::shared_ptr<SpeechSynthesisCancellationDetails> FromResult(std::shared_ptr<SpeechSynthesisResult> result)
    {
        auto ptr = new SpeechSynthesisCancellationDetails(result.get());
        auto cancellation = std::shared_ptr<SpeechSynthesisCancellationDetails>(ptr);
        return cancellation;
    }

    /// <summary>
    /// Creates an instance of SpeechSynthesisCancellationDetails object for the canceled SpeechSynthesisResult.
    /// </summary>
    /// <param name="stream">The audio data stream that was canceled.</param>
    /// <returns>A shared pointer to CancellationDetails.</returns>
    static std::shared_ptr<SpeechSynthesisCancellationDetails> FromStream(std::shared_ptr<AudioDataStream> stream)
    {
        auto ptr = new SpeechSynthesisCancellationDetails(stream.get());
        auto cancellation = std::shared_ptr<SpeechSynthesisCancellationDetails>(ptr);
        return cancellation;
    }

    /// <summary>
    /// The reason the result was canceled.
    /// </summary>
    const CancellationReason& Reason;

    /// <summary>
    /// The error code in case of an unsuccessful speech synthesis (<see cref="Reason"/> is set to Error).
    /// If Reason is not Error, ErrorCode is set to NoError.
    /// </summary>
    const CancellationErrorCode& ErrorCode;

    /// <summary>
    /// The error message in case of an unsuccessful speech synthesis (<see cref="Reason"/> is set to Error).
    /// </summary>
    const SPXSTRING ErrorDetails;

private:

    DISABLE_DEFAULT_CTORS(SpeechSynthesisCancellationDetails);

    SpeechSynthesisCancellationDetails(SpeechSynthesisResult* result) :
        m_reason(GetCancellationReason(result)),
        m_errorCode(GetCancellationErrorCode(result)),
        Reason(m_reason),
        ErrorCode(m_errorCode),
        ErrorDetails(result->Properties.GetProperty(PropertyId::CancellationDetails_ReasonDetailedText))
    {
    }

    SpeechSynthesisCancellationDetails(AudioDataStream* stream) :
        m_reason(GetCancellationReason(stream)),
        m_errorCode(GetCancellationErrorCode(stream)),
        Reason(m_reason),
        ErrorCode(m_errorCode),
        ErrorDetails(stream->Properties.GetProperty(PropertyId::CancellationDetails_ReasonDetailedText))
    {
    }

    Speech::CancellationReason GetCancellationReason(SpeechSynthesisResult* result)
    {
        Result_CancellationReason reason;

        SPXRESULTHANDLE hresult = (SPXRESULTHANDLE)(*result);
        SPX_IFFAILED_THROW_HR(synth_result_get_reason_canceled(hresult, &reason));

        return (Speech::CancellationReason)reason;
    }

    Speech::CancellationErrorCode GetCancellationErrorCode(SpeechSynthesisResult* result)
    {
        Result_CancellationErrorCode errorCode;

        SPXRESULTHANDLE hresult = (SPXRESULTHANDLE)(*result);
        SPX_IFFAILED_THROW_HR(synth_result_get_canceled_error_code(hresult, &errorCode));

        return (Speech::CancellationErrorCode)errorCode;
    }

    Speech::CancellationReason GetCancellationReason(AudioDataStream* stream)
    {
        Result_CancellationReason reason;

        SPXAUDIOSTREAMHANDLE hstream = (SPXAUDIOSTREAMHANDLE)(*stream);
        SPX_IFFAILED_THROW_HR(audio_data_stream_get_reason_canceled(hstream, &reason));

        return (Speech::CancellationReason)reason;
    }

    Speech::CancellationErrorCode GetCancellationErrorCode(AudioDataStream* stream)
    {
        Result_CancellationErrorCode errorCode;

        SPXAUDIOSTREAMHANDLE hstream = (SPXAUDIOSTREAMHANDLE)(*stream);
        SPX_IFFAILED_THROW_HR(audio_data_stream_get_canceled_error_code(hstream, &errorCode));

        return (Speech::CancellationErrorCode)errorCode;
    }
};


} } } // Microsoft::CognitiveServices::Speech
