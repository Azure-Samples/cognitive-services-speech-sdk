//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef audio_stream_format_private_h
#define audio_stream_format_private_h

#import "audio_stream_format.h"
#import "common_private.h"

@interface AudioStreamFormat (Private)

- (instancetype)init: (std::shared_ptr<AudioImpl::AudioStreamFormat>)handle;
- (std::shared_ptr<AudioImpl::AudioStreamFormat>)getHandle;

@end

#endif /* audio_config_private_h */
