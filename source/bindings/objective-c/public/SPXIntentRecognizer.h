//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSpeechConfiguration.h"
#import "SPXAudioConfiguration.h"
#import "SPXPropertyCollection.h"
#import "SPXRecognizer.h"
#import "SPXLanguageUnderstandingModel.h"
#import "SPXIntentRecognitionResult.h"
#import "SPXIntentRecognitionEventArgs.h"

/**
  * Performs intent recognition on speech input. It returns both recognized text and recognized intent.
  */
SPX_EXPORT
@interface SPXIntentRecognizer : SPXRecognizer

typedef void (^SPXIntentRecognitionEventHandler)(SPXIntentRecognizer * _Nonnull, SPXIntentRecognitionEventArgs * _Nonnull);
typedef void (^SPXIntentRecognitionCanceledEventHandler)(SPXIntentRecognizer * _Nonnull, SPXIntentRecognitionCanceledEventArgs * _Nonnull);

/**
  * The authorization token used to communicate with the intent recognition service.
  * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires,
  * the caller needs to refresh it by calling this setter with a new valid token.
  * Otherwise, the recognizer will encounter errors during recognition.
  */
@property (nonatomic, copy, nullable)NSString *authorizationToken;

/**
  * Initializes a new instance of intent recognizer using the specified speech configuration.
  * @param speechConfiguration speech recognition configuration.
  * @return an intent recognizer.
  */
- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration;

/**
  * Initializes a new instance of an intent recognizer using the specified speech and audio configurations.
  * @param speechConfiguration speech recognition configuration.
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
  * Starts intent recognition, and returns after a single utterance is recognized. The end of a
  * single utterance is determined by listening for silence at the end or until a maximum of 15
  * seconds of audio is processed.  The task returns the recognition text as result. 
  * Note: Since recognizeOnce() returns only a single utterance, it is suitable only for single
  * shot recognition like command or query. 
  * For long-running multi-utterance recognition, use startContinuousRecognitionAsync() instead.
  * @return the result of intent recognition.
  */
- (nonnull SPXIntentRecognitionResult *)recognizeOnce;

/**
  * Starts speech recognition, and returns after a single utterance is recognized. The end of a
  * single utterance is determined by listening for silence at the end or until a maximum of 15
  * seconds of audio is processed.  The task returns the recognition text as result. 
  * Note: Since recognizeOnceAsync() returns only a single utterance, it is suitable only for single
  * shot recognition like command or query. 
  * For long-running multi-utterance recognition, use startContinuousRecognitionAsync() instead.
  * @param resultReceivedHandler the block function to be called when the first utterance has been recognized.
  */
- (void)recognizeOnceAsync:(nonnull void (^)(SPXIntentRecognitionResult * _Nonnull)) resultReceivedHandler;

/**
  * Starts speech recognition on a continuous audio stream, until stopContinuousRecognition() is called.
  * User must subscribe to events to receive recognition results.
  */
- (void)startContinuousRecognition;

/**
  * Stops continuous intent recognition.
  */
- (void)stopContinuousRecognition;

/**
  * Subscribes to the Recognized event which indicates that a final result has been recognized.
  */
- (void)addRecognizedEventHandler:(nonnull SPXIntentRecognitionEventHandler)eventHandler;

/**
  * Subscribes to the Recognizing event which indicates an that intermediate result has been recognized.
  */
- (void)addRecognizingEventHandler:(nonnull SPXIntentRecognitionEventHandler)eventHandler;

/**
  * Subscribes to the Canceled event which indicates that an error occurred during recognition.
  */
- (void)addCanceledEventHandler:(nonnull SPXIntentRecognitionCanceledEventHandler)eventHandler;

@end
