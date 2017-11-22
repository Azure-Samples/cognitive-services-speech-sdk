// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*defines*/
#ifndef HYPOTHETIC_MODULE_INTERFACE_H
#define HYPOTHETIC_MODULE_INTERFACE_H

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/*types*/
typedef void(*pVoidFunction)(void);
typedef char* pChar;

/*variable exports*/
/*function exports*/
extern void whenzero(void);
extern int whenizero(void);
extern int whenone  (_In_ int i);
extern int whentwo  (_In_z_ pChar s, _In_ int i);
extern int whenthree(_In_ char c, _In_z_ pChar s, _In_ int i);
extern int whenfour (_In_ unsigned short int si, _In_ char c, _In_z_ pChar s, _In_ int i);
extern int whenfive (_In_opt_ pVoidFunction pVoid, _In_ unsigned short int si, _In_ char c, _In_z_ pChar s, _In_ int i);
extern int whensix  (_In_ char c1, _In_ char c2, _In_ char c3, _In_ char c4, _In_ char c5, _In_ char c6);

#ifdef __cplusplus
}
#endif

#endif
