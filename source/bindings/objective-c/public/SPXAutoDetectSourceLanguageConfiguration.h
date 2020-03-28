//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSourceLanguageConfiguration.h"

/**
 * Represents auto detection source language configuration
 * Added in version 1.12.0
 */
SPX_EXPORT
@interface SPXAutoDetectSourceLanguageConfiguration : NSObject

/**
 * Initializes an SPXAutoDetectSourceLanguageConfiguration object using languages.
 *
 * @param languages the candidate languages.
 *
 * @return an instance of auto detection source language configuration.
 */
- (nullable instancetype)init:(nonnull NSArray<NSString *> *)languages
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an SPXAutoDetectSourceLanguageConfiguration object using languages.
 *
 * @param languages the candidate languages.
 * @param outError error information.
 *
 * @return an instance of auto detection source language configuration.
 */
- (nullable instancetype)init:(nonnull NSArray<NSString *> *)languages error:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes an SPXAutoDetectSourceLanguageConfiguration object using source language configurations.
 *
 * @param sourceLanguageConfigurations the candidate source languages' configurations.
 *
 * @return an instance of auto detection source language configuration.
 */
- (nullable instancetype)initWithSourceLanguageConfigurations:(nonnull NSArray<SPXSourceLanguageConfiguration *> *)sourceLanguageConfigurations
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an SPXAutoDetectSourceLanguageConfiguration object using source language configurations.
 *
 * @param sourceLanguageConfigurations the candidate source languages' configurations.
 * @param outError error information.
 *
 * @return an instance of auto detection source language configuration.
 */
- (nullable instancetype)initWithSourceLanguageConfigurations:(nonnull NSArray<SPXSourceLanguageConfiguration *> *)sourceLanguageConfigurations
                                                        error:(NSError * _Nullable * _Nullable)outError;

@end
