//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognizer_private_h
#define recognizer_private_h

#import <Foundation/Foundation.h>
#import "recognizer.h"

@interface Recognizer (Private)

- (instancetype)initFrom:(RecognizerSharedPtr)recoHandle withParameters:(SpeechImpl::PropertyCollection *)propertiesHandle;

- (void)setDispatchQueue: (dispatch_queue_t)queue;

- (void)onSessionStartedEvent: (SessionEventArgs *)eventArgs;
- (void)onSessionStoppedEvent: (SessionEventArgs *)eventArgs;
- (void)onSpeechStartDetectedEvent: (RecognitionEventArgs *)eventArgs;
- (void)onSpeechEndDetectedEvent: (RecognitionEventArgs *)eventArgs;

@end

#endif /* recognizer_private_h */
