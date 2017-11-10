// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SOME_REFCOUNT_IMPL_H
#define SOME_REFCOUNT_IMPL_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct pos* POS_HANDLE;

extern POS_HANDLE Pos_Create(int x);
extern POS_HANDLE Pos_Clone(POS_HANDLE posHandle);
extern void Pos_Destroy(POS_HANDLE posHandle);

#ifdef __cplusplus
}
#endif

#endif /*SOME_REFCOUNT_IMPL_H*/
