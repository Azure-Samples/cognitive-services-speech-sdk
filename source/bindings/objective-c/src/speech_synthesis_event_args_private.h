//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXSpeechSynthesisEventArgs.h"
#import "common_private.h"

@interface SPXSpeechSynthesisEventArgs (Private)

- (instancetype)init: (const SpeechImpl::SpeechSynthesisEventArgs&)e;

@end

@interface SPXSpeechSynthesisWordBoundaryEventArgs (Private)

- (instancetype)init: (const SpeechImpl::SpeechSynthesisWordBoundaryEventArgs&)e;

@end
