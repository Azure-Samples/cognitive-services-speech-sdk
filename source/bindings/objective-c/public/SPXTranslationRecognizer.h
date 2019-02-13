//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
#import "SPXFoundation.h"
#import "SPXSpeechTranslationConfiguration.h"
#import "SPXAudioConfiguration.h"
#import "SPXRecognizer.h"
#import "SPXTranslationRecognitionResult.h"
#import "SPXTranslationRecognitionEventArgs.h"
#import "SPXTranslationSynthesisResult.h"
#import "SPXTranslationSynthesisEventArgs.h"

/**
 * Performs translation on the specified speech input, and gets transcribed and translated texts as result.
 */
SPX_EXPORT
@interface SPXTranslationRecognizer : SPXRecognizer

typedef void (^SPXTranslationRecognitionEventHandler)(SPXTranslationRecognizer * _Nonnull, SPXTranslationRecognitionEventArgs * _Nonnull);
typedef void (^SPXTranslationRecognitionCanceledEventHandler)(SPXTranslationRecognizer * _Nonnull, SPXTranslationRecognitionCanceledEventArgs * _Nonnull);
typedef void (^SPXTranslationSynthesisEventHandler)(SPXTranslationRecognizer * _Nonnull, SPXTranslationSynthesisEventArgs * _Nonnull);

/**
  * Authorization token used to communicate with the translation recognition service.
  * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires,
  * the caller needs to refresh it by calling this setter with a new valid token.
  * Otherwise, the recognizer will encounter errors during recognition.
  */
@property (nonatomic, copy, nullable)NSString *authorizationToken;

/**
  * Initializes a new instance of translation recognizer.
  * @param translationConfiguration translation recognition configuration.
  * @return an instance of translation recognizer.
  */
- (nullable instancetype)init:(nonnull SPXSpeechTranslationConfiguration *)translationConfiguration;

/**
  * Initializes a new instance of speech recognizer using the specified speech and audio configurations.
  * @param translationConfiguration speech translation recognition configuration.
  * @param audioConfiguration audio configuration.
  * @return an instance of translation recognizer.
  */
- (nullable instancetype)initWithSpeechTranslationConfiguration:(nonnull SPXSpeechTranslationConfiguration *)translationConfiguration audioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration;

/**
  * Starts speech translation, and returns after a single utterance is recognized. The end of a
  * single utterance is determined by listening for silence at the end or until a maximum of 15
  * seconds of audio is processed.  The task returns the recognition text as result. 
  * Note: Since recognizeOnce() returns only a single utterance, it is suitable only for single
  * shot recognition like command or query. 
  * For long-running multi-utterance recognition, use startContinuousRecognition() instead.
  * @return the result of translation.
  */
- (nonnull SPXTranslationRecognitionResult *)recognizeOnce;

/**
  * Starts translation, and returns after a single utterance is recognized. The end of a
  * single utterance is determined by listening for silence at the end or until a maximum of 15
  * seconds of audio is processed.  The task returns the recognition text as result. 
  * Note: Since recognizeOnceAsync() returns only a single utterance, it is suitable only for single
  * shot recognition like command or query. 
  * For long-running multi-utterance recognition, use startContinuousRecognition() instead.
  * @param resultReceivedHandler the block function to be called when the first utterance has been recognized.
    */
- (void)recognizeOnceAsync:(nonnull void (^)(SPXTranslationRecognitionResult * _Nonnull))resultReceivedHandler;

/**
  * Starts speech translation on a continuous audio stream, until stopContinuousRecognition() is called.
  * The user must subscribe to events to receive translation results.
  */
- (void)startContinuousRecognition;

/**
  * Stops continuous translation.
  */
- (void)stopContinuousRecognition;

/**
  * Subscribes to the Recognized event which indicates that a final result has been recognized.
  */
- (void)addRecognizedEventHandler:(nonnull SPXTranslationRecognitionEventHandler)eventHandler;

/**
  * Subscribes to the Recognizing event which indicates that an intermediate result has been recognized.
  */
- (void)addRecognizingEventHandler:(nonnull SPXTranslationRecognitionEventHandler)eventHandler;

/**
  * Subscribes to the Synthesizing event which indicates that a synthesis voice output has been received.
  */
- (void)addSynthesizingEventHandler:(nonnull SPXTranslationSynthesisEventHandler)eventHandler;

/**
  * Subscribes to the Canceled event which indicates that an error occurred during recognition.
  */
- (void)addCanceledEventHandler:(nonnull SPXTranslationRecognitionCanceledEventHandler)eventHandler;

@end
