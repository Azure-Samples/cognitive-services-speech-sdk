//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef audio_stream_private_h
#define audio_stream_private_h

#import "audio_stream.h"
#import "common_private.h"

@interface AudioInputStream (Private)

- (instancetype)init: (std::shared_ptr<AudioImpl::AudioInputStream>)handle;
- (std::shared_ptr<AudioImpl::AudioInputStream>)getHandle;

@end

@interface PushAudioInputStream (Private)

- (instancetype)init: (std::shared_ptr<AudioImpl::PushAudioInputStream>)handle;

@end

@interface PullAudioInputStream (Private)

- (instancetype)init: (std::shared_ptr<AudioImpl::PullAudioInputStream>)handle;

@end

#endif /* audio_stream_private_h */
