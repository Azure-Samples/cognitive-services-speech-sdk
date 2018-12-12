//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

struct ConnectionEventHandlerHelper
{
    SPXConnection *connection;
    ConnectionSharedPtr connectionImpl;

    ConnectionEventHandlerHelper(SPXConnection *connection, ConnectionSharedPtr connectionHandle)
    {
        this->connection = connection;
        this->connectionImpl = connectionHandle;
    }

    void addConnectedEventHandler()
    {
        LogDebug(@"Add ConnectedEventHandler");
        connectionImpl->Connected.Connect([this] (const SpeechImpl::ConnectionEventArgs& e)
            {
                SPXConnectionEventArgs *eventArgs = [[SPXConnectionEventArgs alloc] init:e];
                [connection onConnectedEvent: eventArgs];
            });
    }
    
    void addDisconnectedEventHandler()
    {
        LogDebug(@"Add DisconnectedEventHandler");
        connectionImpl->Disconnected.Connect([this] (const SpeechImpl::ConnectionEventArgs& e)
            {
                SPXConnectionEventArgs *eventArgs = [[SPXConnectionEventArgs alloc] init:e];
                [connection onDisconnectedEvent: eventArgs];
            });
    }
};

@implementation SPXConnection
{
    ConnectionSharedPtr connectionHandle;
    dispatch_queue_t dispatchQueue;

    NSMutableArray *connectedEventHandlerList;
    NSMutableArray *disconnectedEventHandlerList;
    NSLock *connectionEventArrayLock;

    struct ConnectionEventHandlerHelper *eventImpl;
}

- (instancetype)initFromRecognizer:(SPXRecognizer *)recognizer {
     try {
        auto connectionImpl = SpeechImpl::Connection::FromRecognizer([recognizer getHandle]);
        if (connectionImpl == nullptr)
            return nil;
        dispatch_queue_t recognizerQueue = [recognizer getDispatchQueue];
        if (recognizerQueue == nullptr)
            return nil;
        return [self initWithImpl:connectionImpl AndDispatchQueue:recognizerQueue];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating SPXConnection in core.");
    }
    return nil;
}

- (instancetype)initWithImpl:(ConnectionSharedPtr)connectionImpl AndDispatchQueue:(dispatch_queue_t)queue
{
    self = [super init];
    self->connectionHandle = connectionImpl;
    self->dispatchQueue = queue;
    if (!self || self->connectionHandle == nullptr || self->dispatchQueue == nullptr) {
        return nil;
    }
    else
    {
        connectedEventHandlerList = [NSMutableArray array];
        disconnectedEventHandlerList = [NSMutableArray array];
        connectionEventArrayLock = [[NSLock alloc] init];

        eventImpl = new ConnectionEventHandlerHelper(self, self->connectionHandle);
        eventImpl->addConnectedEventHandler();
        eventImpl->addDisconnectedEventHandler();

        return self;
    }
}

- (void)onConnectedEvent:(SPXConnectionEventArgs *)eventArgs
{
    LogDebug(@"OBJC OnConnectedEvent");
    NSArray* workCopyOfList;
    [connectionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:connectedEventHandlerList];
    [connectionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXConnectionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)onDisconnectedEvent:(SPXConnectionEventArgs *)eventArgs
{
    LogDebug(@"OBJC OnDisconnectedEvent");
    NSArray* workCopyOfList;
    [connectionEventArrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:disconnectedEventHandlerList];
    [connectionEventArrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXConnectionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)addConnectedEventHandler:(SPXConnectionEventHandler)eventHandler
{
    [connectionEventArrayLock lock];
    [connectedEventHandlerList addObject:eventHandler];
    [connectionEventArrayLock unlock];
}

- (void)addDisconnectedEventHandler:(SPXConnectionEventHandler)eventHandler
{
    [connectionEventArrayLock lock];
    [disconnectedEventHandlerList addObject:eventHandler];
    [connectionEventArrayLock unlock];
}

@end
