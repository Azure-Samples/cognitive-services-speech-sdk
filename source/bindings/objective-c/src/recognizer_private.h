//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognizer_private_h
#define recognizer_private_h

#import <Foundation/Foundation.h>
#import "recognizer.h"
#import "recognition_error_event_args.h"

@interface Recognizer (Private)

- (void)setDispatchQueue: (dispatch_queue_t)queue;

- (void)onSessionEvent: (SessionEventArgs *)eventArgs;
- (void)onRecognitionEvent: (RecognitionEventArgs *)eventArgs;
- (void)onErrorEvent: (RecognitionErrorEventArgs *)eventArgs;

@end

#endif /* recognizer_private_h */
