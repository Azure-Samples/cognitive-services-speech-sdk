//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXPropertyCollection.h"

/**
  * Defines configurations for speech or intent recognition.
  */
SPX_EXPORT
@interface SPXSpeechConfiguration : NSObject

/**
  * Tag of speech recognition language, using BCP-47 format.
  */
@property (nonatomic, copy, nullable)NSString *speechRecognitionLanguage;

/**
  * Endpoint ID of a customized speech model that is used for speech recognition.
  */
@property (nonatomic, copy, nullable)NSString *endpointId;

/**
  * Authorization token. If this is set, the subscription key is ignored.
  * The user needs to make sure the provided authorization token is valid and not expired.
  */
@property (nonatomic, copy, nullable)NSString *authorizationToken;

/**
  * Subscription key.
  */
@property (nonatomic, copy, readonly, nullable)NSString *subscriptionKey;

/**
  * Region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  */
@property (nonatomic, copy, readonly, nullable)NSString *region;


/**
  * Initializes an instance of a speech configuration with the specified subscription key and service region.
  * @param subscriptionKey the subscription key to be used.
  * @param region the region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  * @return a speech configuration instance.
  */
- (nullable instancetype)initWithSubscription:(nonnull NSString *)subscriptionKey region:(nonnull NSString *)region;

/**
  * Initializes an instance of a speech configuration with specified authorization token and service region.
  * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
  * expires, the caller needs to refresh it by calling setAuthorizationToken with a new valid token on the created recognizer.
  * Otherwise, the recognizer instance will encounter errors during recognition.
  * For long-living recognizers, the authorization token needs to be updated on the recognizer.
  * @param authToken the authorization token.
  * @param region the region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  * @return a speech configuration instance.
  */
- (nullable instancetype)initWithAuthorizationToken:(nonnull NSString *)authToken region:(nonnull NSString *)region;

/**
  * Initializes an instance of the speech configuration with specified endpoint and subscription key.
  * This method is intended only for users who use a non-standard service endpoint or parameters.
  * Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
  * For example, if language is defined in the uri as query parameter "language=de-DE", and is also set to "en-US" via
  * property speechRecognitionLanguage in SpeechConfiguration, the language setting in the uri takes precedence, and the effective language is "de-DE".
  * Only the parameters that are not specified in the endpoint URL can be set by other APIs.
  * @param endpointUri The service endpoint to connect to.
  * @param subscriptionKey the subscription key.
  * @return A speech configuration instance.
  */
- (nullable instancetype)initWithEndpoint:(nonnull NSString *)endpointUri subscription:(nonnull NSString *)subscriptionKey;

/**
 * Sets proxy configuration
 * Added in version 1.1.0
 * @param proxyHostName the host name of the proxy server.
 * @param proxyPort the port number of the proxy server.
 * @param proxyUserName the user name of the proxy server.
 * @param proxyPassword the password of the proxy server.
 */
-(void)setProxyUsingHost:(nonnull NSString *)proxyHostName Port:(uint32_t)proxyPort UserName:(nullable NSString *)proxyUserName Password:(nullable NSString *)proxyPassword;

/**
 * Returns the property value.
 * If the name is not available, it returns an empty string.
 * @param name property name.
 * @return value of the property.
 */
-(nullable NSString *)getPropertyByName:(nonnull NSString *)name;

/**
 * Sets the property value by name.
 * @param name property name.
 * @param value value of the property.
 */
-(void)setPropertyTo:(nonnull NSString *)value byName:(nonnull NSString *)name;

/**
 * Returns the property value.
 * If the specified id is not available, it returns an empty string.
 * @param propertyId property id.
 * @return value of the property.
 */
-(nullable NSString *)getPropertyById:(SPXPropertyId)propertyId;

/**
 * Sets the property value by property id.
 * @param propertyId property id.
 * @param value value of the property.
 */
-(void)setPropertyTo:(nonnull NSString *)value byId:(SPXPropertyId)propertyId;
@end
