//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "recognizer_private.h"

#import "common_private.h"

@implementation Recognizer
{
    dispatch_queue_t dispatchQueue;
    NSMutableArray *sessionEventListenerList;
    NSMutableArray *recognitionEventListenerList;
    NSLock *sessionEventArrayLock;
    NSLock *recognitionEventArrayLock;
}

- (instancetype)init
{
    sessionEventListenerList = [NSMutableArray array];
    recognitionEventListenerList = [NSMutableArray array];
    sessionEventArrayLock = [[NSLock alloc] init];
    recognitionEventArrayLock = [[NSLock alloc] init];
    return self;
}

- (void)setDispatchQueue: (dispatch_queue_t)queue
{
    dispatchQueue = queue;
}

- (void)onSessionEvent:(SessionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnSessionEvent");
    [sessionEventArrayLock lock];
    for (id handle in sessionEventListenerList) {
        dispatch_async(dispatchQueue, ^{
            ((SessionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
    [sessionEventArrayLock unlock];
}

- (void)addSessionEventListener:(SessionEventHandlerBlock)eventHandler
{
    [sessionEventArrayLock lock];
    [sessionEventListenerList addObject:eventHandler];
    [sessionEventArrayLock unlock];
}

- (void)removeSessionEventListener:(SessionEventHandlerBlock)eventHandler
{
    [sessionEventArrayLock lock];
    [sessionEventListenerList removeObject:eventHandler];
    [sessionEventArrayLock unlock];
    return;
}

- (void)onRecognitionEvent:(RecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnRecognitionEvent");
    [recognitionEventArrayLock lock];
    for (id handle in recognitionEventListenerList) {
        dispatch_async(dispatchQueue, ^{
            ((RecognitionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
    [recognitionEventArrayLock unlock];
}

- (void)addRecognitionEventListener:(RecognitionEventHandlerBlock)eventHandler
{
    [recognitionEventArrayLock lock];
    [recognitionEventListenerList addObject:eventHandler];
    [recognitionEventArrayLock unlock];
    return;
}

- (void)removeRecognitionEventListener:(RecognitionEventHandlerBlock)eventHandler
{
    [recognitionEventArrayLock lock];
    [sessionEventListenerList removeObject:eventHandler];
    [recognitionEventArrayLock unlock];
    return;
}

@end
