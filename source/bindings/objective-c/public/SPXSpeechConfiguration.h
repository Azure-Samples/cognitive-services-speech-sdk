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
  * Authorization token.
  * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
  * expires, the caller needs to refresh it by calling this setter with a new valid token.
  * As configuration values are copied when creating a new recognizer, the new token value will not apply to recognizers that have already been created.
  * For recognizers that have been created before, you need to set authorization token of the corresponding recognizer
  * to refresh the token. Otherwise, the recognizers will encounter errors during recognition.
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
  * expires, the caller needs to refresh it by calling this setter with a new valid token.
  * As configuration values are copied when creating a new recognizer, the new token value will not apply to recognizers that have already been created.
  * For recognizers that have been created before, you need to set authorization token of the corresponding recognizer
  * to refresh the token. Otherwise, the recognizers will encounter errors during recognition.
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
  * Note: To use authorization token with initWithEndpoint, pass an empty string to the subscriptionKey in the initWithEndpoint method,
  * and then set the authorizationToken property on the created SpeechConfiguration instance to use the authorization token.
  * @param endpointUri The service endpoint to connect to.
  * @param subscriptionKey the subscription key.
  * @return A speech configuration instance.
  */
- (nullable instancetype)initWithEndpoint:(nonnull NSString *)endpointUri subscription:(nonnull NSString *)subscriptionKey;

/**
 * Sets proxy configuration
 * Added in version 1.1.0
 * Note: Proxy functionality is not available on iOS and macOS. This function will have no effect on these platforms.
 * @param proxyHostName the host name of the proxy server, without the protocol scheme (http://)
 * @param proxyPort the port number of the proxy server.
 * @param proxyUserName the user name of the proxy server. Use empty string if no user name is needed.
 * @param proxyPassword the password of the proxy server. Use empty string if no user password is needed.
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
