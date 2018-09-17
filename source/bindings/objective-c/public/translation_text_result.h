//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_text_result_h
#define translation_text_result_h

#import <Foundation/Foundation.h>
#import "recognition_result.h"

/**
  * Defines translation text result.
  */
@interface TranslationTextResult : RecognitionResult

/**
  * The translation results. Each item in the dictionary represents translation result in one of target languages, where the key
  * is the name of the target language, in BCP-47 format, and the value is the translation text in the specified language.
  * @return the translated text results.
  */
@property (readonly) NSMutableDictionary *translations;

@end

#endif /* translation_text_result */
