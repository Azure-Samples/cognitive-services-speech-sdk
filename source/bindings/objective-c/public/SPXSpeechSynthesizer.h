//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXAudioConfiguration.h"
#import "SPXSpeechConfiguration.h"
#import "SPXSpeechSynthesisResult.h"
#import "SPXSpeechSynthesisEventArgs.h"

/**
 * Performs speech synthesis from a text to a specified audio output.
 * 
 * Added in version 1.7.0
 */
SPX_EXPORT
@interface SPXSpeechSynthesizer : NSObject

typedef void (^SPXSpeechSynthesisEventHandler)(SPXSpeechSynthesizer * _Nonnull, SPXSpeechSynthesisEventArgs * _Nonnull);
typedef void (^SPXSpeechSynthesisWordBoundaryEventHandler)(SPXSpeechSynthesizer * _Nonnull, SPXSpeechSynthesisWordBoundaryEventArgs * _Nonnull);

/**
  * The collection of properties and their values defined for this SPXSpeechRecognizer.
  */
@property (readonly, nullable)id <SPXPropertyCollection> properties;

/**
 * Authorization token used to communicate with the speech synthesis service.
 *
 * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires,
 * the caller needs to refresh it by calling this setter with a new valid token.
 * Otherwise, the synthesizer will encounter errors during speech synthesis.
 */
@property (nonatomic, copy, nullable)NSString *authorizationToken;

/**
 * Initializes a new instance of speech synthesizer using a speech config.
 *
 * @param speechConfiguration speech recognition configuration.
 * @return an instance of speech recognizer.
 */
- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes a new instance of speech synthesizer using a speech config.
 *
 * @param speechConfiguration speech recognition configuration.
 * @param outError error information.
 * @return an instance of speech recognizer.
 */
- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration error:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes a new instance of speech synthesizer using a speech config and audio config.
 *
 * @param speechConfiguration speech recognition configuration.
 * @param audioConfiguration audio configuration.
 * @return an instance of speech synthesizer.
 */
- (nullable instancetype)initWithSpeechConfiguration:(nonnull SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(nullable SPXAudioConfiguration *)audioConfiguration
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes a new instance of speech synthesizer using a speech config and audio config.
 *
 * @param speechConfiguration speech recognition configuration.
 * @param audioConfiguration audio configuration.
 * @param outError error information.
 * @return an instance of speech synthesizer.
 */
- (nullable instancetype)initWithSpeechConfiguration:(nonnull SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(nullable SPXAudioConfiguration *)audioConfiguration error:(NSError * _Nullable * _Nullable)outError;

/**
 * Execute the speech synthesis on plain text.
 * 
 * @param text the plain text for synthesis.
 * @return the result of speech synthesis.
 */
- (nonnull SPXSpeechSynthesisResult *)speakText:(nonnull NSString*)text NS_RETURNS_RETAINED
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Execute the speech synthesis on plain text.
 * 
 * @param text the plain text for synthesis.
 * @param outError error information.
 * @return the result of speech synthesis.
 */
- (nullable SPXSpeechSynthesisResult *)speakText:(nonnull NSString*)text error:(NSError * _Nullable * _Nullable)outError NS_RETURNS_RETAINED;

/**
 * Execute the speech synthesis on SSML.
 * 
 * @param ssml the SSML for synthesis.
 * @return the result of speech synthesis.
 */
- (nonnull SPXSpeechSynthesisResult *)speakSsml:(nonnull NSString*)ssml NS_RETURNS_RETAINED
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Execute the speech synthesis on SSML.
 * 
 * @param ssml the SSML for synthesis.
 * @param outError error information.
 * @return the result of speech synthesis.
 */
- (nullable SPXSpeechSynthesisResult *)speakSsml:(nonnull NSString*)ssml error:(NSError * _Nullable * _Nullable)outError NS_RETURNS_RETAINED;

/**
 * Start the speech synthesis on plain text.
 * 
 * @param text the plain the for synthesis.
 * @return the result of speech synthesis.
 */
- (nonnull SPXSpeechSynthesisResult *)startSpeakingText:(nonnull NSString*)text NS_RETURNS_RETAINED
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Start the speech synthesis on plain text.
 * 
 * @param text the plain text for synthesis.
 * @param outError error information.
 * @return the result of speech synthesis.
 */
- (nullable SPXSpeechSynthesisResult *)startSpeakingText:(nonnull NSString*)text error:(NSError * _Nullable * _Nullable)outError NS_RETURNS_RETAINED;

/**
 * Start the speech synthesis on SSML.
 * 
 * @param ssml the SSML for synthesis.
 * @return the result of speech synthesis.
 */
- (nonnull SPXSpeechSynthesisResult *)startSpeakingSsml:(nonnull NSString*)ssml NS_RETURNS_RETAINED
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Start the speech synthesis on SSML.
 * 
 * @param ssml the SSML for synthesis.
 * @param outError error information.
 * @return the result of speech synthesis.
 */
- (nullable SPXSpeechSynthesisResult *)startSpeakingSsml:(nonnull NSString*)ssml error:(NSError * _Nullable * _Nullable)outError NS_RETURNS_RETAINED;

/**
 * Subscribes to the SynthesisStarted event which indicates that the synthesis just started.
 */
- (void)addSynthesisStartedEventHandler:(nonnull SPXSpeechSynthesisEventHandler)eventHandler;

/**
 * Subscribes to the Synthesizing event which indicates that the synthesis is on going.
 */
- (void)addSynthesizingEventHandler:(nonnull SPXSpeechSynthesisEventHandler)eventHandler;

/**
 * Subscribes to the SynthesisCompleted event which indicates that the synthesis completed.
 */
- (void)addSynthesisCompletedEventHandler:(nonnull SPXSpeechSynthesisEventHandler)eventHandler;

/**
 * Subscribes to the SynthesisCanceled event which indicates that the synthesis is canceled.
 */
- (void)addSynthesisCanceledEventHandler:(nonnull SPXSpeechSynthesisEventHandler)eventHandler;

/**
 * Subscribes to the SynthesisWordBoundary event which indicates that a word boundary is received.
 */
- (void)addSynthesisWordBoundaryEventHandler:(nonnull SPXSpeechSynthesisWordBoundaryEventHandler)eventHandler;

@end
