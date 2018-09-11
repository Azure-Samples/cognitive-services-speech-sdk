//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "intent_recognition_event_args_private.h"
#import "intent_recognition_result_private.h"

#import "common_private.h"

@implementation IntentRecognitionEventArgs

- (instancetype)init:(const IntentImpl::IntentRecognitionEventArgs&)e
{
    self = [super init];
    _sessionId = [NSString stringWithString:e.SessionId];
    _result = [[IntentRecognitionResult alloc] init :e.GetResult()];

    return self;
}

@end

