//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speech_recognition_result_event_args_private.h"
#import "speech_recognition_result_private.h"

#import "common_private.h"

@implementation SpeechRecognitionResultEventArgs

- (instancetype)init:(const SpeechImpl::SpeechRecognitionEventArgs&)e
{
    self = [super init];
    
    _sessionId = [NSString stringWithString:e.SessionId];
    _result = [[SpeechRecognitionResult alloc] init :e.GetResult()];
    
    return self;
}

@end

