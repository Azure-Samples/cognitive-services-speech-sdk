//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
#import <Foundation/Foundation.h>
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
@interface SPXTranslationRecognizer : SPXRecognizer

typedef void (^SPXTranslationRecognitionEventHandler)(SPXTranslationRecognizer * _Nonnull, SPXTranslationRecognitionEventArgs * _Nonnull);
typedef void (^SPXTranslationRecognitionCanceledEventHandler)(SPXTranslationRecognizer * _Nonnull, SPXTranslationRecognitionCanceledEventArgs * _Nonnull);
typedef void (^SPXTranslationSynthesisEventHandler)(SPXTranslationRecognizer * _Nonnull, SPXTranslationSynthesisEventArgs * _Nonnull);

/**
  * Authorization token used to communicate with the translation recognition service.
  */
@property (nonatomic, copy, nullable)NSString *authorizationToken;

/**
  * Initializes a new instance of translation recognizer.
  * @param translationConfiguration translation recognition config.
  * @return an instance of translation recognizer.
  */
- (nullable instancetype)init:(nonnull SPXSpeechTranslationConfiguration *)translationConfiguration;

/**
  * Initializes a new instance of speech recognizer using the specified speech configuration and audio configuration.
  * @param translationConfiguration speech translation recognition config.
  * @param audioConfiguration audio configuration.
  * @return an instance of translation recognizer.
  */
- (nullable instancetype)initWithSpeechTranslationConfiguration:(nonnull SPXSpeechTranslationConfiguration *)translationConfiguration audioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration;

/**
  * Starts speech translation, and stops after the first utterance is recognized. It returns the transcribed and translated texts as result.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  * @return the result of translation.
  */
- (nonnull SPXTranslationRecognitionResult *)recognizeOnce;

/**
  * Starts speech translation, and the block function is called when the first utterance has been recognized.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  */
- (void)recognizeOnceAsync:(nonnull void (^)(SPXTranslationRecognitionResult * _Nonnull))resultReceivedHandler;

/**
  * Starts speech translation on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
  * User must subscribe to events to receive translation results.
  */
- (void)startContinuousRecognition;

/**
  * Stops continuous translation.
  */
- (void)stopContinuousRecognition;

/**
  * Subscribes to Recognized event which indicates a final result has been recognized.
  */
- (void)addRecognizedEventHandler:(nonnull SPXTranslationRecognitionEventHandler)eventHandler;

/**
  * Subscribes to Recognizing event which indicates an intermediate result has been recognized.
  */
- (void)addRecognizingEventHandler:(nonnull SPXTranslationRecognitionEventHandler)eventHandler;

/**
  * Subscribes to Synthesizing event which indicates a synthesis voice output has been received.
  */
- (void)addSynthesizingEventHandler:(nonnull SPXTranslationSynthesisEventHandler)eventHandler;

/**
  * Subscribes to Canceled event which indicates an error occurred during recognition.
  */
- (void)addCanceledEventHandler:(nonnull SPXTranslationRecognitionCanceledEventHandler)eventHandler;

@end
