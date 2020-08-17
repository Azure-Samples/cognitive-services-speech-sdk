//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXPronunciationAssessmentResult.h"
#import "speechapi_private.h"
#import "common_private.h"

@implementation SPXPronunciationAssessmentResult {
}

- (nullable instancetype)init:(SPXSpeechRecognitionResult *)speechRecognitionResult
{
    if(![speechRecognitionResult.properties getPropertyByName:@"AccuracyScore"])
    {
        return nil;
    }

    self = [super init];

    _accuracyScore = [[speechRecognitionResult.properties getPropertyByName:@"AccuracyScore"] doubleValue];
    _pronunciationScore = [[speechRecognitionResult.properties getPropertyByName:@"PronScore"] doubleValue];
    _completenessScore = [[speechRecognitionResult.properties getPropertyByName:@"CompletenessScore"] doubleValue];
    _fluencyScore = [[speechRecognitionResult.properties getPropertyByName:@"FluencyScore"] doubleValue];

    return self;
}

@end
