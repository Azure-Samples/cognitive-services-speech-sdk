//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_config_private_h
#define speech_config_private_h

#import "speech_config.h"
#import "common_private.h"

@interface SpeechConfig (Private)

- (instancetype)init: (std::shared_ptr<SpeechImpl::SpeechConfig>)handle;
- (std::shared_ptr<SpeechImpl::SpeechConfig>)getHandle;

@end

#endif /* speech_config_private_h */
