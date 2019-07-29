//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "speechapi_private.h"
#import "exception.h"

@implementation SPXSpeechSynthesisCancellationDetails
{
    std::shared_ptr<SpeechImpl::SpeechSynthesisCancellationDetails> cancellationImpl;
}

-(instancetype)initFromCanceledSynthesisResult:(SPXSpeechSynthesisResult *)synthesisResult
{
    std::shared_ptr<SpeechImpl::SpeechSynthesisResult> resultHandle = [synthesisResult getHandle];
    if (resultHandle == nullptr)
    {
        self = [super init];
        _reason = SPXCancellationReason_Error;
        _errorCode = SPXCancellationErrorCode_RuntimeError;
        _errorDetails = @"The result handle is null.";
        return self;
    }
    else
    {
        try {
            auto cancellationDetailsImpl = SpeechImpl::SpeechSynthesisCancellationDetails::FromResult(resultHandle);
            return [self initWithImpl: cancellationDetailsImpl];
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
        catch (...)
        {
            NSLog(@"Exception caught when creating SPXCancellationDetails in core. Check to make sure that synthesis result has the reason Canceled.");
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                            reason:@"Runtime Exception"
                                                        userInfo:nil];
            [exception raise];
        }
        return nil;
    }
}

-(nullable instancetype)initFromCanceledSynthesisResult:(nonnull SPXSpeechSynthesisResult *)synthesisResult error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initFromCanceledSynthesisResult:synthesisResult];
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

-(instancetype)initFromCanceledAudioDataStream:(SPXAudioDataStream *)stream
{
    std::shared_ptr<SpeechImpl::AudioDataStream> streamHandle = [stream getHandle];
    if (streamHandle == nullptr)
    {
        self = [super init];
        _reason = SPXCancellationReason_Error;
        _errorCode = SPXCancellationErrorCode_RuntimeError;
        _errorDetails = @"The audio data stream handle is null.";
        return self;
    }
    else
    {
        try {
            auto cancellationDetailsImpl = SpeechImpl::SpeechSynthesisCancellationDetails::FromStream(streamHandle);
            return [self initWithImpl: cancellationDetailsImpl];
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
        catch (...)
        {
            NSLog(@"Exception caught when creating SPXCancellationDetails in core. Check to make sure that synthesis result has the reason Canceled.");
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                            reason:@"Runtime Exception"
                                                        userInfo:nil];
            [exception raise];
        }
        return nil;
    }
}

-(nullable instancetype)initFromCanceledAudioDataStream:(nonnull SPXAudioDataStream *)stream error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initFromCanceledAudioDataStream:stream];
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

- (nullable instancetype)initWithAudioDataStream:(nonnull SPXAudioDataStream *)stream
{
    self = [super init];
    cancellationImpl = SpeechImpl::SpeechSynthesisCancellationDetails::FromStream([stream getHandle]);
    if (cancellationImpl == nullptr) {
        NSLog(@"Unable to create speech synthesis cancellation details from stream in core");
        return nil;
    }
    return self;
}

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::SpeechSynthesisCancellationDetails>)handle
{
    self = [super init];
    if (handle == nullptr) {
        return nil;
    }
    else
    {
        _reason = [Util fromCancellationReasonImpl:handle->Reason];
        _errorCode = [Util fromCancellationErrorCodeImpl:handle->ErrorCode];
        _errorDetails = [NSString StringWithStdString:handle->ErrorDetails];
    }
    return self;
}

@end
