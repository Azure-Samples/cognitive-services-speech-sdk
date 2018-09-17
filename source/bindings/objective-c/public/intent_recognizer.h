//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef intent_recognizer_h
#define intent_recognizer_h

#import <Foundation/Foundation.h>
#import "speech_config.h"
#import "audio_config.h"
#import "property_collection.h"
#import "recognizer.h"
#import "language_understanding_model.h"
#import "intent_recognition_result.h"
#import "intent_recognition_event_args.h"

/**
  * Performs intent recognition on the speech input. It returns both recognized text and recognized intent.
  */
@interface IntentRecognizer : Recognizer

typedef void (^IntentRecognitionEventHandlerBlock)(IntentRecognizer *, IntentRecognitionEventArgs *);
typedef void (^IntentRecognitionCanceledEventHandlerBlock)(IntentRecognizer *, IntentRecognitionCanceledEventArgs *);

/**
  * Creates a new instance of intent recognizer.
  * @param speechConfig intent recognition config.
  * @return a new instance of an intent recognizer.
  */
+ (IntentRecognizer *)fromConfig:(SpeechConfig *)speechConfig;

/**
  * Creates a new instance of an intent recognizer using the specified audio config.
  * @param speechConfig intent recognition config.
  * @param audioConfig audio configuration.
  * @return a new instance of an intent recognizer.
  */
+ (IntentRecognizer *)fromConfig:(SpeechConfig *)speechConfig usingAudio:(AudioConfig *)audioConfig;

/**
  * Sets the authorization token used to communicate with the service.
  * @param token Authorization token.
  */
- (void)setAuthorizationToken:(NSString *)token;

/**
  * Gets the authorization token used to communicate with the service.
  * @return Authorization token.
  */
- (NSString *)getAuthorizationToken;

/**
  * Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
  * @param simplePhrase The phrase corresponding to the intent.
  */
- (void)addIntentFromPhrase:(NSString *)simplePhrase;

/**
  * Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
  * @param simplePhrase The phrase corresponding to the intent.
  * @param intentId A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
  */
- (void)addIntentFromPhrase:(NSString *)simplePhrase mappingToId:(NSString *)intentId;

/**
  * Adds a single intent by name from the specified Language Understanding Model.
  * @param intentName The name of the single intent to be included from the language understanding model.
  * @param model The language understanding model containing the intent.
  */
- (void)addIntent: (NSString *)intentName fromModel:(LanguageUnderstandingModel *)model;

/**
  * Adds a single intent by name from the specified Language Understanding Model.
  * @param intentName The name of the single intent to be included from the language understanding model.
  * @param model The language understanding model containing the intent.
  * @param intentId A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
  */
- (void)addIntent: (NSString *)intentName fromModel:(LanguageUnderstandingModel *)model mappingToId:(NSString *)intentId;

/**
  * Adds all intents from the specified Language Understanding Model.
  * @param model The language understanding model containing the intents.
  * @param intentId A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
  */
- (void)addAllIntentsFromModel:(LanguageUnderstandingModel *)model mappingToId:(NSString *)intentId;

/**
  * Starts intent recognition, and stops after the first utterance is recognized. It returns the recognition text and intent as result.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  * @return the result of intent recognition.
  */
- (IntentRecognitionResult *)recognizeOnce;

/**
  * Starts intent recognition, and the block function is called when the first utterance has been recognized.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  */
- (void)recognizeOnceAsync:(void (^)(IntentRecognitionResult *))resultReceivedBlock;

/**
  * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
  * User must subscribe to events to receive recognition results.
  */
- (void)startContinuousRecognition;

/**
  * Stops continuous intent recognition.
  */
- (void)stopContinuousRecognition;

/**
  * Subscribes to Recognized event which indicates a final result has been recognized.
  */
- (void)addRecognizedEventListener:(IntentRecognitionEventHandlerBlock)eventHandler;

/**
  * Subscribes to Recognizing event which indicates an intermediate result has been recognized.
  */
- (void)addRecognizingEventListener:(IntentRecognitionEventHandlerBlock)eventHandler;

/**
  * Subscribes to Canceled event which indicates an error occurred during recognition.
  */
- (void)addCanceledEventListener:(IntentRecognitionCanceledEventHandlerBlock)eventHandler;

@end

#endif /* intent_recognizer_h */
