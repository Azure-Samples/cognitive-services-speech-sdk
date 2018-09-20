//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXTranslationSynthesisEventArgs.h"
#import "common_private.h"

@interface SPXTranslationSynthesisEventArgs (Private)

- (instancetype)init: (const TranslationImpl::TranslationSynthesisEventArgs&)e;

@end
