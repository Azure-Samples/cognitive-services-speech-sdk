//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXSessionEventArgs.h"
#import "common_private.h"

@interface SPXSessionEventArgs (Private)

- (instancetype)init: (const SpeechImpl::SessionEventArgs&)eventArgsHandle;

@end