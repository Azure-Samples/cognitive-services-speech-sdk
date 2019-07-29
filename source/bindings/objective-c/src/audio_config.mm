//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

@implementation SPXAudioConfiguration
{
    std::shared_ptr<AudioImpl::AudioConfig> audioImpl;
}

- (instancetype)init
{
    self = [super init];
    self->audioImpl = AudioImpl::AudioConfig::FromDefaultMicrophoneInput();
    if (self->audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core using default microphone.");
        return nil;
    }
    return self;
}

- (instancetype)initWithMicrophone:(NSString *)deviceName
{
    self = [super init];
    if (nil == deviceName) {
        return [self init];
    }

    self->audioImpl = AudioImpl::AudioConfig::FromMicrophoneInput([deviceName toSpxString]);
    if (self->audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core using specified microphone.");
        return nil;
    }
    return self;
}

- (instancetype)initWithWavFileInput:(NSString *)path
{
    self = [super init];
    self->audioImpl = AudioImpl::AudioConfig::FromWavFileInput([path toSpxString]);
    if (self->audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core using wav file input.");
        return nil;
    }
    return self;
}

- (instancetype)initWithStreamInput:(SPXAudioInputStream *)stream
{
    self = [super init];
    self->audioImpl = AudioImpl::AudioConfig::FromStreamInput([stream getHandle]);
    if (self->audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core using stream input.");
        return nil;
    }
    return self;
}

// speaker output is not supported in MacOS/iOS now, not exposed.
- (instancetype)initWithDefaultSpeakerOutput
{
    self = [super init];
    audioImpl = AudioImpl::AudioConfig::FromDefaultSpeakerOutput();
    if (audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core using default speaker.");
        return nil;
    }
    return self;
}

- (instancetype)initWithWavFileOutput:(NSString *)path
{
    try {
        self = [super init];
        audioImpl = AudioImpl::AudioConfig::FromWavFileOutput([path toSpxString]);
        if (audioImpl == nullptr) {
            NSLog(@"Unable to create audio config in core using wav file output.");
            return nil;
        }
        return self;
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (...) {
        NSLog(@"%@: Exception caught.", NSStringFromSelector(_cmd));
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (instancetype)initWithWavFileOutput:(NSString *)path error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initWithWavFileOutput:path];
        return self;
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (instancetype)initWithStreamOutput:(SPXAudioOutputStream *)stream;
{
    try {
        self = [super init];
        audioImpl = AudioImpl::AudioConfig::FromStreamOutput([stream getHandle]);
        if (audioImpl == nullptr) {
            NSLog(@"Unable to create audio config in core using stream output.");
            return nil;
        }
        return self;
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (...) {
        NSLog(@"%@: Exception caught.", NSStringFromSelector(_cmd));
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (instancetype)initWithStreamOutput:(SPXAudioOutputStream *)stream error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initWithStreamOutput:stream];
        return self;
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (std::shared_ptr<AudioImpl::AudioConfig>)getHandle
{
    return audioImpl;
}

@end
