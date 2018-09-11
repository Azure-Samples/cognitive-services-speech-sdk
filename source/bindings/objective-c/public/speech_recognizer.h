//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_recognizer_h
#define speech_recognizer_h

#import <Foundation/Foundation.h>
#import "recognizer.h"
#import "speech_recognition_result.h"
#import "speech_recognition_event_args.h"

@interface SpeechRecognizer : Recognizer

typedef void (^SpeechRecognitionEventHandlerBlock)(SpeechRecognizer *, SpeechRecognitionEventArgs *);

- (void)close;

- (SpeechRecognitionResult *)recognize;
- (void)recognizeAsync:(void (^)(SpeechRecognitionResult *))resultReceivedBlock;

// Todo: add protocol interface for registering callback.

// Todo: add block interface
- (void)startContinuousRecognition;
- (void)stopContinuousRecognition;

// - (void) setDeploymentId: (NSString *)deploymentId;

// How to add events, as a protocol, and individual blocks?
- (void)addFinalResultEventListener:(SpeechRecognitionEventHandlerBlock)eventHandler;
- (void)addIntermediateResultEventListener:(SpeechRecognitionEventHandlerBlock)eventHandler;

@end

#endif /* speech_recognizer_h */
