//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_recognition_event_args_private_h
#define speech_recognition_event_args_private_h

#import "speech_recognition_event_args.h"
#import "common_private.h"

@interface SpeechRecognitionEventArgs (Private)

- (instancetype)init: (const SpeechImpl::SpeechRecognitionEventArgs&)e;

@end

@interface SpeechRecognitionCanceledEventArgs (Private)

- (instancetype)init: (const SpeechImpl::SpeechRecognitionCanceledEventArgs&)e;

@end

#endif /* speech_recognition_event_args_private_h */
