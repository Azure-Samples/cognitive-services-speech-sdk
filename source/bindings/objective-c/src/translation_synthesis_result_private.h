//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_synthesis_result_private_h
#define translation_synthesis_result_private_h

#import "translation_synthesis_result.h"

#import "common_private.h"

@interface TranslationSynthesisResult (Private)

- (instancetype)init :(std::shared_ptr<TranslationImpl::TranslationSynthesisResult>)resultHandle;

- (instancetype)initWithError:(NSString *)message;

@end

#endif /* translation_synthesis_result_private_h */
