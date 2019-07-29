//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "nsstring_stdstring.h"

@implementation NSString (STL)

- (std::string)toSpxString
{
    const char * s = [self UTF8String];
    if (s != nullptr)
    {
        return [self UTF8String];
    }
    else
    {
        return std::string();
    }
}

+ (instancetype)StringWithStdString:(const std::string&)str
{
    return [[NSString alloc] initWithUTF8String: str.c_str()];
}

@end
