//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_text_result_private_h
#define translation_text_result_private_h

#import "translation_text_result.h"
#import "recognition_result_private.h"
#import "common_private.h"

@interface TranslationTextResult (Private)

- (instancetype)init :(std::shared_ptr<TranslationImpl::TranslationTextResult>)resultHandle;

@end

#endif /* translation_text_result_private_h */
