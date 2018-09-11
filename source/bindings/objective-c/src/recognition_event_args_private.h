//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_event_args_private_h
#define recognition_event_args_private_h

#import "recognition_event_args.h"
#import "common_private.h"

@interface RecognitionEventArgs (Private)

- (instancetype)init: (RecognitionEventType)eventType :(const SpeechImpl::RecognitionEventArgs&) eventArgsHandle;

@end

#endif /* recognition_event_args_private_h */
