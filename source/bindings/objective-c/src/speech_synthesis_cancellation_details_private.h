//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXSpeechSynthesisCancellationDetails.h"
#import "common_private.h"

@interface SPXSpeechSynthesisCancellationDetails (Private)

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::SpeechSynthesisCancellationDetails>)handle;

@end