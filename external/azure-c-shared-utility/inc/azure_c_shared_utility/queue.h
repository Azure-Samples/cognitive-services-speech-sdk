// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef QUEUE_H
#define QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "azure_c_shared_utility/list.h"

void *queue_peek(LIST_HANDLE hQueue);
void *queue_dequeue(LIST_HANDLE hQueue);
int   queue_enqueue(LIST_HANDLE hQueue, void *pData);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* QUEUE_H */
