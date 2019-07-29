//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXAudioStream.h"
#import "common_private.h"

@interface SPXAudioInputStream (Private)

- (instancetype)init: (std::shared_ptr<AudioImpl::AudioInputStream>)handle;
- (std::shared_ptr<AudioImpl::AudioInputStream>)getHandle;

@end

@interface SPXPushAudioInputStream (Private)

- (instancetype)initWithImpl: (std::shared_ptr<AudioImpl::PushAudioInputStream>)handle;

@end

@interface SPXPullAudioInputStream (Private)

- (instancetype)initWithImpl: (std::shared_ptr<AudioImpl::PullAudioInputStream>)handle;

@end

@interface SPXAudioOutputStream (Private)

- (instancetype)init: (std::shared_ptr<AudioImpl::AudioOutputStream>)handle;
- (std::shared_ptr<AudioImpl::AudioOutputStream>)getHandle;

@end

@interface SPXPullAudioOutputStream (Private)

- (instancetype)initWithImpl: (std::shared_ptr<AudioImpl::PullAudioOutputStream>)handle;

@end

@interface SPXPushAudioOutputStream (Private)

- (instancetype)initWithImpl: (std::shared_ptr<AudioImpl::PushAudioOutputStream>)handle;

@end