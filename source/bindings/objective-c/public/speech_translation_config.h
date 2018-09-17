//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_translation_config_h
#define speech_translation_config_h

#import <Foundation/Foundation.h>
#import "speech_config.h"

/**
  * Defines configurations for translation with speech input.
  */
@interface SpeechTranslationConfig : SpeechConfig

/**
  * Creates an instance of a speech translation config with specified subscription key and service region.
  * @param subscriptionKey the key of subscription to be used.
  * @param region the region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  * @return a speech translation config instance.
  */
+ (SpeechTranslationConfig *)fromSubscription:(NSString *)subscriptionKey andRegion:(NSString *)region;

/**
  * Creates an instance of a speech translation config with specified authorization token and service region.
  * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
  * expires, the caller needs to refresh it by calling setAuthorizationToken with a new valid token on the created recognizer.
  * Otherwise, the recognizer instance will encounter errors during recognition.
  * For long-living recognizers, `setAuthorizationToken` needs to called on the recognizer.
  * @param authToken the authorization token.
  * @param region the region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  * @return A speech translation config instance.
  */
+ (SpeechTranslationConfig *)fromAuthorizationToken:(NSString *)authToken andRegion:(NSString *)region;

/**
  * Creates an instance of the speech translation config with specified endpoint and subscription key.
  * This method is intended only for users who use a non-standard service endpoint or paramters.
  * Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
  * For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
  * the language setting in uri takes precedence, and the effective language is "de-DE".
  * Only the parameters that are not specified in the endpoint URL can be set by other APIs.
  * @param endpointUri The service endpoint to connect to.
  * @param subscriptionKey the subscription key.
  * @return A speech translation config instance.
  */
+ (SpeechTranslationConfig *)fromEndpoint:(NSString *)endpointUri andSubscription:(NSString *)subscriptionKey;

/**
  * Add a (text) target language for translation.
  * @param lang the language identifier in BCP-47 format.
  */
- (void)addTargetLanguage:(NSString *)lang;

/**
  * Gets all target languages that have been added for translation.
  * @return the list of target languages.
  */
- (NSArray *)getTargetLanguages;

/**
  * Sets voice of the translated language, enable voice synthesis output.
  * @param voice the voice name of synthesis output.
  */
- (void)setVoiceName:(NSString *)voice;

/**
  * Gets voice name of synthesis output for translation.
  * @return voice the voice name of synthesis output.
  */
- (NSString *)getVoiceName;

@end

#endif /* speech_translation_config_h */
