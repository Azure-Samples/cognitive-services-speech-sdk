//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface EndToEndTests : NSObject

+(void)runTest:(NSString *)speechKey withRegion:(NSString *)serviceRegion withIntentKey:(NSString *)intentKey withIntentRegion:(NSString*)intentRegion;

@end

