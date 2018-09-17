//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_recognizer_h
#define speech_recognizer_h

#import <Foundation/Foundation.h>
#import "audio_config.h"
#import "speech_config.h"
#import "speech_recognizer.h"
#import "speech_recognition_result.h"
#import "speech_recognition_event_args.h"

/**
 * Performs speech recognition on the specifed audio input, and gets transcribed text as result.
 */
@interface SpeechRecognizer : Recognizer

typedef void (^SpeechRecognitionEventHandlerBlock)(SpeechRecognizer *, SpeechRecognitionEventArgs *);
typedef void (^SpeechRecognitionCanceledEventHandlerBlock)(SpeechRecognizer *, SpeechRecognitionCanceledEventArgs *);

/**
  * Creates a new instance of speech recognizer.
  * @param speechConfig speech recognition config.
  * @return a new instance of a speech recognizer.
  */
+ (SpeechRecognizer *)fromConfig:(SpeechConfig *)speechConfig;

/**
  * Creates a new instance of speech recognizer using the specified audio config.
  * @param speechConfig speech recognition config.
  * @param audioConfig audio configuration.
  * @return a new instance of a speech recognizer.
  */
+ (SpeechRecognizer *)fromConfig:(SpeechConfig *)speechConfig usingAudio:(AudioConfig *)audioConfig;

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
  * Gets the endpoint ID of a customized speech model that is used for speech recognition.
  * @return the endpoint ID of a customized speech model.
  */
- (NSString *)getEndpointId;

/**
  * Starts speech recognition, and stops after the first utterance is recognized. It returns the recognition text as result.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  * @return the result of speech recognition.
  */
- (SpeechRecognitionResult *)recognizeOnce;

/**
  * Starts speech recognition, and the block function is called when the first utterance has been recognized.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  */
- (void)recognizeOnceAsync:(void (^)(SpeechRecognitionResult *))resultReceivedBlock;

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
- (void)addRecognizedEventListener:(SpeechRecognitionEventHandlerBlock)eventHandler;

/**
  * Subscribes to Recognizing event which indicates an intermediate result has been recognized.
  */
- (void)addRecognizingEventListener:(SpeechRecognitionEventHandlerBlock)eventHandler;

/**
  * Subscribes to Canceled event which indicates an error occurred during recognition.
  */
- (void)addCanceledEventListener:(SpeechRecognitionCanceledEventHandlerBlock)eventHandler;

@end

#endif /* speech_recognizer_h */
