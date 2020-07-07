//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

@implementation SPXConnection
{
    ConnectionSharedPtr connectionHandle;
}

- (instancetype)initFromRecognizer:(SPXRecognizer *)recognizer {
     try {
        auto connectionImpl = SpeechImpl::Connection::FromRecognizer([recognizer getHandle]);
        if (connectionImpl == nullptr)
            return nil;
        return [self initWithImpl:connectionImpl];
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (...) {
        NSLog(@"Exception caught when creating SPXConnection in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initFromRecognizer:(nonnull SPXRecognizer *)recognizer error:(NSError * _Nullable * _Nullable)outError {
    try {
        self = [self initFromRecognizer:recognizer];
        return self;
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]]
                                               userInfo:errorDict];
    }
    return nil;
}

- (instancetype)initFromConversationTranslator:(SPXConversationTranslator *)translator {
     try {
        auto connectionImpl = SpeechImpl::Connection::FromConversationTranslator([translator getHandle]);
        if (connectionImpl == nullptr)
            return nil;
        return [self initWithImpl:connectionImpl];
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (...) {
        NSLog(@"Exception caught when creating SPXConnection in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initFromConversationTranslator:(nonnull SPXConversationTranslator *)translator error:(NSError * _Nullable * _Nullable)outError {
    try {
        self = [self initFromConversationTranslator:translator];
        return self;
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]]
                                               userInfo:errorDict];
    }
    return nil;
}

- (instancetype)initWithImpl:(ConnectionSharedPtr)connectionImpl
{
    self = [super init];
    self->connectionHandle = connectionImpl;
    if (!self || self->connectionHandle == nullptr) {
        NSLog(@"connectionHandle is nil in SPXConnection constructor");
        return nil;
    }
    else
    {
        return self;
    }
}

- (void)dealloc {
    LogDebug(@"connection object deallocated.");
    if (!self->connectionHandle)
    {
        NSLog(@"connectionHandle is nil in SPXConnection destructor");
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

- (void)addConnectedEventHandler:(SPXConnectionEventHandler)eventHandler
{
    connectionHandle->Connected.Connect(^(const Microsoft::CognitiveServices::Speech::ConnectionEventArgs & evt) {
        SPXConnectionEventArgs *eventArgs = [[SPXConnectionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addDisconnectedEventHandler:(SPXConnectionEventHandler)eventHandler
{
    connectionHandle->Disconnected.Connect(^(const Microsoft::CognitiveServices::Speech::ConnectionEventArgs & evt) {
        SPXConnectionEventArgs *eventArgs = [[SPXConnectionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)setMessageProperty:(NSString *)path propertyName:(NSString *)propertyName propertyValue:(NSString *)propertyValue
{
    if (connectionHandle == nullptr) {
        NSLog(@"connectionHandle handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"connectionHandle handle is null"
                                                       userInfo:nil];
        [exception raise];
    }
    connectionHandle->SetMessageProperty([path toSpxString], [propertyName toSpxString], [propertyValue toSpxString]);
}

- (void)sendMessage:(NSString *)path payload:(NSString *)payload
{
    if (connectionHandle == nullptr) {
        NSLog(@"connectionHandle handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"connectionHandle handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        connectionHandle->SendMessageAsync([path toSpxString], [payload toSpxString]).get();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (...) {
        NSLog(@"Exception caught in sendMessage");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
}

- (BOOL)sendMessage:(NSString *)path payload:(NSString *)payload error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self sendMessage:path payload:payload];
        return TRUE;
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return FALSE;
}

@end
