//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef test_audio_h
#define test_audio_h

#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface AudioStreamTest :NSObject

+(void) runPushTest:(NSString *)speechKey withRegion:(NSString *)region;
+(void) runPullTest:(NSString *)speechKey withRegion:(NSString *)region;

@end

#endif /* test_audio_h */
