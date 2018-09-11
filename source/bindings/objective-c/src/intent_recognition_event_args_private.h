//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef intent_recognition_event_args_private_h
#define intent_recognition_event_args_private_h

#import "intent_recognition_event_args.h"
#import "common_private.h"

@interface IntentRecognitionEventArgs (Private)

- (instancetype)init: (const IntentImpl::IntentRecognitionEventArgs&)e;

@end

#endif /* intent_recognition_event_args_private_h */
