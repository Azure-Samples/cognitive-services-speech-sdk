//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXAudioStreamFormat.h"
#import "common_private.h"

@interface SPXAudioStreamFormat (Private)

- (std::shared_ptr<AudioImpl::AudioStreamFormat>)getHandle;

@end