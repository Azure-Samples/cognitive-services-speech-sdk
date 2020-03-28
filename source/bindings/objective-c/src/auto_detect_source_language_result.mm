//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXAutoDetectSourceLanguageResult.h"

@implementation SPXAutoDetectSourceLanguageResult {
    NSString * srcLanguage;
}

- (instancetype)init:(SPXSpeechRecognitionResult *)speechRecognitionResult
{
    self = [super init];
    srcLanguage=[speechRecognitionResult.properties getPropertyById:SPXSpeechServiceConnectionAutoDetectSourceLanguageResult];
    return self;
}


- (NSString *)language
{
    return srcLanguage;
}

@end