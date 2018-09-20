//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import <Foundation/Foundation.h>
#import "SPXAudioConfiguration.h"
#import "SPXSpeechConfiguration.h"
#import "SPXSpeechRecognizer.h"
#import "SPXSpeechRecognitionResult.h"
#import "SPXSpeechRecognitionEventArgs.h"

/**
 * Performs speech recognition on the specified audio input, and gets transcribed text as result.
 */
@interface SPXSpeechRecognizer : SPXRecognizer

typedef void (^SPXSpeechRecognitionEventHandler)(SPXSpeechRecognizer * _Nonnull, SPXSpeechRecognitionEventArgs * _Nonnull);
typedef void (^SPXSpeechRecognitionCanceledEventHandler)(SPXSpeechRecognizer * _Nonnull, SPXSpeechRecognitionCanceledEventArgs * _Nonnull);

/**
  * Authorization token used to communicate with the speech recognition service.
  */
@property (nonatomic, copy, nullable)NSString *authorizationToken;

/**
  * Endpoint ID of a customized speech model that is used for speech recognition.
  */
@property (nonatomic, copy, readonly, nullable)NSString *endpointId;

/**
  * Initializes a new instance of speech recognizer.
  * @param speechConfiguration speech recognition config.
  * @return an instance of speech recognizer.
  */
- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration;

/**
  * Initializes a new instance of speech recognizer using the specified audio config.
  * @param speechConfiguration speech recognition config.
  * @param audioConfiguration audio configuration.
  * @return an instance of speech recognizer.
  */
- (nullable instancetype)initWithSpeechConfiguration:(nonnull SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration;

/**
  * Starts speech recognition, and stops after the first utterance is recognized. It returns the recognition text as result.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  * @return the result of speech recognition.
  */
- (nonnull SPXSpeechRecognitionResult *)recognizeOnce;

/**
  * Starts speech recognition, and the block function is called when the first utterance has been recognized.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  */
- (void)recognizeOnceAsync:(nonnull void (^)(SPXSpeechRecognitionResult * _Nonnull))resultReceivedHandler;

/**
  * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
  * User must subscribe to events to receive recognition results.
  */
- (void)startContinuousRecognition;

/**
  * Stops continuous speech recognition.
  */
- (void)stopContinuousRecognition;

/**
  * Subscribes to Recognized event which indicates a final result has been recognized.
  */
- (void)addRecognizedEventHandler:(nonnull SPXSpeechRecognitionEventHandler)eventHandler;

/**
  * Subscribes to Recognizing event which indicates an intermediate result has been recognized.
  */
- (void)addRecognizingEventHandler:(nonnull SPXSpeechRecognitionEventHandler)eventHandler;

/**
  * Subscribes to Canceled event which indicates an error occurred during recognition.
  */
- (void)addCanceledEventHandler:(nonnull SPXSpeechRecognitionCanceledEventHandler)eventHandler;

@end
