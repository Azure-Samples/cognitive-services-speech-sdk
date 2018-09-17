//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_translation_config_private_h
#define speech_translation_config_private_h

#import "speech_translation_config.h"
#import "common_private.h"

@interface SpeechTranslationConfig (Private)

- (instancetype)init: (std::shared_ptr<TranslationImpl::SpeechTranslationConfig>)handle;
- (std::shared_ptr<TranslationImpl::SpeechTranslationConfig>)getHandle;

@end

#endif /* speech_translation_config_private_h */
