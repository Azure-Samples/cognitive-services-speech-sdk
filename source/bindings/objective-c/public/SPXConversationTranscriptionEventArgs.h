//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXRecognitionEventArgs.h"
#import "SPXConversationTranscriptionResult.h"

/**
 * Defines the payload for conversation transcription events.
 *
 */
SPX_EXPORT
@interface SPXConversationTranscriptionEventArgs : SPXRecognitionEventArgs

/**
 * Represents the conversation transcription result.
 */
@property (readonly, nullable)SPXConversationTranscriptionResult *result;

@end

/**
 * Defines the payload of conversation transcription canceled result events.
 */
SPX_EXPORT
@interface SPXConversationTranscriptionCanceledEventArgs : SPXConversationTranscriptionEventArgs

/**
 * The reason the trancription was canceled.
 */
@property (readonly)SPXCancellationReason reason;

/**
 * The error code in case of an unsuccessful transcription (reason is set to Error).
 *
 */
@property (readonly)SPXCancellationErrorCode errorCode;

/**
 * The error message in case of an unsuccessful transcription (reason is set to Error).
 */
@property (copy, readonly, nullable)NSString *errorDetails;

@end
