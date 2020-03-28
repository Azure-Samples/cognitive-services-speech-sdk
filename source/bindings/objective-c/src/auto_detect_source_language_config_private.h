//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#import "SPXAutoDetectSourceLanguageConfiguration.h"
#import "common_private.h"

@interface SPXAutoDetectSourceLanguageConfiguration (Private)

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::AutoDetectSourceLanguageConfig>) configImpl;
- (std::shared_ptr<SpeechImpl::AutoDetectSourceLanguageConfig>)getHandle;

@end