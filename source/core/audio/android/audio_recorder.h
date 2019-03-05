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

#ifndef NATIVE_AUDIO_AUDIO_RECORDER_H
#define NATIVE_AUDIO_AUDIO_RECORDER_H
#include <sys/types.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "audio_common.h"
#include "debug_utils.h"

class AudioRecorder {
    SLObjectItf recObjectItf_;
    SLRecordItf recItf_;
    SLAndroidSimpleBufferQueueItf recBufQueueItf_;

    SampleFormat sampleInfo_;

    ENGINE_CALLBACK callback_;
    void *ctx_;

    std::unique_ptr<std::unique_ptr<SLuint8[]>[]> audioBuffers_;
    int audioBufferIndex_;

public:
    explicit AudioRecorder(std::string deviceName, SampleFormat *format, SLObjectItf slEngineObject, SLEngineItf engineEngine);
    ~AudioRecorder();
    SLboolean Start(void);
    SLboolean Stop(void);
    void ProcessSLCallback(SLAndroidSimpleBufferQueueItf bq);
    void RegisterCallback(ENGINE_CALLBACK cb, void *ctx);
    SLAndroidSimpleBufferQueueState GetBufferQueueState() const;
    SLuint32 GetBufferCount() const;
    void ReadAudioBuffer();
    bool EnqueueAudioBuffer();
    SLuint32 GetRecordState() const;

private:
    SLuint32 GetDeviceID(SLObjectItf slEngineObject, const std::string& deviceName);


#ifdef ENABLE_LOG
    AndroidLog *recLog_;
#endif
};

#endif  // NATIVE_AUDIO_AUDIO_RECORDER_H
