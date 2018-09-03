//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef SpeechRecognizer_h
#define SpeechRecognizer_h
#import "speech_recognition_result.h"
#import "speech_recognition_result_event_args.h"

@interface SpeechRecognizer : NSObject

typedef void (^RecognitionResultEventHandlerBlock)(SpeechRecognizer *, SpeechRecognitionResultEventArgs *);

- (void)close;

- (SpeechRecognitionResult *)recognize;
- (void)recognizeAsync:(void (^)(SpeechRecognitionResult *))resultReceivedBlock;

// Todo: add protocol interface for registering callback.

// Todo: add block interface
- (void)startContinuousRecognition;
- (void)stopContinuousRecognition;

// - (void) setDeploymentId: (NSString *)deploymentId;

// How to add events, as a protocol, and individual blocks?
- (void)addFinalResultEventListener:(RecognitionResultEventHandlerBlock)eventHandler;
- (void)removeFinalResultEventListener:(RecognitionResultEventHandlerBlock)eventHandler;

- (void)addIntermediateResultEventListener:(RecognitionResultEventHandlerBlock)eventHandler;
- (void)removeIntermediateResultEventListener:(RecognitionResultEventHandlerBlock)eventHandler;
// - connectSessionStarted((void) (^HandlingSessionStarted) : (SessionEventArgs *)sessionEvent);

// We might need a close/dispose method to ensure the native resource can be released quickly


@end

#endif /* SpeechRecognizer_h */
