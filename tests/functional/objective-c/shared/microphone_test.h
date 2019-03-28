//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef microphone_test_h
#define microphone_test_h

#import <Foundation/Foundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface MicrophoneTest : NSObject

+(void) runAsync:(NSString *)speechKey withRegion:(NSString *)serviceRegion;
+(void) runTranslation:(NSString *)speechKey withRegion:(NSString *)serviceRegion;
+(void) runContinuous:(NSString *)speechKey withRegion:(NSString *)serviceRegion;

@end

#endif /* microphone_test_h */
