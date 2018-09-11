//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_synthesis_event_args_private_h
#define translation_synthesis_event_args_private_h

#import "translation_synthesis_event_args.h"

#import "common_private.h"

@interface TranslationSynthesisEventArgs (Private)

- (instancetype)init: (const TranslationImpl::TranslationSynthesisResultEventArgs&)e;

@end

#endif /* translation_synthesis_event_args_private_h */
