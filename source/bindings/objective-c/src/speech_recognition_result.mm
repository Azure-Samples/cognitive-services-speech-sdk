//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "NSString_STL.h"
#import "speech_recognition_result.h"
#import "speech_recognition_result_private.h"
#import "speechapi_cxx.h"

@implementation SpeechRecognitionResult
{
    std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult> resultImpl;
}

- (instancetype)init:(void *)resultHandle
{
    self = [super init];
    resultImpl = *static_cast<std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult> *>(resultHandle);
    
    _resultId = [NSString stringWithWString:resultImpl->ResultId];
    _text = [NSString stringWithWString:resultImpl->Text];
    _recognitionFailureReason = [NSString stringWithWString:resultImpl->ErrorDetails];
    
    switch (resultImpl->Reason)
    {
        case Microsoft::CognitiveServices::Speech::Reason::Recognized:
            _recognitionStatus = RecognitionStatus::Recognized;
            break;
        case Microsoft::CognitiveServices::Speech::Reason::IntermediateResult:
            _recognitionStatus = RecognitionStatus::IntermediateResult;
            break;
        case Microsoft::CognitiveServices::Speech::Reason::NoMatch:
            _recognitionStatus = RecognitionStatus::NoMatch;
            break;
        case Microsoft::CognitiveServices::Speech::Reason::InitialSilenceTimeout:
            _recognitionStatus = RecognitionStatus::InitialSilenceTimeout;
            break;
        case Microsoft::CognitiveServices::Speech::Reason::InitialBabbleTimeout:
            _recognitionStatus = RecognitionStatus::InitialBabbleTimeout;
            break;
        case Microsoft::CognitiveServices::Speech::Reason::Canceled:
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

- (instancetype)initWithRuntimeError: (NSString *)message
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
