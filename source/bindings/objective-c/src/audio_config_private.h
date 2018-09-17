//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef audio_config_private_h
#define audio_config_private_h

#import "audio_config.h"
#import "common_private.h"

@interface AudioConfig (Private)

- (instancetype)init: (std::shared_ptr<AudioImpl::AudioConfig>)audioHandle;
- (std::shared_ptr<AudioImpl::AudioConfig>)getHandle;

@end

#endif /* audio_config_private_h */
