// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*defines*/
#include "stdafx_c.h"
#include "hypothetic_module.h"
#include "hypothetic_module_interfaces.h"

#define SHADOW_BUFFER_SIZE 100
/*types*/
typedef struct data_tag
{
    int wasZeroCalled;

    int wasizeroCalled;
    
    int wasoneCalled;  
    int ioneiShadow;

    int wastwoCalled;
    char twosShadow[SHADOW_BUFFER_SIZE];
    int twoiShadow;

    int wasthreeCalled;
    char threecShadow;
    char threesShadow[SHADOW_BUFFER_SIZE];
    int threeiShadow;

    int wasfourCalled; 
    unsigned short int foursiShadow;
    char fourcShadow;
    char foursShadow[SHADOW_BUFFER_SIZE];
    int fouriShadow;

    int wasfiveCalled; 
    pVoidFunction fivepVoidShadow;
    unsigned short int fivesiShadow;
    char fivecShadow;
    char fivesShadow[SHADOW_BUFFER_SIZE];
    int fiveiShadow;

    int wassixCalled;  
    char sixc1Shadow ,sixc2Shadow ,sixc3Shadow ,sixc4Shadow ,sixc5Shadow ,sixc6Shadow;
}data;
/*static variables*/
static data d;
/*static functions*/
/*variable exports*/
/*function exports*/
void zero(void)
{
    d.wasZeroCalled=1;
}

int izero(void)
{
    d.wasizeroCalled=1;
    return 0;
}

int one(_In_ int i)
{
    d.wasoneCalled=1;
    d.ioneiShadow=i;
    return 0;
}

int two(_In_z_ pChar s, _In_ int i)
{
    d.wastwoCalled=1;
    strcpy_s(d.twosShadow, SHADOW_BUFFER_SIZE,s);
    //d.twosShadow[0]=s;

    d.twoiShadow = i;
    return 0;
}

int three(_In_ char c, _In_z_ pChar s, _In_ int i)
{
    d.wasthreeCalled=1;
    d.threecShadow=c;
    strcpy_s(d.threesShadow, SHADOW_BUFFER_SIZE, s);
    //d.threesShadow[0]=s;
    d.threeiShadow = i;
    return 0;
}

int four(_In_ unsigned short int si, _In_ char c, _In_z_ pChar s, _In_ int i)
{
    d.wasfourCalled=1;
    d.foursiShadow = si;
    d.fourcShadow=c;
    strcpy_s(d.foursShadow, SHADOW_BUFFER_SIZE, s);
    //d.foursShadow[0]=s;
    d.fouriShadow = i;
    return 0;
}

int five(_In_opt_ pVoidFunction pVoid, _In_ unsigned short int si, _In_ char c, _In_z_ pChar s, _In_ int i)
{
    d.wasfiveCalled=1;
    d.fivepVoidShadow = pVoid;
    d.fivesiShadow = si;
    d.fivecShadow=c;
    strcpy_s(d.fivesShadow, SHADOW_BUFFER_SIZE, s);
    //d.fivesShadow[0]=s;
    d.fiveiShadow = i;
    return 0;
}

int six(_In_ char c1, _In_ char c2, _In_ char c3, _In_ char c4, _In_ char c5, _In_ char c6)
{
    d.wassixCalled=1;
    d.sixc1Shadow=c1;
    d.sixc2Shadow=c2;
    d.sixc3Shadow=c3;
    d.sixc4Shadow=c4;
    d.sixc5Shadow=c5;
    d.sixc6Shadow=c6;
    return 0;
}

void theTask(void)
{
    if(d.wasZeroCalled)
    {
        d.wasZeroCalled=0;
        whenzero();
    }

    if(d.wasizeroCalled)
    {
        d.wasizeroCalled=0;
        whenizero();
    }

    if(d.wasoneCalled)
    {
        d.wasoneCalled=0;
        whenone(d.ioneiShadow+1);
    }

    if(d.wastwoCalled)
    {
        d.wastwoCalled=0;
        whentwo(d.twosShadow, d.twoiShadow+2);
    }

    if(d.wasthreeCalled)
    {
        d.wasthreeCalled=0;
        whenthree(d.threecShadow+3, d.threesShadow, d.threeiShadow+3);
    }

    if(d.wasfourCalled)
    {
        d.wasfourCalled=0;
        whenfour(d.foursiShadow+4, d.fourcShadow+4, d.foursShadow, d.fouriShadow+4);
    }

    if(d.wasfiveCalled)
    {
        d.wasfiveCalled=0;
        whenfive(d.fivepVoidShadow, d.fivesiShadow+5, d.fivecShadow+5, d.fivesShadow, d.fiveiShadow+5);
    }

    if(d.wassixCalled)
    {
        d.wassixCalled=0;
        whensix(d.sixc1Shadow+6,d.sixc2Shadow+6,d.sixc3Shadow+6,d.sixc4Shadow+6,d.sixc5Shadow+6,d.sixc6Shadow+6);
    }


}

