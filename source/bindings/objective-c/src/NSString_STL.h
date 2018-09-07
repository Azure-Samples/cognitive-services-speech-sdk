//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


#ifndef NSString_STL_h
#define NSString_STL_h

#import <Foundation/Foundation.h>
#import <string>

@interface NSString (STL)

- (std::string)string;
+ (instancetype)stringWithString:(const std::string&)str;

@end

#endif /* NSString_STL_h */
