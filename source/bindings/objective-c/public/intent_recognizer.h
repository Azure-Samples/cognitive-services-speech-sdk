//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef intent_recognizer_h
#define intent_recognizer_h

#import <Foundation/Foundation.h>
#import "recognizer.h"
#import "language_understanding_model.h"
#import "intent_recognition_result.h"
#import "intent_recognition_event_args.h"

@interface IntentRecognizer : Recognizer

typedef void (^IntentRecognitionEventHandlerBlock)(IntentRecognizer *, IntentRecognitionEventArgs *);

- (void)close;

- (IntentRecognitionResult *)recognize;
- (void)recognizeAsync:(void (^)(IntentRecognitionResult *))resultReceivedBlock;

// Todo: add protocol interface for registering callback.

// Todo: add block interface
- (void)startContinuousRecognition;
- (void)stopContinuousRecognition;

- (void)AddIntentUsingId: (NSString *)intentId ForPhrase: (NSString *)simplePhrase;
- (void)AddIntentUsingId: (NSString *)intentId FromModel: (LanguageUnderstandingModel *)model;
- (void)AddIntentUsingId: (NSString *)intentId FromModel: (LanguageUnderstandingModel *)model WithIntentName: (NSString *)intentName;

// - (void) setDeploymentId: (NSString *)deploymentId;

// How to add events, as a protocol, and individual blocks?
- (void)addFinalResultEventListener:(IntentRecognitionEventHandlerBlock)eventHandler;
- (void)addIntermediateResultEventListener:(IntentRecognitionEventHandlerBlock)eventHandler;

@end

#endif /* intent_recognizer_h */
