//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_text_result_event_args_private_h
#define translation_text_result_event_args_private_h

#import "translation_text_result_event_args.h"

#import "common_private.h"

@interface TranslationRecognitionEventArgs (Private)

- (instancetype)init: (const TranslationImpl::TranslationRecognitionEventArgs&)e;

@end

@interface TranslationRecognitionCanceledEventArgs (Private)

- (instancetype)init: (const TranslationImpl::TranslationRecognitionCanceledEventArgs&)e;

@end

#endif /* translation_text_result_event_args_private_h */
