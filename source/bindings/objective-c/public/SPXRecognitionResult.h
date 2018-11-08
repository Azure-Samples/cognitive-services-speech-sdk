//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSpeechEnums.h"
#import "SPXPropertyCollection.h"

/**
  * Represents the recognition result base class.
  */
SPX_EXPORT
@interface SPXRecognitionResult : NSObject

/**
  * The result identifier.
  */
@property (copy, readonly, nonnull)NSString *resultId;

/**
  * The reason the result was created.
  */
@property (readonly)SPXResultReason reason;

/**
  * The recognized text in the result.
  */
@property (copy, readonly, nullable)NSString *text;

/**
  * Duration of recognized speech in ticks.
  * A single tick represents one hundred nanoseconds or one ten-millionth of a second.
  */
@property (readonly)uint64_t duration;

/**
  * Offset of recognized speech in milliseconds.
  * A single tick represents one hundred nanoseconds or one ten-millionth of a second.
  */
@property (readonly)uint64_t offset;

/**
 *  The set of properties exposed in the result.
 */
@property (readonly, nullable)id <SPXPropertyCollection> properties;

@end


/**
  * Defines detailed information about why a result was canceled.
  */
SPX_EXPORT
@interface SPXCancellationDetails : NSObject

/**
  * The reason the recognition was canceled.
  */
@property (readonly)SPXCancellationReason reason;

/**
  * The error code in case of an unsuccessful recognition (reason is set to Error).
  * Added in version 1.1.0.
  */
@property (readonly)SPXCancellationErrorCode errorCode;

/**
  * The error message in case of an unsuccessful recognition (reason is set to Error).
  */
@property (copy, readonly, nullable)NSString *errorDetails;

/**
  * Creates an instance of SPXCancellationDetails object for the canceled SPXSpeechRecognitionResult.
  * @param recognitionResult The result that was canceled.
  * @return The cancellation details object being created.
  */
-(nullable instancetype)initFromCanceledRecognitionResult:(nonnull SPXRecognitionResult *)recognitionResult;
@end


/**
  * Defines detailed information for NoMatch recognition results.
  */
SPX_EXPORT
@interface SPXNoMatchDetails : NSObject

/**
  * The reason why NoMatch is returned.
  */
@property (readonly)SPXNoMatchReason reason;

/**
  * Initializes an instance of SPXNoMatchDetails object from a NoMatch result.
  * @param recognitionResult recognition result that has NoMatch returned in Reason.
  * @return an SPXNoMatchDetails instance.
  */
-(nullable instancetype)initFromNoMatchRecognitionResult:(nonnull SPXRecognitionResult *)recognitionResult;

@end
