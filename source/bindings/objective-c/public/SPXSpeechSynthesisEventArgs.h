//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSpeechEnums.h"

/**
 * Defines the payload of speech synthesis event.
 * 
 * Added in version 1.7.0
 */
SPX_EXPORT
@interface SPXSpeechSynthesisEventArgs : NSObject

/**
 * The synthesis result.
 */
@property (readonly, nonnull)SPXSpeechSynthesisResult *result;

@end

/**
 * Defines the payload of speech synthesis word boundary event.
 *
 * Added in version 1.7.0
 */
SPX_EXPORT
@interface SPXSpeechSynthesisWordBoundaryEventArgs : NSObject

/**
 * Word boundary audio offset.
 */    
@property (readonly)NSUInteger audioOffset;

/**
 * Word boundary text offset.
 */
@property (readonly)NSUInteger textOffset;

/**
 * Word boundary word length.
 */
@property (readonly)NSUInteger wordLength;

@end
