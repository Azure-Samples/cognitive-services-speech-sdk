//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "../../shared/test.h"
#import "../../shared/test_audio.h"
#import "../../shared/microphone_test.h"

int main(int argc, char * argv[]) {
    @autoreleasepool {
        //return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        // [EndToEndTests runTest];
        // [MicrophoneTest runAsync];
        // [MicrophoneTest runTranslation];
        [MicrophoneTest runContinuous];
        // [AudioStreamTest run];
        return 0;
    }
}
