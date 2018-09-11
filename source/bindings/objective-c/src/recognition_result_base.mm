//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "recognition_result_base_private.h"

@implementation RecognitionResultBase

- (instancetype)init :(std::shared_ptr<SpeechImpl::RecognitionResult>)resultHandle
{
    self = [super init];
    _resultId = [NSString stringWithString:resultHandle->ResultId];
    _text = [NSString stringWithString:resultHandle->Text];
    _errorDetails = [NSString stringWithString:resultHandle->ErrorDetails];
    _duration = resultHandle->Duration();
    _offset = resultHandle->Offset();
    
    switch (resultHandle->Reason)
    {
        case SpeechImpl::Reason::Recognized:
            _reason = RecognitionStatus::Recognized;
            break;
        case SpeechImpl::Reason::IntermediateResult:
            _reason = RecognitionStatus::IntermediateResult;
            break;
        case SpeechImpl::Reason::NoMatch:
            _reason = RecognitionStatus::NoMatch;
            break;
        case SpeechImpl::Reason::InitialSilenceTimeout:
            _reason = RecognitionStatus::InitialSilenceTimeout;
            break;
        case SpeechImpl::Reason::InitialBabbleTimeout:
            _reason = RecognitionStatus::InitialBabbleTimeout;
            break;
        case SpeechImpl::Reason::Canceled:
            _reason = RecognitionStatus::Canceled;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown recognition status");
            self = nil;
            break;
    }
    return self;
}

- (instancetype)initWithError: (NSString *)message
{
    _reason = RecognitionStatus::Canceled;
    _errorDetails = message;
    return self;
}

@end
