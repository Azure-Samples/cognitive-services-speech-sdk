//
//  main.m
//  Mictest
//
//  Created by carbon on 16.09.18.
//  Copyright © 2018 MSFT. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
#import "microphone_test.h"

int main(int argc, char * argv[]) {
    @autoreleasepool {
        [MicrophoneTest runAsync];
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
