//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "nsstring_stdstring.h"

@implementation NSString (STL)

- (std::string)toSpxString
{
    return [self UTF8String];
}

+ (instancetype)StringWithStdString:(const std::string&)str
{
    return [[NSString alloc] initWithUTF8String: str.c_str()];
}

@end
