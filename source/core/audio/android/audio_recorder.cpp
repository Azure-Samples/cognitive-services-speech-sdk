/*
* Copyright 2015 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* This file has been modified by Microsoft on 7/2018, 10/2018, and 1/2019.
*/

#include <cstring>
#include <cstdlib>
#include "audio_recorder.h"
#include "azure_c_shared_utility/xlogging.h"
#include "data_buffer.h"

using Microsoft::CognitiveServices::Speech::Impl::DataBuffer;
/*
* bqRecorderCallback(): called for every buffer is full;
*                       pass directly to handler
*/
void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *rec) {
    (static_cast<AudioRecorder *>(rec))->ProcessSLCallback(bq);
}

void AudioRecorder::ProcessSLCallback(SLAndroidSimpleBufferQueueItf bq) {
#ifdef ENABLE_LOG
    recLog_->logTime();
#endif
    SPX_IFFALSE_THROW_HR(bq == recBufQueueItf_, SPXERR_INVALID_ARG);
    ReadAudioBuffer();
}

void AudioRecorder::ReadAudioBuffer() {
    SLuint32 state = GetRecordState();
    if (state != SL_RECORDSTATE_RECORDING) {
        LogError("[%s] incorrect state: %d", __FUNCTION__, state);
        return;
    }

    DataBuffer dataBuf;
    dataBuf.data = static_cast<unsigned char*>(audioBuffers_[audioBufferIndex_].get());
    dataBuf.size = sampleInfo_.bufSize_;

#ifdef ENABLE_LOG
    recLog_->log(dataBuf.data, dataBuf.size);
#endif

    callback_(ctx_, ENGINE_SERVICE_MSG_RECORDED_AUDIO_AVAILABLE, &dataBuf);
    EnqueueAudioBuffer();
}

bool AudioRecorder::EnqueueAudioBuffer() {
    SLresult err = (*recBufQueueItf_)->Enqueue(recBufQueueItf_, audioBuffers_[audioBufferIndex_].get(), sampleInfo_.bufSize_);
    if (SL_RESULT_SUCCESS != err) {
        LogError("%s Enqueue fails and return false, err: %d", __FUNCTION__, err);
        SLASSERT(err);
        return false;
    }
    audioBufferIndex_ = (audioBufferIndex_ + 1) % DEVICE_SHADOW_BUFFER_QUEUE_LEN;
    return true;
}

SLuint32 AudioRecorder::GetDeviceID(SLObjectItf slEngineObject, const std::string& deviceName)
{
    // configure audio source
    // default is the default input device
    SLuint32 deviceID = SL_DEFAULTDEVICEID_AUDIOINPUT;

    // in case we got a configured device, try to find it.
    if (!deviceName.empty())
    {
        LogInfo("RequestedInput specific deviceName '%s'", deviceName.c_str());

        bool foundDeviceName = false;
        SLAudioIODeviceCapabilitiesItf audioIODeviceCapabilitiesItf;

        // retrieve the audio capabilities
        SLresult result = (*slEngineObject)->GetInterface(slEngineObject, SL_IID_AUDIOIODEVICECAPABILITIES, (void*)&audioIODeviceCapabilitiesItf);
        SLASSERT(result);

#define MAX_NUMBER_INPUT_DEVICES 16
        SLint32 numInputs = MAX_NUMBER_INPUT_DEVICES;
        SLuint32 inputDeviceIDs[MAX_NUMBER_INPUT_DEVICES];
        result = (*audioIODeviceCapabilitiesItf)->GetAvailableAudioInputs(audioIODeviceCapabilitiesItf, &numInputs, inputDeviceIDs);
        SLASSERT(result);

        // find the specific device by name
        for (int i = 0; i < numInputs; i++)
        {
            SLAudioInputDescriptor audioInputDescriptor;

            result = (*audioIODeviceCapabilitiesItf)->QueryAudioInputCapabilities(audioIODeviceCapabilitiesItf, inputDeviceIDs[i], &audioInputDescriptor);
            SLASSERT(result);

            LogInfo("Input#%d deviceName '%s'", i, audioInputDescriptor.deviceName);

            if (audioInputDescriptor.deviceName &&
                *(audioInputDescriptor.deviceName) &&
                strcmp(deviceName.c_str(), (const char*)audioInputDescriptor.deviceName) == 0)
            {
                deviceID = inputDeviceIDs[i];
                foundDeviceName = true;
                break;
            }
        }

        // fail if we could not find the device
        // note: no fallback to default mic in this case!
        SLASSERT(foundDeviceName);
    }

    return deviceID;
}

AudioRecorder::AudioRecorder(std::string deviceName, SampleFormat *sampleFormat, SLObjectItf slEngineObject, SLEngineItf slEngine) :
    callback_(nullptr),
    audioBufferIndex_(0)
{
    SLresult result;
    sampleInfo_ = *sampleFormat;
    SLAndroidDataFormat_PCM_EX format_pcm;
    ConvertToSLSampleFormat(&format_pcm, &sampleInfo_);

    // configure audio source
    // default is the default input device
    SLuint32 deviceID = GetDeviceID(slEngineObject, deviceName);

    SLDataLocator_IODevice loc_dev = { SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT, deviceID, NULL };
    SLDataSource audioSrc = { &loc_dev, NULL };

    // configure audio sink
    SLDataLocator_AndroidSimpleBufferQueue loc_bq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, DEVICE_SHADOW_BUFFER_QUEUE_LEN };
    SLDataSink audioSnk = { &loc_bq, &format_pcm };

    // create audio recorder
    // (requires the RECORD_AUDIO permission)
    const SLInterfaceID id[] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
    const SLboolean req[]    = { SL_BOOLEAN_TRUE };
    result = (*slEngine)->CreateAudioRecorder(slEngine, &recObjectItf_, &audioSrc, &audioSnk, sizeof(id) / sizeof(id[0]), id, req);
    SLASSERT(result);

    result = (*recObjectItf_)->Realize(recObjectItf_, SL_BOOLEAN_FALSE);
    SLASSERT(result);

    result = (*recObjectItf_)->GetInterface(recObjectItf_, SL_IID_RECORD, &recItf_);
    SLASSERT(result);

    result = (*recObjectItf_)
        ->GetInterface(recObjectItf_, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
            &recBufQueueItf_);
    SLASSERT(result);

    result = (*recBufQueueItf_)
        ->RegisterCallback(recBufQueueItf_, bqRecorderCallback, this);
    SLASSERT(result);

    sampleInfo_.bufSize_ = sampleInfo_.framesPerBuf_ * sampleInfo_.channels_ * SL_PCMSAMPLEFORMAT_FIXED_16;

    audioBuffers_.reset(new std::unique_ptr<SLuint8[]>[DEVICE_SHADOW_BUFFER_QUEUE_LEN]);

    for (int i = 0; i < DEVICE_SHADOW_BUFFER_QUEUE_LEN; i++) {
        audioBuffers_[i].reset(new SLuint8[sampleInfo_.bufSize_]);
    }

#ifdef ENABLE_LOG
    std::string name = "rec";
    recLog_ = new AndroidLog(name);
#endif
}

SLboolean AudioRecorder::Start(void) {

    if (!audioBuffers_.get()) {
        LogInfo("%s audioBuffers_ is null, return false", __FUNCTION__);
        return SL_BOOLEAN_FALSE;
    }

    SLresult result;
    // in case already recording, stop recording and clear buffer queue
    result = (*recItf_)->SetRecordState(recItf_, SL_RECORDSTATE_STOPPED);
    SLASSERT(result);
    result = (*recBufQueueItf_)->Clear(recBufQueueItf_);
    SLASSERT(result);

    // Check is there already buffers in the queue, OpenSL Enqueue call will fail with
    // SL_RESULT_BUFFER_INSUFFICIENT error if the buffers are full when trying to enqueue more data
    int buffersInQueue = (int) GetBufferCount();
    for (int i = 0; i < DEVICE_SHADOW_BUFFER_QUEUE_LEN - buffersInQueue; i++) {
        if(!EnqueueAudioBuffer()) {
            LogInfo("%s No audiobuffers available for start, return false", __FUNCTION__);
            return SL_BOOLEAN_FALSE;
        }
    }

    result = (*recItf_)->SetRecordState(recItf_, SL_RECORDSTATE_RECORDING);
    SLASSERT(result);

    return (result == SL_RESULT_SUCCESS ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE);
}

SLboolean AudioRecorder::Stop(void) {
    // in case already recording, stop recording and clear buffer queue
    SLuint32 curState;

    SLresult result = (*recItf_)->GetRecordState(recItf_, &curState);
    SLASSERT(result);
    if (curState == SL_RECORDSTATE_STOPPED) {
        return SL_BOOLEAN_TRUE;
    }
    result = (*recItf_)->SetRecordState(recItf_, SL_RECORDSTATE_STOPPED);
    SLASSERT(result);
    result = (*recBufQueueItf_)->Clear(recBufQueueItf_);
    SLASSERT(result);

#ifdef ENABLE_LOG
    recLog_->flush();
#endif

    return SL_BOOLEAN_TRUE;
}

AudioRecorder::~AudioRecorder() {

    // destroy audio recorder object, and invalidate all associated interfaces
    if (recBufQueueItf_ != NULL) {
        (*recBufQueueItf_)->RegisterCallback(recBufQueueItf_, nullptr, nullptr);
        recBufQueueItf_ = NULL;
    }

    if (recObjectItf_ != NULL) {
        (*recObjectItf_)->Destroy(recObjectItf_);
    }

#ifdef ENABLE_LOG
    if (recLog_) {
        delete recLog_;
    }
#endif
}

void AudioRecorder::RegisterCallback(ENGINE_CALLBACK cb, void *ctx) {
    callback_ = cb;
    ctx_ = ctx;
}

SLAndroidSimpleBufferQueueState AudioRecorder::GetBufferQueueState() const {

    SLAndroidSimpleBufferQueueState state = {};

    if (recBufQueueItf_) {
        SLresult err = (*recBufQueueItf_)->GetState(recBufQueueItf_, &state);
        if (SL_RESULT_SUCCESS != err) {
            LogError("[%s] failed: %d", __FUNCTION__, err);
        }

    }
    return state;
}

SLuint32 AudioRecorder::GetBufferCount() const {
    SLAndroidSimpleBufferQueueState state = GetBufferQueueState();
    return state.count;
}

SLuint32 AudioRecorder::GetRecordState() const {
    SLuint32 state;
    SLresult err = (*recItf_)->GetRecordState(recItf_, &state);
    if (SL_RESULT_SUCCESS != err) {
        LogError("[%s] failed: %d", __FUNCTION__, err);
    }
    return state;
}
