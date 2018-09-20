//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXSpeechTranslationConfiguration.h"
#import "common_private.h"

@interface SPXSpeechTranslationConfiguration (Private)

- (instancetype)initWithImpl:(std::shared_ptr<TranslationImpl::SpeechTranslationConfig>)speechTranslationConfigurationImpl;
- (std::shared_ptr<TranslationImpl::SpeechTranslationConfig>)getHandle;

@end
