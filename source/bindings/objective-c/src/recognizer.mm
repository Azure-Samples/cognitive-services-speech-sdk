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
    NSLock *sessionEventArrayLock;
    NSMutableArray *recognitionEventListenerList;   
    NSLock *recognitionEventArrayLock;
    NSMutableArray *errorEventListenerList;
    NSLock *errorEventArrayLock;
}

- (instancetype)init
{
    sessionEventListenerList = [NSMutableArray array];
    sessionEventArrayLock = [[NSLock alloc] init];
    recognitionEventListenerList = [NSMutableArray array];
    recognitionEventArrayLock = [[NSLock alloc] init];
    errorEventListenerList = [NSMutableArray array];
    errorEventArrayLock = [[NSLock alloc] init];
    
    return self;
}

- (void)setDispatchQueue: (dispatch_queue_t)queue
{
    dispatchQueue = queue;
}

- (void)onSessionEvent:(SessionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnSessionEvent");
    NSArray* workCopyOfList;
    [sessionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:sessionEventListenerList];
    [sessionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SessionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)addSessionEventListener:(SessionEventHandlerBlock)eventHandler
{
    [sessionEventArrayLock lock];
    [sessionEventListenerList addObject:eventHandler];
    [sessionEventArrayLock unlock];
}

- (void)onRecognitionEvent:(RecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnRecognitionEvent");
    NSArray* workCopyOfList;
    [recognitionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognitionEventListenerList];
    [recognitionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((RecognitionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)addRecognitionEventListener:(RecognitionEventHandlerBlock)eventHandler
{
    [recognitionEventArrayLock lock];
    [recognitionEventListenerList addObject:eventHandler];
    [recognitionEventArrayLock unlock];
}

- (void)onErrorEvent:(RecognitionErrorEventArgs *)eventArgs
{
    NSLog(@"OBJC OnErrorEvent");
    NSArray* workCopyOfList;
    [errorEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:errorEventListenerList];
    [errorEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((ErrorEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)addErrorEventListener:(ErrorEventHandlerBlock)eventHandler
{
    [errorEventArrayLock lock];
    [errorEventListenerList addObject:eventHandler];
    [errorEventArrayLock unlock];
}

@end
