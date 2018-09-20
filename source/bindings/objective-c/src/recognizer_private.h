//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "SPXRecognizer.h"

@interface SPXRecognizer (Private)

- (instancetype)initFrom:(RecognizerSharedPtr)recoHandle withParameters:(SpeechImpl::PropertyCollection *)propertiesHandle;

- (void)setDispatchQueue: (dispatch_queue_t)queue;

- (void)onSessionStartedEvent: (SPXSessionEventArgs *)eventArgs;
- (void)onSessionStoppedEvent: (SPXSessionEventArgs *)eventArgs;
- (void)onSpeechStartDetectedEvent: (SPXRecognitionEventArgs *)eventArgs;
- (void)onSpeechEndDetectedEvent: (SPXRecognitionEventArgs *)eventArgs;

@end
