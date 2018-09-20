//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation SPXRecognizer
{
    dispatch_queue_t dispatchQueue;
    NSMutableArray *sessionStartedEventHandlerList;
    NSMutableArray *sessionStoppedEventHandlerList;
    NSLock *sessionEventArrayLock;
    NSMutableArray *speechStartDetectedEventHandlerList;
    NSMutableArray *speechEndDetectedEventHandlerList;
    NSLock *speechDetectionEventArrayLock;
}

- (instancetype)initFrom:(RecognizerSharedPtr)recoHandle withParameters:(SpeechImpl::PropertyCollection *)propertiesHandle
{
    self = [super init];
    _properties = [[RecognizerPropertyCollection alloc] initWithPropertyCollection:propertiesHandle from:recoHandle];
    sessionStartedEventHandlerList = [NSMutableArray array];
    sessionStoppedEventHandlerList = [NSMutableArray array];
    sessionEventArrayLock = [[NSLock alloc] init];
    speechStartDetectedEventHandlerList = [NSMutableArray array];
    speechEndDetectedEventHandlerList = [NSMutableArray array];
    speechDetectionEventArrayLock = [[NSLock alloc] init];
    
    return self;
}

- (void)setDispatchQueue: (dispatch_queue_t)queue
{
    dispatchQueue = queue;
}

- (void)onSessionStartedEvent:(SPXSessionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnSessionStartedEvent");
    NSArray* workCopyOfList;
    [sessionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:sessionStartedEventHandlerList];
    [sessionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXSessionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)onSessionStoppedEvent:(SPXSessionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnSessionStoppedEvent");
    NSArray* workCopyOfList;
    [sessionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:sessionStoppedEventHandlerList];
    [sessionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXSessionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)addSessionStartedEventHandler:(SPXSessionEventHandler)eventHandler
{
    [sessionEventArrayLock lock];
    [sessionStartedEventHandlerList addObject:eventHandler];
    [sessionEventArrayLock unlock];
}

- (void)addSessionStoppedEventHandler:(SPXSessionEventHandler)eventHandler
{
    [sessionEventArrayLock lock];
    [sessionStoppedEventHandlerList addObject:eventHandler];
    [sessionEventArrayLock unlock];
}

- (void)onSpeechStartDetectedEvent:(SPXRecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnSpeechStartDetectedEvent");
    NSArray* workCopyOfList;
    [speechDetectionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:speechStartDetectedEventHandlerList];
    [speechDetectionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXRecognitionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)onSpeechEndDetectedEvent:(SPXRecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnSpeechEndDetectedEvent");
    NSArray* workCopyOfList;
    [speechDetectionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:speechEndDetectedEventHandlerList];
    [speechDetectionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXRecognitionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)addSpeechStartDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
    [speechDetectionEventArrayLock lock];
    [speechStartDetectedEventHandlerList addObject:eventHandler];
    [speechDetectionEventArrayLock unlock];
}

- (void)addSpeechEndDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
    [speechDetectionEventArrayLock lock];
    [speechEndDetectedEventHandlerList addObject:eventHandler];
    [speechDetectionEventArrayLock unlock];
}

@end
