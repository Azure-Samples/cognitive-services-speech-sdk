//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"

/**
 * Represents keyword recognition model used with StartKeywordRecognitionAsync methods.
 * 
 * Note: Keyword spotting (KWS) functionality might work with any microphone type, official KWS support, however, is currently limited to the microphone arrays found in the Azure Kinect DK hardware or the Speech Devices SDK.
 */
SPX_EXPORT
@interface SPXKeywordRecognitionModel : NSObject

/**
 * Creates a keyword recognition model using the specified file.
 *
 * @param fileName The file name of the keyword recognition model.
 * @return an instance of keyword recognition model.
 */
- (nullable instancetype)initFromFile:(nonnull NSString *)fileName
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Creates a keyword recognition model using the specified file.
 *
 * @param fileName The file name of the keyword recognition model.
 * @param outError error information.
 * @return  an instance of keyword recognition model.
 */
- (nullable instancetype)initFromFile:(nonnull NSString *)fileName error:(NSError * _Nullable * _Nullable)outError;

@end
