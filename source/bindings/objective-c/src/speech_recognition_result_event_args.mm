//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "NSString_STL.h"
#import "speech_recognition_result_event_args.h"
#import "speech_recognition_result_event_args_private.h"
#import "speech_recognition_result_private.h"
#import "speechapi_cxx.h"

@implementation SpeechRecognitionResultEventArgs
{
    Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs* eventArgsImpl;
}

- (instancetype)init:(void *)eventArgsHandle
{
    self = [super init];
    eventArgsImpl = static_cast<Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs *>(eventArgsHandle);
    
    _sessionId = [NSString stringWithWString:eventArgsImpl->SessionId];
    
    // Todo: error handling
    std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult> resultImpl = eventArgsImpl->GetResult();
    _result = [[SpeechRecognitionResult alloc] init: (void *)&resultImpl];
    
    return self;
}

@end

