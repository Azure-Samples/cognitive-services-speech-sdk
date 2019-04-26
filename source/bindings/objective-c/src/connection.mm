//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

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
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        // [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        // [exception raise];
    }
    catch (...) {
        NSLog(@"Exception caught when creating SPXConnection in core.\nNOTE: This will raise an exception in the future!");
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

- (void)dealloc {
    LogDebug(@"connection object deallocated.");
    if (!self->connectionHandle)
    {
        NSLog(@"connectionHandle is nil in speech recognizer destructor");
        return;
    }
    
    try 
    {
        connectionHandle->Connected.DisconnectAll();
        connectionHandle->Disconnected.DisconnectAll();
        connectionHandle.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...)
    {
        NSLog(@"Exception caught in Connection destructor");
    }
}

- (void)open:(BOOL)forContinuousRecognition
{
    connectionHandle->Open(forContinuousRecognition);
}

- (void)close
{
    connectionHandle->Close();
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
