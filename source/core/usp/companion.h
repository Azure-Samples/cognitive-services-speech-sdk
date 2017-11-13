#ifndef COMPANION_H
#define COMPANION_H

#include "auth.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_URI_LEN 1024

typedef enum _COMPANION_LISTENER_TYPE
{
    COMPANION_LISTENER_TYPE_HTTP,
    COMPANION_LISTENER_TYPE_BLE,
} COMPANION_LISTENER_TYPE;

typedef enum _COMPANION_REQUEST_TYPE {
    CA_REQUEST_GET,
    CA_REQUEST_POST,
} COMPANION_REQUEST_TYPE;

struct CompanionApp_;
typedef struct CompanionApp_* CompanionApp;

/**
 * Create a handle to a companion app
 * @param  cortana_auth The auth refresher to give tokens to
 * @param  event_queue The event queue to push events onto
 * @param  hCortana     The handle to Cortana. Needed to play speech files
 * @return              A reference to a companion app handle. NULL on failure.
 */
CompanionApp companion_app_create(
    CortanaAuth cortana_auth,
    EventQueue event_queue,
    CORTANA_HANDLE hCortana
);

/**
 * Destroy a companion app
 * @param companion_app The companion app to destroy
 */
void companion_app_destroy(CompanionApp companion_app);

/**
 * Initialize the companion App listener per the type.
 * @return A return code or zero if successful.
 */
//int InitializeCompanionAppListener(COMPANION_LISTENER_TYPE listener_type, void* hCortana);
int companion_app_start(CompanionApp cortana_auth, COMPANION_LISTENER_TYPE listener_type);

/**
 * Stop the companion App listener per the type.
 * @return A return code or zero if successful.
 */
int companion_app_stop(CompanionApp cortana_auth, COMPANION_LISTENER_TYPE listener_type);

typedef void(*PCOMPANION_HANDLER_CALLBACK)(int result, const char* response, void* pContext);

/**
 * Process the CompanionApp request with current companion handers
 * @return 0 means the companion app handle was able to process the request.
 */
int companion_app_process_request(
    CompanionApp companion_app,
    const char* uri,
    COMPANION_REQUEST_TYPE cmdType,
    unsigned char* postdata,
    size_t postdataLen,
    PCOMPANION_HANDLER_CALLBACK pHandlerCallback,
    void* pCallbackContext
);

/**
* Events during OOBE. 
* Enumeration for the states of OOBE. Once particular stage of OOBE is completed, even is fired.
*/
typedef enum _COMPANION_OOBE_STATE
{
    COMPANION_STATE_RECEIVED_TOKEN_CONFIRM = 1
} COMPANION_OOBE_STATE;

typedef void(*PCOMPANION_OOBE_CHANGE_HANDLER)(COMPANION_OOBE_STATE newState, void *pContext);

/**
* Register for changes in OOBE state.
* @param pHanlder - pointer to function callback that is called once OOBE status change, which happens after one OOBE command is completed.
* @param pContext - void pointer that will be passed to the pHandler callback.
* return 0 if listener added successfully. If listener cannot be added, return non-zero.
*/
int companion_app_register_oobe_event(CompanionApp companion_app, PCOMPANION_OOBE_CHANGE_HANDLER pHanlder, void *pContext);

/**
* Unregister for changes in OOBE state.
* @param pHanlder - pointer to function callback that user wants to unregister.
* return 0 if listener was removed successfully. If listener cannot be found, return 1.
*/
int companion_app_unregister_oobe_event(CompanionApp companion_app, PCOMPANION_OOBE_CHANGE_HANDLER pHanlder);

/****************************/
/*   TEMPORARY INTERFACE    */
/****************************/

//This is temporarily exposed because previously this was the only sane way for Android
// and windows to set the token. Once we change the public API of cortana so that
// the auth refresher is exposed correctly, this won't be needed any longer
void parse_companionapp_token_payload(SPEECH_HANDLE speech_handle, const char * payload);

int companion_unregister_oobe_event(SPEECH_HANDLE speech_handle, PCOMPANION_OOBE_CHANGE_HANDLER pHanlder);
int companion_register_oobe_event(SPEECH_HANDLE speech_handle, PCOMPANION_OOBE_CHANGE_HANDLER pHandler, void *pContext);


#ifdef __cplusplus
} // extern "C" 
#endif

#endif // COMPANION_H
