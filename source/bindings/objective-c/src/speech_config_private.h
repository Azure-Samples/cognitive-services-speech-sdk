//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXSpeechConfiguration.h"
#import "common_private.h"

@interface SPXSpeechConfiguration (Private)

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::SpeechConfig>)speechConfigImpl;
- (std::shared_ptr<SpeechImpl::SpeechConfig>)getHandle;

@end