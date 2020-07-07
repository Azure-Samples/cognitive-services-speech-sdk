//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXRecognitionEventArgs.h"
#import "SPXConversationTranscriptionEventArgs.h"
#import "SPXConversationTranslationResult.h"

/**
 * Defines the payload for conversation translation events.
 *
 */
SPX_EXPORT
@interface SPXConversationTranslationEventArgs : SPXRecognitionEventArgs

/**
 * Represents the conversation translation result.
 */
@property (readonly, nullable)SPXConversationTranslationResult *result;

@end

/**
 * Defines the payload of conversation translation canceled result events.
 */
SPX_EXPORT
@interface SPXConversationTranslationCanceledEventArgs : SPXConversationTranscriptionCanceledEventArgs

@end
