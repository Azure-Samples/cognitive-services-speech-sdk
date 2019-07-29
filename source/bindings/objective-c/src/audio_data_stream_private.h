//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#import "SPXAudioDataStream.h"
#import "common_private.h"

@interface SPXAudioDataStream (Private)

- (std::shared_ptr<SpeechImpl::AudioDataStream>)getHandle;

@end