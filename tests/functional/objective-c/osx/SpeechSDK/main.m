//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "../../shared/microphone_test.h"

NSString *speechKey = @"YourSubscriptionKey";
NSString *serviceRegion = @"YourServiceRegion";
NSString *intentKey = @"YourLanguageUnderstandingSubscriptionKey";
NSString *intentRegion = @"YourLanguageUnderstandingServiceRegion";

int main(int argc, char * argv[]) {
    @autoreleasepool {
        [MicrophoneTest runAsync:speechKey withRegion:serviceRegion];
        [MicrophoneTest runTranslation:speechKey withRegion:serviceRegion];
        [MicrophoneTest runContinuous:speechKey withRegion:serviceRegion];
        return 0;
    }
}
