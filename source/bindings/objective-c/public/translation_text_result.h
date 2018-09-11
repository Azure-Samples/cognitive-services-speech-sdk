//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_text_result_h
#define translation_text_result_h

#import <Foundation/Foundation.h>
#import "recognition_result_base.h"

typedef NS_ENUM(NSInteger, TranslationStatus)
{
    TranslationSuccess = 0,
    TranslationError
};

@interface TranslationTextResult : RecognitionResultBase

@property (readonly) TranslationStatus translationStatus;

@property (readonly) NSMutableDictionary* translations;

@property (readonly) NSString* failureReason;

// Todo: get property bag

@end

#endif /* translation_text_result */
