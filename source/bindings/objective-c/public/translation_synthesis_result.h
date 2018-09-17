//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_synthesis_result_h
#define translation_synthesis_result_h

#import <Foundation/Foundation.h>

/**
  * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
  */
@interface TranslationSynthesisResult : NSObject

/**
  * The voice output of the translated text in the target language.
  */
@property (readonly) NSData *audio;

@end

#endif /* translation_synthesis_result_h */
