//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_config_h
#define speech_config_h

#import <Foundation/Foundation.h>

/**
  * Defines configurations for speech or intent recognition.
  */
@interface SpeechConfig : NSObject

/**
  * Creates an instance of a speech config with specified subscription key and service region.
  * @param subscriptionKey the key of subscription to be used.
  * @param region the region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  * @return a speech config instance.
  */
+ (SpeechConfig *)fromSubscription:(NSString *)subscriptionKey andRegion:(NSString *)region;

/**
  * Creates an instance of a speech config with specified authorization token and service region.
  * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
  * expires, the caller needs to refresh it by calling setAuthorizationToken with a new valid token on the created recognizer.
  * Otherwise, the recognizer instance will encounter errors during recognition.
  * For long-living recognizers, `setAuthorizationToken` needs to called on the recognizer.
  * @param authToken the authorization token.
  * @param region the region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  * @return A speech config instance.
  */
+ (SpeechConfig *)fromAuthorizationToken:(NSString *)authToken andRegion:(NSString *)region;

/**
  * Creates an instance of the speech config with specified endpoint and subscription key.
  * This method is intended only for users who use a non-standard service endpoint or paramters.
  * Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
  * For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
  * the language setting in uri takes precedence, and the effective language is "de-DE".
  * Only the parameters that are not specified in the endpoint URL can be set by other APIs.
  * @param endpointUri The service endpoint to connect to.
  * @param subscriptionKey the subscription key.
  * @return A speech config instance.
  */
+ (SpeechConfig *)fromEndpoint:(NSString *)endpointUri andSubscription:(NSString *)subscriptionKey;

/**
  * Sets the speech recognition language
  * @param lang the language identifier in BCP-47 format.
  */
- (void)setSpeechRecognitionLanguage:(NSString *)lang;

/**
  * Gets the speech recognition language
  * @return the language identifier in BCP-47 format.
  */
- (NSString *)getSpeechRecognitionLanguage;

/**
  * Sets the endpoint ID of a customized speech model that is used for speech recognition.
  * @param endpointId the endpoint ID
  */
- (void)setEndpointId:(NSString *)endpointId;

/**
  * Gets the endpoint ID of a customized speech model that is used for speech recognition.
  * @return the endpoint ID
  */
- (NSString *)getEndpointId;

/**
  * Sets the authorization token.
  * If this is set, subscription key is ignored.
  * User needs to make sure the provided authorization token is valid and not expired.
  * @param token the authorization token.
  */
- (void)setAuthorizationToken:(NSString *)token;

/**
  * Gets the authorization token.
  * @return the authorization token.
  */
- (NSString *)getAuthorizationToken;

/**
  * Gets the subscription key.
  * @return the subscription key.
  */
- (NSString *)getSubscriptionKey;

/**
  * Gets the region name.
  * @return the region name.
  */
- (NSString *)getRegion;

@end

#endif /* speech_config_h */
