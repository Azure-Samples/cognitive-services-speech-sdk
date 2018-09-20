//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "recognition_event_args_private.h"
#import "session_event_args_private.h"

@implementation SPXRecognitionEventArgs

- (instancetype)init: (const SpeechImpl::RecognitionEventArgs&)eventArgsHandle
{
    self = [super init: eventArgsHandle];
    _offset = eventArgsHandle.Offset;
    return self;
}


@end

