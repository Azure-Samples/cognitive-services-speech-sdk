//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_result_h
#define recognition_result_h

#import <Foundation/Foundation.h>
#import "speech_enums.h"
#import "property_collection.h"

/**
  * Represents recognition result base class.
  */
@interface RecognitionResult : NSObject

/**
  * The result identifier.
  */
@property (readonly) NSString *resultId;

/**
  * The reason the result was created.
  */
@property (readonly) ResultReason reason;

/**
  * The recognized text in the result.
  */
@property (readonly) NSString *text;

/**
  * Duration of recognized speech in ticks.
  * A single tick represents one hundred nanoseconds or one ten-millionth of a second.
  */
@property (readonly) NSUInteger duration;

/**
  * Offset of recognized speech in milliseconds.
  * A single tick represents one hundred nanoseconds or one ten-millionth of a second.
  */
@property (readonly) NSUInteger offset;

/**
 *  The set of properties exposed in the result.
 */
@property (readonly) NSObject<PropertyCollection> *properties;

@end


/**
  * Defines detailed information about why a result was canceled.
  */
@interface CancellationDetails : NSObject

/**
  * The reason the recognition was canceled.
  */
@property (readonly) CancellationReason reason;

/**
 * The error details of why the cancellation occurred.
 */
@property (readonly) NSString *errorDetails;

/**
  * Creates an instance of CancellationDetails object for the canceled SpeechRecognitionResult.
  * @param recognitionResult The result that was canceled.
  * @return The cancellation details object being created.
  */
+(CancellationDetails *) fromResult:(RecognitionResult *)recognitionResult;
@end


/**
  * Defines detailed information for NoMatch recognition results.
  */
@interface NoMatchDetails : NSObject

/**
  * The reason why NoMatch is returned.
  */
@property (readonly)NoMatchReason reason;

/**
  * Creates an instance of NoMatchDetails object from a NoMatch result.
  * @param recognitionResult recognition result that has NoMatch returned in Reason.
  * @return The NoMatchDetails object being created.
  */
+(NoMatchDetails *) fromResult:(RecognitionResult *)recognitionResult;

@end

#endif /* recognition_result_h */
