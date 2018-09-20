//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import <Foundation/Foundation.h>
#import "SPXSpeechConfiguration.h"
#import "SPXAudioConfiguration.h"
#import "SPXPropertyCollection.h"
#import "SPXRecognizer.h"
#import "SPXLanguageUnderstandingModel.h"
#import "SPXIntentRecognitionResult.h"
#import "SPXIntentRecognitionEventArgs.h"

/**
  * Performs intent recognition on the speech input. It returns both recognized text and recognized intent.
  */
@interface SPXIntentRecognizer : SPXRecognizer

typedef void (^SPXIntentRecognitionEventHandler)(SPXIntentRecognizer * _Nonnull, SPXIntentRecognitionEventArgs * _Nonnull);
typedef void (^SPXIntentRecognitionCanceledEventHandler)(SPXIntentRecognizer * _Nonnull, SPXIntentRecognitionCanceledEventArgs * _Nonnull);

/**
  * The authorization token used to communicate with the intent recognition service.
  */
@property (nonatomic, copy, nullable)NSString *authorizationToken;

/**
  * Initializes a new instance of intent recognizer using specified speech configuration.
  * @param speechConfiguration recognition configuration
  * @return an intent recognizer.
  */
- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration;

/**
  * Initializes a new instance of an intent recognizer using specified speech configuration and audio configuration.
  * @param speechConfiguration recognition configuration
  * @param audioConfiguration audio configuration.
  * @return an intent recognizer.
  */
- (nullable instancetype)initWithSpeechConfiguration:(nonnull SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration;

/**
  * Adds a simple phrase that may be spoken by the user, indicating a specific user intent.
  * @param simplePhrase The phrase corresponding to the intent.
  */
- (void)addIntentFromPhrase:(nonnull NSString *)simplePhrase;

/**
  * Adds a simple phrase that may be spoken by the user, indicating a specific user intent, and maps it to the provided identifier.
  * @param simplePhrase The phrase corresponding to the intent.
  * @param intentId A custom id string to be returned in the SPXIntentRecognitionResult's getIntentId() method.
  */
- (void)addIntentFromPhrase:(nonnull NSString *)simplePhrase mappingToId:(nonnull NSString *)intentId;

/**
  * Adds a single intent by name from the specified Language Understanding Model.
  * @param intentName The name of the single intent to be included from the language understanding model.
  * @param model The language understanding model containing the intent.
  */
- (void)addIntent:(nonnull NSString *)intentName fromModel:(nonnull SPXLanguageUnderstandingModel *)model;

/**
  * Adds a single intent by name from the specified Language Understanding Model, and maps the intent name to the provided identifier.
  * @param intentName The name of the single intent to be included from the language understanding model.
  * @param model The language understanding model containing the intent.
  * @param intentId A custom id string to be returned in the SPXIntentRecognitionResult's getIntentId() method.
  */
- (void)addIntent:(nonnull NSString *)intentName fromModel:(nonnull SPXLanguageUnderstandingModel *)model mappingToId:(nonnull NSString *)intentId;

/**
 * Adds all intents from the specified Language Understanding Model.
 * @param model The language understanding model containing the intents.
 */
- (void)addAllIntentsFromModel:(nonnull SPXLanguageUnderstandingModel *)model;

/**
  * Adds all intents from the specified Language Understanding Model, and maps them to the provided identifier.
  * @param model The language understanding model containing the intents.
  * @param intentId A custom id string to be returned in the SPXIntentRecognitionResult's getIntentId() method.
  */
- (void)addAllIntentsFromModel:(nonnull SPXLanguageUnderstandingModel *)model mappingToId:(nonnull NSString *)intentId;

/**
  * Starts intent recognition, and stops after the first utterance is recognized. It returns the recognition text and intent as result.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  * @return the result of intent recognition.
  */
- (nonnull SPXIntentRecognitionResult *)recognizeOnce;

/**
  * Starts intent recognition, and the block function is called when the first utterance has been recognized.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  */
- (void)recognizeOnceAsync:(nonnull void (^)(SPXIntentRecognitionResult * _Nonnull)) resultReceivedHandler;

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
- (void)addRecognizedEventHandler:(nonnull SPXIntentRecognitionEventHandler)eventHandler;

/**
  * Subscribes to Recognizing event which indicates an intermediate result has been recognized.
  */
- (void)addRecognizingEventHandler:(nonnull SPXIntentRecognitionEventHandler)eventHandler;

/**
  * Subscribes to Canceled event which indicates an error occurred during recognition.
  */
- (void)addCanceledEventHandler:(nonnull SPXIntentRecognitionCanceledEventHandler)eventHandler;

@end
