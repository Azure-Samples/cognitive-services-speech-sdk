//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speech_recognition_result_private.h"

#import "common_private.h"

@implementation SpeechRecognitionResult
{
    std::shared_ptr<SpeechImpl::SpeechRecognitionResult> resultImpl;
}

- (instancetype)init :(std::shared_ptr<SpeechImpl::SpeechRecognitionResult>)resultHandle
{
    self = [super init];
    resultImpl = resultHandle;
    
    _resultId = [NSString stringWithString:resultImpl->ResultId];
    _text = [NSString stringWithString:resultImpl->Text];
    _recognitionFailureReason = [NSString stringWithString:resultImpl->ErrorDetails];
    _duration = resultImpl->Duration();
    _offset = resultImpl->Offset();
    
    switch (resultImpl->Reason)
    {
        case SpeechImpl::Reason::Recognized:
            _recognitionStatus = RecognitionStatus::Recognized;
            break;
        case SpeechImpl::Reason::IntermediateResult:
            _recognitionStatus = RecognitionStatus::IntermediateResult;
            break;
        case SpeechImpl::Reason::NoMatch:
            _recognitionStatus = RecognitionStatus::NoMatch;
            break;
        case SpeechImpl::Reason::InitialSilenceTimeout:
            _recognitionStatus = RecognitionStatus::InitialSilenceTimeout;
            break;
        case SpeechImpl::Reason::InitialBabbleTimeout:
            _recognitionStatus = RecognitionStatus::InitialBabbleTimeout;
            break;
        case SpeechImpl::Reason::Canceled:
            _recognitionStatus = RecognitionStatus::Canceled;
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
    _recognitionStatus = RecognitionStatus::Canceled;
    _recognitionFailureReason = message;
    return self;    
}

- (void)dealloc
{
    if (resultImpl != nullptr) {
        resultImpl.reset();
    }
}

@end
