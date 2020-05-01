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

+ (double) levenshteinSimilarityRatio:(NSString *)stringA withString:(NSString *)stringB 
{
    NSUInteger distance = [self levenshteinDistance:stringA to:stringB];
    NSLog(@"levenshteinDistance: %ld", (long)distance);
    double distanceRatio = (((double)distance) / (double)(stringA.length + stringB.length));
    NSLog(@"similarity: %f", 1.0 - distanceRatio);
    return 1.0 - distanceRatio;
}

+ (NSUInteger) levenshteinDistance:(NSString *)stringA to:(NSString *)stringB  
{
    NSUInteger sl = stringA.length;
    NSUInteger tl = stringB.length;
    NSUInteger *d = calloc(sizeof(*d), (sl+1) * (tl+1));
 
#define d(i, j) d[((j) * sl) + (i)]
    for (NSUInteger i = 0; i <= sl; i++) {
        d(i, 0) = i;
    }
    for (NSUInteger j = 0; j <= tl; j++) {
        d(0, j) = j;
    }
    for (NSUInteger j = 1; j <= tl; j++) {
        for (NSUInteger i = 1; i <= sl; i++) {
            if ([stringA characterAtIndex:i-1] == [stringB characterAtIndex:j-1]) {
                d(i, j) = d(i-1, j-1);
            } else {
                d(i, j) = MIN(d(i-1, j), MIN(d(i, j-1), d(i-1, j-1))) + 1;
            }
        }
    }
 
    NSUInteger r = d(sl, tl);
#undef d
 
    free(d);
 
    return r;
}

@end
