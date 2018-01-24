//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp.h: the header file includes types, methods and error code exposed by usplib
//

#pragma once

#include <stdint.h>

#include "uspmessages.h"
#include "usperror.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USP_CALLBACK_VERSION 1

/**
 * The UspHandle represents an opaque handle used by usplib.
 */
typedef struct _UspContext* UspHandle;

/**
 * The UspResult represents the result of a function call of usplib.
*/
typedef int UspResult;

/**
 * A callback function that will be called when a speech.startDetected message is received from service.
 * @param uspHandle The UspHandle.
 * @param context A pointer to the application-defined callback context.
 * @param message A pointer to the speech.startDetected message.
*/
typedef void(*UspOnSpeechStartDetected)(UspHandle uspHandle, void* context, UspMsgSpeechStartDetected *message);

/**
* A callback function that will be called when a speech.endDetected message is received from service.
* @param uspHandle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.endDetected message.
*/
typedef void(*UspOnSpeechEndDetected)(UspHandle uspHandle, void* context, UspMsgSpeechEndDetected *message);

/**
* A callback function that will be called when a speech.hypothesis message is received from service.
* @param uspHandle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.hypothesis message.
*/
typedef void(*UspOnSpeechHypothesis)(UspHandle uspHandle, void* context, UspMsgSpeechHypothesis *message);

/**
* A callback function that will be called when a speech.phrase message is received from service.
* @param uspHandle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.phrase message.
*/
typedef void(*UspOnSpeechPhrase)(UspHandle uspHandle, void* context, UspMsgSpeechPhrase *message);

/**
* A callback function that will be called when a speech.fragment message is received from service.
* @param uspHandle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.hypothesis message.
*/
typedef void(*UspOnSpeechFragment)(UspHandle handle, void* context, UspMsgSpeechFragment *message);

/**
A callback function that will be called when a turn.start message is received from service.
* @param uspHandle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the turn.start message.
*/
typedef void(*UspOnTurnStart)(UspHandle uspHandle, void* context, UspMsgTurnStart *message);

/**
A callback function that will be called when a turn.end message is received from service.
* @param uspHandle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the turn.end message.
*/
typedef void(*UspOnTurnEnd)(UspHandle uspHandle, void* context, UspMsgTurnEnd *message);

/**
A callback function that will be called when an error occurs in handling communication with service.
* @param uspHandle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param error an error code.
*/
typedef void(*UspOnError)(UspHandle uspHandle, void* context, UspResult error);


/**
* The UspCallbacks type represents an application-defined
* structure used to register all USP events.
*/
typedef struct _UspCallbacks
{
    uint16_t size;
    uint16_t version;
    UspOnSpeechStartDetected onSpeechStartDetected;
    UspOnSpeechEndDetected onSpeechEndDetected;
    UspOnSpeechHypothesis onSpeechHypothesis;
    UspOnSpeechPhrase onSpeechPhrase;
    UspOnSpeechFragment onSpeechFragment;
    UspOnTurnStart onTurnStart;
    UspOnTurnEnd onTurnEnd;
    UspOnError OnError;
} UspCallbacks;

typedef enum {
    USP_ENDPOINT_BING_SPEECH,
    USP_ENDPOINT_CRIS
} UspEndpointType;

typedef enum {
    USP_RECO_MODE_INTERACTIVE,
    USP_RECO_MODE_CONVERSATION,
    USP_RECO_MODE_DICTATION
} UspRecognitionMode;

typedef enum {
    USP_OUTPUT_SIMPLE,
    USP_OUTPUT_DETAILED
} UspOutputFormat;

/**
* The UspOption defines supported options for a connection to services.
*/
typedef enum {
    USP_OPTION_LANGUAGE,
    USP_OPTION_OUTPUT_FORMAT,
} UspOption;

/**
* Opens a UspHandle for the specified speech service endpoint.
* @param type  The speech service to be used, e.g. USP_ENDPOINT_BING_SPEECH, USP_ENDPOINT_CRIS.
* @param mode  The recognition mode to be used, e.g. USP_RECO_MODE_INTERACTIVE, USP_RECO_MODE_CONVERSATION, and USP_RECO_MODE_DICTATION.
* @param callbacks The struct defines callback functions that will be called when various USP events occur.
* @param callbackContext A pointer to the caller provided data, which will be passed as parameter when the callback function is invoked.
* @param uspHandle The pointer to UspHandle. If the function returns USP_SUCCESS, the value pointered by uspHandle is the initialzed UspHandle that is ready for use.
*/
UspResult UspOpen(UspEndpointType type, UspRecognitionMode mode, UspCallbacks *callbacks, void* callbackContext, UspHandle* uspHandle);

/**
* Sets option of the specified uspHandle. Options must be set before calling UspConnect.
* @param uspHandle The UspHandle.
* @param optionKey The option to be set.
* @param optionValue The value of the option to be set.
*/
UspResult UspSetOption(UspHandle uspHandle, UspOption optionKey, const char* optionValue);

/**
* Establish connection to the service.
* @param uspHandle The UspHandle.
*/
UspResult UspConnect(UspHandle uspHandle);

/**
* Sends data to the uspHandle.
* @param uspHandle The UspHandle to which the data is sent.
* @param buffer The buffer contains data to be sent.
* @param bytesToWrite The amount of data in bytes to be sent.
* @param bytesWritten On a successful return, the number of bytes have been sent is returned.
*/
UspResult UspWrite(UspHandle uspHandle, const uint8_t* buffer, size_t bytesToWrite, size_t* bytesWritten);

/**
* Closes the uspHandle.
* @param uspHandle The uspHandle to be closed.
*/
UspResult UspClose(UspHandle uspHandle);

/**
* Runs the event loop.
* @param uspHandle The UspHandle.
*/
void UspRun(UspHandle uspHandle);

#ifdef __cplusplus
}
#endif