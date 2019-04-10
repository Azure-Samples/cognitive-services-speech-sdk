//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSpeechConfiguration.h"

/**
  * Defines configurations for translation with speech input.
  */
SPX_EXPORT
@interface SPXSpeechTranslationConfiguration : SPXSpeechConfiguration


/**
  * All target languages that have been added for translation.
  */
@property (nonatomic, copy, readonly, nonnull)NSArray *targetLanguages;

/**
  * Voice name of the synthesis output of translated language.
  */
@property (nonatomic, copy, nullable)NSString *voiceName;

/**
  * Initializes speech translation configuration with the specified subscription key and service region.
  * @param subscriptionKey the key of subscription to be used.
  * @param region the region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  * @return an instance of speech translation configuration.
  */
- (nullable instancetype)initWithSubscription:(nonnull NSString *)subscriptionKey region:(nonnull NSString *)region;

/**
  * Initializes speech translation configuration with the specified subscription key and service region.
  * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
  * expires, the caller needs to refresh it by calling setAuthorizationToken with a new valid token on the created recognizer.
  * Otherwise, the recognizer instance will encounter errors during recognition.
  * For long-living recognizers, the authorization token needs to be updated on the recognizer.
  * @param authToken the authorization token.
  * @param region the region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  * @return an instance of speech translation configuration.
  */
- (nullable instancetype)initWithAuthorizationToken:(nonnull NSString *)authToken region:(nonnull NSString *)region;

/**
  * Initializes speech translation configuration with specified endpoint and subscription key.
  * This method is intended only for users who use a non-standard service endpoint or parameters.
  * Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
  * For example, if the recognition language is defined in the URI as query parameter "language=de-DE", and is also set to "en-US" via
  * property speechRecognitionLanguage in SPXSpeechTranslationConfiguration, the language setting in URI takes precedence,
  * and the effective language is "de-DE".
  * Only the parameters that are not specified in the endpoint URI can be set by other APIs.
  * Note: To use an authorization token, use initWithEndpoint, and then set the authorizationToken property on the created SPXSpeechTranslationConfiguration instance.
  * @param endpointUri The service endpoint to connect to.
  * @param subscriptionKey the subscription key.
  * @return an instance of speech translation configuration.
  */
- (nullable instancetype)initWithEndpoint:(nonnull NSString *)endpointUri subscription:(nonnull NSString *)subscriptionKey;

/**
  * Initializes speech translation configuration with specified endpoint.
  * This method is intended only for users who use a non-standard service endpoint or parameters.
  * Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
  * For example, if the recognition language is defined in the URI as query parameter "language=de-DE", and is also set to "en-US" via
  * property speechRecognitionLanguage in SPXSpeechTranslationConfiguration, the language setting in URI takes precedence,
  * and the effective language is "de-DE".
  * Only the parameters that are not specified in the endpoint URI can be set by other APIs.
  * Note: if the endpoint requires a subscription key for authentication, please use initWithEndpoint:subscription: to pass the subscription key as parameter.
  * To use an authorization token, use this method to create a SPXSpeechTranslationConfig instance, and then set the authorizationToken property on the created SPXSpeechTranslationConfiguration instance.
  * Note: Added in version 1.5.0.
  * @param endpointUri The service endpoint to connect to.
  * @return an instance of speech translation configuration.
  */
- (nullable instancetype)initWithEndpoint:(nonnull NSString *)endpointUri;

/**
  * Add a (text) target language for translation.
  * @param lang the language identifier in BCP-47 format.
  */
- (void)addTargetLanguage:(nonnull NSString *)lang;

@end
