//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "NSString_STL.h"

@implementation NSString (STL)

- (std::string)string
{
    NSData *data = [self dataUsingEncoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF8)];
    std::string value = std::string((char*)[data bytes], [data length]/sizeof(char));
    return value;
}

+ (instancetype)stringWithString:(const std::string&)str
{
    return [[NSString alloc] initWithBytes:str.data() length:str.size()*sizeof(char) encoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF8)];
}

@end
