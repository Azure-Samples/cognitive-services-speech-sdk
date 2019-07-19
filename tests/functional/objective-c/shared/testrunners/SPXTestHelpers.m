//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "SPXTestHelpers.h"

@implementation SPXTestHelpers
+ (NSString *) normalizeText:(NSString *) input
{
    return [[[input componentsSeparatedByCharactersInSet:[[NSCharacterSet letterCharacterSet] invertedSet]] componentsJoinedByString:@""] lowercaseString];
}
@end
