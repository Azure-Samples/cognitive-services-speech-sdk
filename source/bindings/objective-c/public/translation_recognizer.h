//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_recognizer_h
#define translation_recognizer_h

#import <Foundation/Foundation.h>
#import "speech_translation_config.h"
#import "audio_config.h"
#import "recognizer.h"
#import "translation_text_result.h"
#import "translation_text_result_event_args.h"
#import "translation_synthesis_result.h"
#import "translation_synthesis_result_event_args.h"

/**
 * Performs translation on the specifed speech input, and gets transcribed and translated texts as result.
 */
@interface TranslationRecognizer : Recognizer

typedef void (^TranslationTextResultEventHandlerBlock)(TranslationRecognizer *, TranslationTextResultEventArgs *);
typedef void (^TranslationTextResultCanceledEventHandlerBlock)(TranslationRecognizer *, TranslationTextResultCanceledEventArgs *);
typedef void (^TranslationSynthesisResultEventHandlerBlock)(TranslationRecognizer *, TranslationSynthesisResultEventArgs *);

/**
  * Creates a new instance of translation recognizer.
  * @param speechConfig translation recognition config.
  * @return a new instance of a translation recognizer.
  */
+ (TranslationRecognizer *)fromConfig:(SpeechTranslationConfig *)speechConfig;

/**
  * Creates a new instance of speech recognizer using the specified audio config.
  * @param speechConfig translation recognition config.
  * @param audioConfig audio configuration.
  * @return a new instance of a speech recognizer.
  */
+ (TranslationRecognizer *)fromConfig:(SpeechTranslationConfig *)speechConfig usingAudio:(AudioConfig *)audioConfig;

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
  * Starts speech translation, and stops after the first utterance is recognized. It returns the transcribed and translated texts as result.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  * @return the result of translation.
  */
- (TranslationTextResult *)recognizeOnce;

/**
  * Starts speech translation, and the block function is called when the first utterance has been recognized.
  * Note: it returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
  */
- (void)recognizeOnceAsync:(void (^)(TranslationTextResult *))resultReceivedBlock;

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
- (void)addRecognizedEventListener:(TranslationTextResultEventHandlerBlock)eventHandler;

/**
  * Subscribes to Recognizing event which indicates an intermediate result has been recognized.
  */
- (void)addRecognizingEventListener:(TranslationTextResultEventHandlerBlock)eventHandler;

/**
  * Subscribes to Synthesizing event which indicates a synthesis voice output has been received.
  */
- (void)addSynthesizingEventListener:(TranslationSynthesisResultEventHandlerBlock)eventHandler;

/**
  * Subscribes to Canceled event which indicates an error occurred during recognition.
  */
- (void)addCanceledEventListener:(TranslationTextResultCanceledEventHandlerBlock)eventHandler;

@end

#endif /* translation_recognizer_h */
