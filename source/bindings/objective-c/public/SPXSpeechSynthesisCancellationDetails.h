//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSpeechEnums.h"
#import "SPXSpeechSynthesisResult.h"
#import "SPXAudioDataStream.h"


/**
 * Defines detailed information about why a speech synthesis result was canceled.
 * 
 * Added in version 1.7.0
 */
SPX_EXPORT
@interface SPXSpeechSynthesisCancellationDetails : NSObject

/**
 * The reason the synthesis was canceled.
 */
@property (readonly)SPXCancellationReason reason;

/**
 * The error message in case of an unsuccessful synthesis (reason is set to Error). If reason is not an error, errorCode returns NoErro.
 */
@property (readonly)SPXCancellationErrorCode errorCode;

/**
 * The error message in case of an unsuccessful synthesis (reason is set to Error).
 */
@property (copy, readonly, nullable)NSString *errorDetails;

/**
 * Creates an instance of SPXSpeechSynthesisCancellationDetails object for the canceled SPXSpeechSynthesisResult.
 *
 * @param synthesisResult the synthesis result that was canceled.
 * @return the speech synthesis cancellation details object being created.
 */
- (nullable instancetype)initFromCanceledSynthesisResult:(nonnull SPXSpeechSynthesisResult *)synthesisResult
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Creates an instance of SPXSpeechSynthesisCancellationDetails object for the canceled SPXSpeechSynthesisResult.
 *
 * @param synthesisResult the synthesis result that was canceled.
 * @param outError error information.
 * @return the speech synthesis cancellation details object being created.
 */
- (nullable instancetype)initFromCanceledSynthesisResult:(nonnull SPXSpeechSynthesisResult *)synthesisResult error:(NSError * _Nullable * _Nullable)outError;

/**
 * Creates an instance of SPXSpeechSynthesisCancellationDetails object for the canceled SPXAudioDataStream.
 *
 * @param stream the audio data stream that was canceled.
 * @return the speech synthesis cancellation details object being created.
 */
- (nullable instancetype)initFromCanceledAudioDataStream:(nonnull SPXAudioDataStream *)stream
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Creates an instance of SPXSpeechSynthesisCancellationDetails object for the canceled SPXAudioDataStream.
 *
 * @param stream the audio data stream that was canceled.
 * @param outError error information.
 * @return the speech synthesis cancellation details object being created.
 */
- (nullable instancetype)initFromCanceledAudioDataStream:(nonnull SPXAudioDataStream *)stream error:(NSError * _Nullable * _Nullable)outError;

@end