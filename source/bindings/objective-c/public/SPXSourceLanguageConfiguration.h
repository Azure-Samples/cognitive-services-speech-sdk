//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"

/**
 * Represents source language configuration, allowing specifying the source language and customized endpoint
 * Added in version 1.12.0
 */
SPX_EXPORT
@interface SPXSourceLanguageConfiguration : NSObject

/**
 * Initializes an SPXSourceLanguageConfiguration object using the language.
 *
 * @param language value of the language.
 *
 * @return an instance of source language configuration.
 */
- (nullable instancetype)init:(nonnull NSString *)language
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an SPXSourceLanguageConfiguration object using the language.
 *
 * @param language value of the language.
 * @param outError error information.
 *
 * @return an instance of source language configuration.
 */
- (nullable instancetype)init:(nonnull NSString *)language error:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes an SPXSourceLanguageConfiguration object using the language and corresponding endpoint id
 *
 * @param language value of the language.
 * @param endpointId id of the customized endpoint, will be used to recognize the audio in the source language. 
 *
 * @return an instance of source language configuration.
 */
 - (nullable instancetype)initWithLanguage:(nonnull NSString *)language endpointId:(nonnull NSString *)endpointId
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an SPXSourceLanguageConfiguration object using the language and corresponding endpoint id
 *
 * @param language value of the language.
 * @param endpointId id of the customized endpoint, will be used to recognize the audio in the source language.
 * @param outError error information.
 *
 * @return an instance of source language configuration.
 */
 - (nullable instancetype)initWithLanguage:(nonnull NSString *)language
                                endpointId:(nonnull NSString *)endpointId
                                     error:(NSError * _Nullable * _Nullable)outError;

@end