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
 * The output format of the recognition result.
 */
@property (nonatomic)SPXOutputFormat outputFormat;

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
  * Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
  * For example, if the recognition language is defined in the URI as query parameter "language=de-DE", and is also set to "en-US" via
  * property speechRecognitionLanguage in SPXSpeechConfiguration, the language setting in the URI takes precedence, and the effective language is "de-DE".
  * Only the parameters that are not specified in the endpoint URI can be set by other APIs.
  * Note: To use an authorization token, use initWithEndpoint, and then set the authorizationToken property on the created SPXSpeechConfiguration instance.
  * @param endpointUri The service endpoint to connect to.
  * @param subscriptionKey the subscription key.
  * @return A speech configuration instance.
  */
- (nullable instancetype)initWithEndpoint:(nonnull NSString *)endpointUri subscription:(nonnull NSString *)subscriptionKey;

/**
  * Initializes an instance of the speech configuration with specified endpoint.
  * This method is intended only for users who use a non-standard service endpoint or parameters.
  * Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
  * For example, if the recognition language is defined in the uri as query parameter "language=de-DE", and is also set to "en-US" via
  * property speechRecognitionLanguage in SpeechConfiguration, the language setting in the uri takes precedence, and the effective language is "de-DE".
  * Only the parameters that are not specified in the endpoint URL can be set by other APIs.
  * Note: if the endpoint requires a subscription key for authentication, please use initWithEndpoint:subscription: to pass the subscription key as parameter.
  * To use an authorization token, use this method to create a SpeechConfig instance, and then set the authorizationToken property on the created SPXSpeechConfiguration instance.
  * Note: Added in version 1.5.0.
  * @param endpointUri The service endpoint to connect to.
  * @return A speech configuration instance.
  */
- (nullable instancetype)initWithEndpoint:(nonnull NSString *)endpointUri;

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

/**
 *  Sets a property value that will be passed to service using the specified channel.
 *  Added in version 1.5.0.
 *  @param name the property name.
 *  @param value the property value.
 *  @param channel the channel used to pass the specified property to service.
 */
-(void)setServicePropertyTo:(nonnull NSString *)value byName:(nonnull NSString*)name usingChannel:(SPXServicePropertyChannel)channel;

/**
 * Sets profanity option.
 * Added in version 1.5.0.
 * @param profanity Profanity option value.
 */
-(void)setProfanityOptionTo:(SPXSpeechConfigProfanityOption)profanity;

/**
 * Enables audio logging in service.
 * Added in version 1.5.0.
 */
-(void)enableAudioLogging;

/**
 * Includes word-level timestamps in response result.
 * Added in version 1.5.0.
 */
-(void)requestWordLevelTimestamps;

/**
 * Enables dictation mode. Only supported in speech continuous recognition.
 * Added in version 1.5.0.
 */
-(void)enableDictation;

@end
