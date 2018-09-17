//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "speechapi_private.h"

@implementation RecognitionResult
{
    std::shared_ptr<SpeechImpl::RecognitionResult> resultImpl;
}

- (instancetype)init :(std::shared_ptr<SpeechImpl::RecognitionResult>)resultHandle
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
    _reason = ResultReason::Canceled;
    return self;
}

- (std::shared_ptr<SpeechImpl::RecognitionResult>)getHandle
{
    return resultImpl;
}

@end

@implementation CancellationDetails

- (instancetype)init :(std::shared_ptr<SpeechImpl::CancellationDetails>)handle
{
    self = [super init];
    _reason = [Util fromCancellationReasonImpl:handle->Reason];
    _errorDetails = [NSString stringWithString:handle->ErrorDetails];
    return self;
}

+(CancellationDetails *) fromResult:(RecognitionResult *)recognitionResult
{
    return [[CancellationDetails alloc] init:SpeechImpl::CancellationDetails::FromResult([recognitionResult getHandle])];
}

@end

@implementation NoMatchDetails

- (instancetype)init :(std::shared_ptr<SpeechImpl::NoMatchDetails>)handle
{
    self = [super init];
    _reason = [Util fromNoMatchReasonImpl:handle->Reason];
    return self;
}

+(NoMatchDetails *) fromResult:(RecognitionResult *)recognitionResult
{
    return [[NoMatchDetails alloc] init:SpeechImpl::NoMatchDetails::FromResult([recognitionResult getHandle])];
}

@end
