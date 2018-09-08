//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speechrecognitioneventargs_private_h
#define speechrecognitioneventargs_private_h

#import "speech_recognition_result_event_args.h"

#define BINDING_OBJECTIVE_C
#import "common_private.h"

@interface SpeechRecognitionResultEventArgs (Private)

// Todo: better opaque pointer in obj-c, or a typedef
- (instancetype)init: (const SpeechImpl::SpeechRecognitionEventArgs&)e;

@end

#endif /* speechrecognitioneventargs_private_h */
