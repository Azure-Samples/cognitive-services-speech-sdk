//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXRecognizer.h"
#import "SPXConnectionEventArgs.h"

/**
 * Manages connection of a recognizer.
 * Added in version 1.2.0.
 */
SPX_EXPORT
@interface SPXConnection : NSObject

typedef void (^SPXConnectionEventHandler)(SPXConnection* _Nonnull, SPXConnectionEventArgs * _Nonnull);

/**
  * Initializes a new instance of connection for the specified recognizer.
  * @param recognizer the recognizer associated with the connection.
  * @return an instance of connection object
  */
- (nullable instancetype)initFromRecognizer:(nonnull SPXRecognizer *)recognizer;

/**
  * Subscribes to the Connected event which indicates that the connection to the service is established.
  */
- (void)addConnectedEventHandler:(nonnull SPXConnectionEventHandler)eventHandler;

/**
  * Subscribes to the disconnected event which indicates that the connection to the service is droped.
  */
- (void)addDisconnectedEventHandler:(nonnull SPXConnectionEventHandler)eventHandler;

@end
