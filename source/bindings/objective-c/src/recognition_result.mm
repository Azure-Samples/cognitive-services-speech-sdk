//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "speechapi_private.h"

@implementation SPXRecognitionResult
{
    std::shared_ptr<SpeechImpl::RecognitionResult> resultImpl;
}

- (instancetype)init:(std::shared_ptr<SpeechImpl::RecognitionResult>)resultHandle
{
    self = [super init];
    resultImpl = resultHandle;

    _reason = [Util fromResultReasonImpl:resultHandle->Reason];
    _resultId = [NSString stringWithString:resultHandle->ResultId];
    _text = [NSString stringWithString:resultHandle->Text];
    _duration = resultHandle->Duration();
    _offset = resultHandle->Offset();
    _properties = [[RecognitionResultPropertyCollection alloc] initFrom:resultHandle];
    
    return self;
}

- (instancetype)initWithError: (NSString *)message
{
    _reason = SPXResultReason_Canceled;
    return self;
}

- (std::shared_ptr<SpeechImpl::RecognitionResult>)getHandle
{
    return resultImpl;
}

@end

@implementation SPXCancellationDetails

-(instancetype)initFromCanceledRecognitionResult:(SPXRecognitionResult *)recognitionResult
{
    try {
        auto cancellationDetailsImpl = SpeechImpl::CancellationDetails::FromResult([recognitionResult getHandle]);
        return [self initWithImpl: cancellationDetailsImpl];
    } catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating SPXCancellationDetails in core. Check to make sure that recognition result has the reason Canceled.");
    }
    return nil;
}

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::CancellationDetails>)handle
{
    self = [super init];
    _reason = [Util fromCancellationReasonImpl:handle->Reason];
    _errorDetails = [NSString stringWithString:handle->ErrorDetails];
    return self;
}

@end

@implementation SPXNoMatchDetails

-(instancetype)initFromNoMatchRecognitionResult:(SPXRecognitionResult *)recognitionResult
{
    try {
        auto noMatchDetailsImpl = SpeechImpl::NoMatchDetails::FromResult([recognitionResult getHandle]);
        return [self initWithImpl: noMatchDetailsImpl];
    } catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating SPXNoMatchDetails in core. Check to make sure that recognition result has the reason NoMatch.");
    }
    return nil;
}

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::NoMatchDetails>)handle
{
    self = [super init];
    _reason = [Util fromNoMatchReasonImpl:handle->Reason];
    return self;
}

@end
