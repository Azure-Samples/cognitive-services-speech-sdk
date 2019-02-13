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
  * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires,
  * the caller needs to refresh it by calling this setter with a new valid token.
  * Otherwise, the recognizer will encounter errors during recognition.
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
  * Starts speech recognition, and returns after a single utterance is recognized. The end of a
  * single utterance is determined by listening for silence at the end or until a maximum of 15
  * seconds of audio is processed.  The task returns the recognition text as result. 
  * Note: Since recognizeOnceAsync() returns only a single utterance, it is suitable only for single
  * shot recognition like command or query. 
  * For long-running multi-utterance recognition, use startContinuousRecognition() instead.
  * @return the result of speech recognition.
  */
- (nonnull SPXSpeechRecognitionResult *)recognizeOnce;

/**
  * Starts speech recognition, and returns after a single utterance is recognized. The end of a
  * single utterance is determined by listening for silence at the end or until a maximum of 15
  * seconds of audio is processed.  The task returns the recognition text as result. 
  * Note: Since recognizeOnceAsync() returns only a single utterance, it is suitable only for single
  * shot recognition like command or query. 
  * For long-running multi-utterance recognition, use startContinuousRecognition() instead.
  * @param resultReceivedHandler the block function to be called when the first utterance has been recognized.
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
