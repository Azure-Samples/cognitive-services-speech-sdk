//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXRecognizer.h"
#import "SPXConnectionEventArgs.h"

/**
 * Connection is a proxy class for managing connection to the speech service of the specified Recognizer.
 * By default, a Recognizer autonomously manages connection to service when needed. 
 * The Connection class provides additional methods for users to explicitly open or close a connection and 
 * to subscribe to connection status changes.
 * The use of Connection is optional, and mainly for scenarios where fine tuning of application
 * behavior based on connection status is needed. Users can optionally call Open() to manually set up a connection 
 * in advance before starting recognition on the Recognizer associated with this Connection. After starting recognition,
 * calling Open() or Close() might fail, depending on the process state of the Recognizer. But this does not affect 
 * the state of the associated Recognizer. And if the Recognizer needs to connect or disconnect to service, it will 
 * setup or shutdown the connection independently. In this case the Connection will be notified by change of connection 
 * status via Connected/Disconnected events.
 * Added in version 1.2.0.
 */
SPX_EXPORT
@interface SPXConnection : NSObject

typedef void (^SPXConnectionEventHandler)(SPXConnection* _Nonnull, SPXConnectionEventArgs * _Nonnull);

/**
 * Gets the Connection instance from the specified recognizer.
 * @param recognizer The recognizer associated with the connection.
 * @return The Connection instance of the recognizer.
 */
- (nullable instancetype)initFromRecognizer:(nonnull SPXRecognizer *)recognizer;

/**
 * Starts to set up connection to the service.
 * Users can optionally call Open() to manually set up a connection in advance before starting recognition on the 
 * Recognizer associated with this Connection. After starting recognition, calling Open() might fail, depending on 
 * the process state of the Recognizer. But the failure does not affect the state of the associated Recognizer.
 * Note: On return, the connection might not be ready yet. Please subscribe to the Connected event to
 * be notfied when the connection is established.
 * @param forContinuousRecognition indicates whether the connection is used for continuous recognition or single-shot recognition.
 */
- (void)open:(BOOL)forContinuousRecognition;

/**
 * Closes the connection the service.
 * Users can optionally call Close() to manually shutdown the connection of the associated Recognizer. The call
 * might fail, depending on the process state of the Recognizer. But the failure does not affect the state of the 
 * associated Recognizer.
 */
- (void)close;

/**
 * Subscribes to the Connected event which indicates that the recognizer is connected to service.
 * In order to receive the Connected event after subscribing to it, the Connection object itself needs to be alive.
 * If the Connection object owning this event is out of its life time, all subscribed events won't be delivered.
 */
- (void)addConnectedEventHandler:(nonnull SPXConnectionEventHandler)eventHandler;

/**
 * Subscribe to the Diconnected event which indicates that the recognizer is disconnected from service.
 * In order to receive the Disonnected event after subscribing to it, the Connection object itself needs to be alive.
 * If the Connection object owning this event is out of its life time, all subscribed events won't be delivered.
 */
- (void)addDisconnectedEventHandler:(nonnull SPXConnectionEventHandler)eventHandler;

@end
