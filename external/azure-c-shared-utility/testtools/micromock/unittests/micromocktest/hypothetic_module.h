// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*defines*/
#ifndef HYPOTHETIC_MODULE_H
#define HYPOTHETIC_MODULE_H

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/*types*/
/*variable exports*/
/*function exports*/

typedef void (*pVoidFunction)(void);
typedef char *pChar;

extern void zero(void);
extern int izero(void);
extern int one  (_In_ int i);
extern int two  (_In_z_ pChar s, _In_ int i);
extern int three(_In_ char c, _In_z_ pChar s, _In_ int i);
extern int four (_In_ unsigned short int si, _In_ char c, _In_z_ pChar s, _In_ int i);
extern int five (_In_opt_ pVoidFunction pVoid, _In_ unsigned short int si, _In_ char c, _In_z_ pChar s, _In_ int i);
extern int six  (_In_ char c1, _In_ char c2, _In_ char c3, _In_ char c4, _In_ char c5, _In_ char c6);

extern void theTask(void);

#ifdef __cplusplus
}
#endif

#endif
