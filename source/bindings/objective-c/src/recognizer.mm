//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation SPXRecognizer
{
    RecognizerSharedPtr recoHandle;
    dispatch_queue_t dispatchQueue;
}

- (instancetype)initWith:(RecognizerSharedPtr)recoImpl withParameters:(SpeechImpl::PropertyCollection *)propertiesHandle
{
    self = [super init];
    recoHandle = recoImpl;
    _properties = [[RecognizerPropertyCollection alloc] initWithPropertyCollection:propertiesHandle from:recoImpl];
    return self;
}

- (RecognizerSharedPtr)getHandle
{
    return recoHandle;
}

- (void)addSessionStartedEventHandler:(SPXSessionEventHandler)eventHandler
{
}

- (void)addSessionStoppedEventHandler:(SPXSessionEventHandler)eventHandler
{
}

- (void)addSpeechStartDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
}

- (void)addSpeechEndDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
}

@end
