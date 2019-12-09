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
 * The use of Connection is optional. It is intended for scenarios where fine tuning of application
 * behavior based on connection status is needed. Users can optionally call Open() to manually 
 * initiate a service connection before starting recognition on the Recognizer associated with this Connection. 
 * After starting a recognition, calling Open() or Close() might fail. This will not impact
 * the Recognizer or the ongoing recognition. Connection might drop for various reasons, the Recognizer will 
 * always try to reinstitute the connection as required to guarantee ongoing operations. In all these cases 
 * Connected/Disconnected events will indicate the change of the connection status.
 *
 * Added in version 1.2.0.
 */
SPX_EXPORT
@interface SPXConnection : NSObject

typedef void (^SPXConnectionEventHandler)(SPXConnection* _Nonnull, SPXConnectionEventArgs * _Nonnull);

/**
 * Gets the Connection instance from the specified recognizer.
 *
 * @param recognizer The recognizer associated with the connection.
 * @return The Connection instance of the recognizer.
 */
- (nullable instancetype)initFromRecognizer:(nonnull SPXRecognizer *)recognizer
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Gets the Connection instance from the specified recognizer.
 *
 * Added in version 1.6.0.
 *
 * @param recognizer The recognizer associated with the connection.
 * @return The Connection instance of the recognizer.
 * @param outError error information.
 */
- (nullable instancetype)initFromRecognizer:(nonnull SPXRecognizer *)recognizer error:(NSError * _Nullable * _Nullable)outError;

/**
 * Starts to set up connection to the service.
 * Users can optionally call Open() to manually set up a connection in advance before starting recognition on the
 * Recognizer associated with this Connection. After starting recognition, calling Open() might fail, depending on
 * the process state of the Recognizer. But the failure does not affect the state of the associated Recognizer.
 *
 * Note: On return, the connection might not be ready yet. Please subscribe to the Connected event to
 * be notfied when the connection is established.
 *
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

/**
 * Appends a parameter in a message to service.
 * Added in version 1.9.0.
 *
 * @param path The path of the message.
 * @param propertyName The propertyName of the message.
 * @param propertyValue The propertyValue of the message as a json string.
 */
- (void)setMessageProperty:(nonnull NSString *)path propertyName:(nonnull NSString *)propertyName propertyValue:(nonnull NSString *)propertyValue;

/**
 * Send message to service.
 * Added in version 1.9.0.
 *
 * @param path The path of the message.
 * @param payload The payload of the message as a json string.
 */
- (void)sendMessage:(nonnull NSString *)path payload:(nonnull NSString *)payload
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Send message to service.
 * Added in version 1.9.0.
 *
 * @param path The path of the message.
 * @param payload The payload of the message as a json string.
 * @return The boolean value indicating successful operation.
 * @param outError error information.
 */
- (BOOL)sendMessage:(nonnull NSString *)path payload:(nonnull NSString *)payload error:(NSError * _Nullable * _Nullable)outError;

@end
