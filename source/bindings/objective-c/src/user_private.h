//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXUser.h"
#import "common_private.h"

@interface SPXUser (Private)

- (instancetype)initWithImpl:(UserSharedPtr)userHandle;

- (instancetype)initWithError:(NSString *)message;

- (UserSharedPtr)getHandle;

@end
