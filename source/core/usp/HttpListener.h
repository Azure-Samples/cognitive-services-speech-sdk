#ifndef HTTP_LISTENER_H
#define HTTP_LISTENER_H

#include "companion.h"

#ifdef __cplusplus
extern "C" {
#endif

int  httplistener_create(unsigned short port, CompanionApp companion_app);
void httplistener_destroy();
int  httplistener_dowork();

#ifdef __cplusplus
} // extern "C" 
#endif

#endif // HTTP_LISTENER_H
