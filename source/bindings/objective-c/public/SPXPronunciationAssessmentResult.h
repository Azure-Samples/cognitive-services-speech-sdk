//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSpeechRecognitionResult.h"

/**
 * Represents pronunciation assessment result
 * Added in version 1.14.0
 */
SPX_EXPORT
@interface SPXPronunciationAssessmentResult : NSObject

/**
 * The pronunciation accuracy of the given speech, which indicates
 * how closely the phonemes match a native speaker's pronunciation.
 */
@property (readonly) double accuracyScore;

/**
 * The overall score indicating the pronunciation quality of the given speech.
 * This is calculated from AccuracyScore, FluencyScore and CompletenessScore with weight.
 */
@property (readonly)double pronunciationScore;

/**
 * The score indicating the completeness of the given speech by calculating the ratio of pronounced words towards entire input.
 */
@property (readonly)double completenessScore;

/**
 * The score indicating the fluency of the given speech.
 */
@property (readonly)double fluencyScore;

/**
 * Initializes an SPXPronunciationAssessmentResult object using SPXSpeechRecognitionResult.
 *
 * @param speechRecognitionResult the speech recognition result
 *
 * @return an instance of auto detection source language result.
 */

- (nullable instancetype)init:(nonnull SPXSpeechRecognitionResult *)speechRecognitionResult;


@end