//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#import "SPXAudioConfiguration.h"
#import "common_private.h"

@interface SPXAudioConfiguration (Private)

- (std::shared_ptr<AudioImpl::AudioConfig>)getHandle;

@end