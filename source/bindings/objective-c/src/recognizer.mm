//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation Recognizer
{
    dispatch_queue_t dispatchQueue;
    NSMutableArray *sessionStartedEventListenerList;
    NSMutableArray *sessionStoppedEventListenerList;
    NSLock *sessionEventArrayLock;
    NSMutableArray *speechStartDetectedEventListenerList;
    NSMutableArray *speechEndDetectedEventListenerList;
    NSLock *speechDetectionEventArrayLock;
}

- (instancetype)initFrom:(RecognizerSharedPtr)recoHandle withParameters:(SpeechImpl::PropertyCollection *)propertiesHandle
{
    self = [super init];
    _properties = [[RecognizerPropertyCollection alloc] initWithPropertyCollection:propertiesHandle from:recoHandle];
    sessionStartedEventListenerList = [NSMutableArray array];
    sessionStoppedEventListenerList = [NSMutableArray array];
    sessionEventArrayLock = [[NSLock alloc] init];
    speechStartDetectedEventListenerList = [NSMutableArray array];
    speechEndDetectedEventListenerList = [NSMutableArray array];
    speechDetectionEventArrayLock = [[NSLock alloc] init];
    
    return self;
}

- (void)setDispatchQueue: (dispatch_queue_t)queue
{
    dispatchQueue = queue;
}

- (void)onSessionStartedEvent:(SessionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnSessionStartedEvent");
    NSArray* workCopyOfList;
    [sessionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:sessionStartedEventListenerList];
    [sessionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SessionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onSessionStoppedEvent:(SessionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnSessionStoppedEvent");
    NSArray* workCopyOfList;
    [sessionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:sessionStoppedEventListenerList];
    [sessionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SessionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)addSessionStartedEventListener:(SessionEventHandlerBlock)eventHandler
{
    [sessionEventArrayLock lock];
    [sessionStartedEventListenerList addObject:eventHandler];
    [sessionEventArrayLock unlock];
}

- (void)addSessionStoppedEventListener:(SessionEventHandlerBlock)eventHandler
{
    [sessionEventArrayLock lock];
    [sessionStoppedEventListenerList addObject:eventHandler];
    [sessionEventArrayLock unlock];
}

- (void)onSpeechStartDetectedEvent:(RecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnSpeechStartDetectedEvent");
    NSArray* workCopyOfList;
    [speechDetectionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:speechStartDetectedEventListenerList];
    [speechDetectionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((RecognitionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onSpeechEndDetectedEvent:(RecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC OnSpeechEndDetectedEvent");
    NSArray* workCopyOfList;
    [speechDetectionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:speechEndDetectedEventListenerList];
    [speechDetectionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((RecognitionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)addSpeechStartDetectedEventListener:(RecognitionEventHandlerBlock)eventHandler
{
    [speechDetectionEventArrayLock lock];
    [speechStartDetectedEventListenerList addObject:eventHandler];
    [speechDetectionEventArrayLock unlock];
}

- (void)addSpeechEndDetectedEventListener:(RecognitionEventHandlerBlock)eventHandler
{
    [speechDetectionEventArrayLock lock];
    [speechEndDetectedEventListenerList addObject:eventHandler];
    [speechDetectionEventArrayLock unlock];
}

@end
