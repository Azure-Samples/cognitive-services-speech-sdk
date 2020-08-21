//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

@implementation SPXAudioDataStream
{
    std::shared_ptr<SpeechImpl::AudioDataStream> audioImpl;
}

- (instancetype)initFromSynthesisResult:(SPXSpeechSynthesisResult *) result
{
    try {
        self = [super init]; 
        audioImpl = SpeechImpl::AudioDataStream::FromResult([result getHandle]);
        if (audioImpl == nullptr) {
            NSLog(@"Unable to get valid AudioDataStream handle");
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

- (nullable instancetype)initFromSynthesisResult:(nonnull SPXSpeechSynthesisResult *)result error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initFromSynthesisResult:result];
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

- (instancetype)initFromKeywordRecognitionResult:(SPXKeywordRecognitionResult *) result
{
    try {
        self = [super init]; 
        audioImpl = SpeechImpl::AudioDataStream::FromResult([result getHandle]);
        if (audioImpl == nullptr) {
            NSLog(@"Unable to get valid AudioDataStream handle");
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

- (nullable instancetype)initFromKeywordRecognitionResult:(nonnull SPXKeywordRecognitionResult *)result error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initFromKeywordRecognitionResult:result];
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

- (SPXStreamStatus)getStatus
{
    return (SPXStreamStatus)audioImpl->GetStatus();
}

- (BOOL)canReadData:(NSUInteger) bytesRequested
{
    return audioImpl->CanReadData((uint32_t)bytesRequested);
}

- (BOOL)canReadDataFromPosition:(NSUInteger) pos bytesRequested:(NSUInteger)bytesRequested
{
    return audioImpl->CanReadData((uint32_t)pos, (uint32_t)bytesRequested);
}

- (NSUInteger)readData:(NSMutableData *)data length:(NSUInteger)length
{
    uint8_t* buffer = new uint8_t[length];
    NSUInteger ret = (NSUInteger)audioImpl->ReadData(buffer, (uint32_t)length);
    [data replaceBytesInRange:NSMakeRange(0, [data length]) withBytes:buffer length:ret];
    return ret;
}

- (NSUInteger)readDataFromPosition:(NSUInteger)pos data:(NSMutableData *)data length:(NSUInteger)length
{
    uint8_t* buffer = new uint8_t[length];
    NSUInteger ret = (NSUInteger)audioImpl->ReadData((uint32_t)pos, buffer, (uint32_t)length);
    [data replaceBytesInRange:NSMakeRange(0, [data length]) withBytes:buffer length:ret];
    return ret;
}

- (NSUInteger)getPosition
{
    return (NSUInteger)audioImpl->GetPosition();
}

- (void)setPosition:(NSUInteger) pos
{
    return audioImpl->SetPosition((uint32_t)pos);
}

- (void)saveToWavFile:(NSString *)fileName
{
    return audioImpl->SaveToWavFileAsync([fileName toSpxString]).get();
}

- (void)detachInput
{
    return audioImpl->DetachInput();
}

- (void)detachInput:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self detachInput];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
}

@end