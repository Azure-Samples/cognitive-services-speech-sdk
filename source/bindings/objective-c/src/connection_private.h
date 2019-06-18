//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXConnection.h"
#import "common_private.h"

using ConnectionSharedPtr = std::shared_ptr<SpeechImpl::Connection>;

@interface SPXConnection (Private)

- (instancetype)initWithImpl:(ConnectionSharedPtr)connectionHandle;

@end
