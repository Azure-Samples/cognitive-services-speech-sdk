//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXAudioConfiguration.h"
#import "SPXSpeechConfiguration.h"
#import "SPXSpeechRecognizer.h"
#import "SPXSpeechRecognitionResult.h"
#import "SPXSpeechRecognitionEventArgs.h"

/**
 * Performs speech recognition on the specified audio input, and gets transcribed text as result.
 */
SPX_EXPORT
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
  * @param speechConfiguration speech recognition configuration.
  * @return an instance of speech recognizer.
  */
- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration;

/**
  * Initializes a new instance of speech recognizer using the specified audio config.
  * @param speechConfiguration speech recognition configuration.
  * @param audioConfiguration audio configuration.
  * @return an instance of speech recognizer.
  */
- (nullable instancetype)initWithSpeechConfiguration:(nonnull SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration;

/**
  * Starts speech recognition, and stops after the first utterance is recognized. It returns the final recognition result.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query.
  * For long-running recognition, use continuous recognition instead.
  * @return the result of speech recognition.
  */
- (nonnull SPXSpeechRecognitionResult *)recognizeOnce;

/**
  * Starts asynchronous speech recognition.
  * @param resultReceivedHandler the block function to be called when the first utterance has been recognized.
  * Note: recognizeOnceAsync stops recognizing when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query.
  * For long-running recognition, use continuous recognition instead.
  */
- (void)recognizeOnceAsync:(nonnull void (^)(SPXSpeechRecognitionResult * _Nonnull))resultReceivedHandler;

/**
  * Starts speech recognition on a continuous audio stream, until stopContinuousRecognition() is called.
  * User must subscribe to events to receive recognition results.
  */
- (void)startContinuousRecognition;

/**
  * Stops continuous speech recognition.
  */
- (void)stopContinuousRecognition;

/**
  * Subscribes to the Recognized event which indicates that a final result has been recognized.
  */
- (void)addRecognizedEventHandler:(nonnull SPXSpeechRecognitionEventHandler)eventHandler;

/**
  * Subscribes to the Recognizing event which indicates that an intermediate result has been recognized.
  */
- (void)addRecognizingEventHandler:(nonnull SPXSpeechRecognitionEventHandler)eventHandler;

/**
  * Subscribes to the Canceled event which indicates that an error occurred during recognition.
  */
- (void)addCanceledEventHandler:(nonnull SPXSpeechRecognitionCanceledEventHandler)eventHandler;

@end
