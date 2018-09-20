//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXTranslationRecognitionEventArgs.h"
#import "common_private.h"

@interface SPXTranslationRecognitionEventArgs (Private)

- (instancetype)init: (const TranslationImpl::TranslationRecognitionEventArgs&)e;

@end

@interface SPXTranslationRecognitionCanceledEventArgs (Private)

- (instancetype)init: (const TranslationImpl::TranslationRecognitionCanceledEventArgs&)e;

@end
