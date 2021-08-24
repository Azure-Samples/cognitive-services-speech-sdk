//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "AudioRecorder.h"
#import <AVFoundation/AVFoundation.h>

static const int kNumberBuffers = 3;

@interface AudioRecorder () {
    AudioQueueRef               queueRef;
    AudioQueueBufferRef         buffers[kNumberBuffers];
    SPXPushAudioInputStream     *pushStream;
}

@property (nonatomic, assign) SInt64 currPacket;

@end

@implementation AudioRecorder

- (instancetype)initWithPushStream:(SPXPushAudioInputStream *)stream
{
    if (self = [super init]) {
        AudioStreamBasicDescription recordFormat = {0};
        recordFormat.mFormatID = kAudioFormatLinearPCM;
        recordFormat.mSampleRate = 16000;
        recordFormat.mChannelsPerFrame = 1;
        recordFormat.mBitsPerChannel = 16;
        recordFormat.mFramesPerPacket = 1;
        recordFormat.mBytesPerFrame = recordFormat.mBytesPerPacket = (recordFormat.mBitsPerChannel / 8) * recordFormat.mChannelsPerFrame;
        recordFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;

        self->pushStream = stream;
        self.currPacket = 0;

        OSStatus status = AudioQueueNewInput(&recordFormat,
                                             recorderCallBack,
                                             (__bridge void *)self,
                                             NULL,
                                             kCFRunLoopCommonModes,
                                             0,
                                             &queueRef);
        if (status != noErr) {
            NSLog(@"new input error");
        }

        for (int i = 0; i < kNumberBuffers; i++) {
            AudioQueueAllocateBuffer(queueRef, 3200, &buffers[i]);
            AudioQueueEnqueueBuffer(queueRef, buffers[i], 0, NULL);
        }

        if (status != noErr) {
            NSLog(@"create recorder file failure");
        }
    }
    return self;
}

- (void)dealloc {
    AudioQueueDispose(queueRef, true);
}

static void recorderCallBack(void *aqData,
                             AudioQueueRef inAQ,
                             AudioQueueBufferRef inBuffer,
                             const AudioTimeStamp *timestamp,
                             UInt32 inNumPackets,
                             const AudioStreamPacketDescription *inPacketDesc) {
    AudioRecorder *recorder = (__bridge AudioRecorder *)aqData;

    NSData* data = [[NSData alloc] initWithBytesNoCopy:(void*)inBuffer->mAudioData
                                                length:(NSUInteger)inBuffer->mAudioDataByteSize
                                          freeWhenDone:false];
    [recorder->pushStream write:data];

    if (recorder.isRunning) {
        AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
    }
}

- (void)record {
    if (self.isRunning) {
        return;
    }

    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
    [[AVAudioSession sharedInstance] setActive:true error:nil];

    OSStatus status = AudioQueueStart(queueRef, NULL);
    if (status != noErr) {
        NSLog(@"start queue failure");
        return;
    }
    _isRunning = true;
}

- (void)stop
{
    if (self.isRunning) {
        AudioQueueStop(queueRef, true);
        _isRunning = false;

        [[AVAudioSession sharedInstance] setActive:false
                                       withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation
                                             error:nil];
    }
}

@end
