//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "NSString_STL.h"

@implementation NSString (STL)

- (std::wstring)wstring
{
    NSData *data = [self dataUsingEncoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
    std::wstring wstring = std::wstring((wchar_t *)[data bytes], [data length]/sizeof(wchar_t));

    return wstring;
}

+ (instancetype)stringWithWString:(const std::wstring&)str
{
    return [[NSString alloc] initWithBytes:str.data() length:str.size() encoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
}

@end
