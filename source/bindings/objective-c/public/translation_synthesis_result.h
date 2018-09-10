//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_synthesis_result_h
#define translation_synthesis_result_h

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, SynthesisStatus)
{
    SynthesisSuccess = 0,
    SynthesisEnd,
    SynthesisError
};

@interface TranslationSynthesisResult : NSObject

@property (readonly) SynthesisStatus status;

@property (readonly) NSData* audio;

@property (readonly) NSString* failureReason;

@end

#endif /* translation_synthesis_result_h */
