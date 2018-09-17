//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_synthesis_result_event_args_h
#define translation_synthesis_result_event_args_h

#import <Foundation/Foundation.h>
#import "session_event_args.h"
#import "translation_synthesis_result.h"

/**
  * Defines payload of translation synthesis result event.
  */
@interface TranslationSynthesisResultEventArgs : SessionEventArgs

/**
  * The translation synthesis result.
  */
@property (readonly) TranslationSynthesisResult *result;

@end

#endif /* translation_synthesis_result_event_args_h */
