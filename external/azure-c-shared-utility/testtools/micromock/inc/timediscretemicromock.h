// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*defines*/
#ifndef TIME_DISCRETE_MICRO_MOCK_H
#define TIME_DISCRETE_MICRO_MOCK_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "micromock.h"

typedef unsigned int UINT32;

#define TD_MOCK_CLASS(className)       \
class Typed_##className; \
typedef CTimeDiscreteMicroMock<Typed_##className> className; \
TYPED_MOCK_CLASS(Typed_##className, CThreadSafeGlobalMock)

// A strict expected call implies that all arguments are checked
#define STRICT_EXPECTED_CALL_AT(ClassName, time, ...)						\
    STRICT_EXPECTED_CALL(ClassName, __VA_ARGS__)                            \
    .AddExtraCallArgument(new CMockCallArgument<UINT32>(time, false))				

   

// By using the below macro, none of the arguments are checked by default
// To specify checking the argument values, use the ValidateArgument 
// expected call modifier method
#define EXPECTED_CALL_AT(ClassName, time, ...)               \
STRICT_EXPECTED_CALL_AT(ClassName, time, __VA_ARGS__)        \
    .IgnoreAllArguments()

/*this macro takes a mock name, looks it up in the repository, and gets the time provider for that mock*/
#define MOCK_TIMEPROVIDER(mockName)																									\
    (dynamic_cast<CTimeDiscreteMicroMockBase*>(GetSingleton())->getCurrentTick())





/*a poor interface that says "I know how to play"*/
class canPlay
{
    public:
        virtual void PlayTick(_In_ UINT32 time, _In_ UINT32 order)=0;
        virtual ~canPlay()=0;
};

inline canPlay::~canPlay(){}

template <typename T> class valueHolder
{
private:
    T theValue;
public:
    valueHolder(_In_ const T& t)
    {
        theValue = t;
    }

    void operator=(_In_ T& t)
    {
        theValue = t;
    }

    operator T*(void)
    {
        return &theValue;
    }

    operator T&(void)
    {
        return theValue;
    }

#pragma warning (push)
#pragma warning (disable : 4100) /*'size' : unreferenced formal parameter*/
    void setArraySize(_In_ size_t size)
    {
        UNREFERENCED_PARAMETER(size);
        ASSERT_FAIL("Setting an array size for something that is not a pointer is CATASTROPHIC failure");
    }
#pragma warning (pop)
};

/*template template parameter partial specialization, or something*/
template <typename T> class valueHolder <T*>
{
private:
    bool wasNULL;
    
    T theValue;
    
    T* originalPointer;
    T* copyArray;
    size_t arraySize;

private:
    void SetValue(_In_ T* t)
    {
        if(t==NULL)
        {
            wasNULL = true;
        }
        else
        {
            wasNULL = false;
            theValue = *t;
            originalPointer = t;
            arraySize = 0;
        }
    }

public:
    valueHolder(_In_ T* t) :
        copyArray(NULL)
    {
        SetValue(t);
    }

    valueHolder(_In_ const valueHolder & t):
        wasNULL(t.wasNULL), theValue(t.theValue), originalPointer(t.originalPointer), arraySize(t.arraySize)
    {
        if(t.copyArray!=NULL)
        {
            copyArray = new T[t.arraySize];
            ASSERT_IS_TRUE_WITH_MSG(NULL != copyArray, _T("memory error"));
            for(size_t i=0;i<t.arraySize;i++)
            {
                copyArray[i]=t.copyArray[i];
            }
        }
        else
        {
            copyArray = NULL;
        }
    }
    valueHolder():wasNULL(false),arraySize(0), copyArray(NULL), originalPointer(NULL)
    {
    }

    virtual ~valueHolder()
    {
        if(copyArray!=NULL)
        {
            delete[] copyArray;
            copyArray=NULL;
        }
    }

    /*it could be just a simple pointer...*/
    void operator=(_In_ T* t)
    {
        SetValue(t);
    }
    
    operator T*(void)
    {
        if(wasNULL)
        {
            return NULL;
        }
        else
        {
            if(arraySize==0)
            {
                return &theValue;
            }
            else
            {
                return copyArray;
            }
        }
    }

    /*or could be an array of user specififed length*/
    void setArraySize(_In_ size_t size)
    {
        if(size==0) 
        {
            ASSERT_FAIL("size cannot ever be 0 for an array");
        }
        else
        {
            if(copyArray!=NULL)
            {
                delete[] copyArray;
                copyArray=NULL;
            }

            copyArray = new T[size];
            ASSERT_IS_TRUE_WITH_MSG(copyArray != NULL, _T("catastrophic memory allocation error (out of memory?)"));
            for(size_t i=0;i<size;i++)
            {
                copyArray[i]=originalPointer[i];
            }
            arraySize = size;
        }
    }
};

/*copy & paste from above, except the const modifier*/
template <typename T> class valueHolder <const T*>
{
private:
    bool wasNULL;
    
    T theValue;
    
    const T* originalPointer;
    T* copyArray;
    size_t arraySize;

private:
    void SetValue(_In_ const T* t)
    {
        if(t==NULL)
        {
            wasNULL = true;
        }
        else
        {
            wasNULL = false;
            theValue = *t;
            originalPointer = t;
            arraySize = 0;
        }
    }

public:
    valueHolder(_In_ const T* t) :
        copyArray(NULL)
    {
        SetValue(t);
    }

    valueHolder(_In_ const valueHolder & t):
        wasNULL(t.wasNULL), theValue(t.theValue), originalPointer(t.originalPointer), arraySize(t.arraySize)
    {
        if(t.copyArray!=NULL)
        {
            copyArray = new T[t.arraySize];
            ASSERT_IS_TRUE_WITH_MSG(NULL != copyArray, _T("memory error"));
            for(size_t i=0;i<t.arraySize;i++)
            {
                copyArray[i]=t.copyArray[i];
            }
        }
        else
        {
            copyArray = NULL;
        }
    }
    valueHolder():wasNULL(false),arraySize(0), copyArray(NULL), originalPointer(NULL)
    {
    }

    virtual ~valueHolder()
    {
        if(copyArray!=NULL)
        {
            delete[]copyArray;
            copyArray=NULL;
        }
    }

    /*it could be just a simple pointer...*/
    void operator=(_In_ const T* t)
    {
        SetValue(t);
    }
    
    operator T*(void)
    {
        if(wasNULL)
        {
            return NULL;
        }
        else
        {
            if(arraySize==0)
            {
                return &theValue;
            }
            else
            {
                return copyArray;
            }
        }
    }

    /*or could be an array of user specified length*/
    void setArraySize(_In_ size_t size)
    {
        if(size==0) 
        {
            ASSERT_FAIL("size cannot ever be 0 for an array");
        }
        else
        {
            if(copyArray!=NULL)
            {
                delete []copyArray;
                copyArray=NULL;
            }

            copyArray=new T[size];
            ASSERT_IS_TRUE_WITH_MSG(copyArray != NULL, _T("catastrophic memory allocation error (out of memory?)"));
            for(size_t i=0;i<size;i++)
            {
                copyArray[i]=originalPointer[i];
            }
            arraySize = size;
        }
    }
};

/*this is a special case*/
template <> class valueHolder <char*>
{
private:
    std::string theValue;
public:
    valueHolder(_In_z_ char* t)
    {
        theValue = t;
    }
    
    void operator=(_In_z_ char* t)
    {
        theValue = t;
    }
    
    operator char*(void)
    {
        return (char*)(theValue.c_str());
    }
};

/*this is a special case*/
template <> class valueHolder <wchar_t*>
{
private:
    std::wstring theValue;
public:
    valueHolder(_In_z_ wchar_t* t)
    {
        theValue = t;
    }

    void operator=(_In_z_ wchar_t* t)
    {
        theValue = t;
    }
    
    operator wchar_t*(void)
    {
        return (wchar_t*)theValue.c_str();
    }
};



class stims_base
{
    /*friends because registerCallXArg should not be available as public*/
    template<typename resultType, class C> friend class call0Arg;
    template<typename resultType, typename arg1Type, class C> friend class call1Arg;
    template<typename resultType, typename arg1Type, typename arg2Type, class C> friend class call2Arg;
    template<typename resultType, typename arg1Type, typename arg2Type, typename arg3Type, class C> friend class call3Arg;
    template<typename resultType, typename arg1Type, typename arg2Type, typename arg3Type, typename arg4Type, class C> friend class call4Arg;
    template<typename resultType, typename arg1Type, typename arg2Type, typename arg3Type, typename arg4Type, typename arg5Type, class C> friend class call5Arg;
    template<typename resultType, typename arg1Type, typename arg2Type, typename arg3Type, typename arg4Type, typename arg5Type, typename arg6Type, class C> friend class call6Arg;
    template<typename resultType, typename arg1Type, typename arg2Type, typename arg3Type, typename arg4Type, typename arg5Type, typename arg6Type, typename arg7Type, class C> friend class call7Arg;
    template<typename resultType, typename arg1Type, typename arg2Type, typename arg3Type, typename arg4Type, typename arg5Type, typename arg6Type, typename arg7Type, typename arg8Type, class C> friend class call8Arg;
    template<typename resultType, typename arg1Type, typename arg2Type, typename arg3Type, typename arg4Type, typename arg5Type, typename arg6Type, typename arg7Type, typename arg8Type, typename arg9Type, class C> friend class call9Arg;
    template<typename resultType, typename arg1Type, typename arg2Type, typename arg3Type, typename arg4Type, typename arg5Type, typename arg6Type, typename arg7Type, typename arg8Type, typename arg9Type, typename arg10Type, class C> friend class call10Arg;
    
private:
    static std::vector<canPlay *> allPlayers;

    static void registerCallXArg(canPlay* someStim)								
    {																													
        allPlayers.push_back(someStim);																					
    }

public:
    virtual ~stims_base()
    {
        allPlayers.clear();
    }
    
    void static PlayTick(_In_ UINT32 tick, _In_ UINT32 order)
    {
        for(UINT32 i=0;i<allPlayers.size();i++)
        {
            allPlayers[i]->PlayTick(tick, order);
        }
    }
};

#include "timediscretemicromockcallmacros.h"

extern UINT32 theTick;
extern UINT32 totalTicksPlayed;
#define MAXTICK 1000000

class CTimeDiscreteMicroMockBase : public stims_base
{
private:
    UINT32 m_currentTick;
    void(*m_theTask)(void);
    UINT32*maxOrder;
public:
    /*the only constructor will accept a task given as function name*/
    CTimeDiscreteMicroMockBase(_In_opt_ void(*p)(void)) :m_theTask(p)
    {
        maxOrder = (UINT32*)malloc(MAXTICK*sizeof(UINT32));
        if (maxOrder == NULL)
        {
            MOCK_THROW(_T("out of memory"));
        }
        else
        {
            memset(maxOrder, 0xFF, MAXTICK*sizeof(UINT32));
        }
    }

    virtual ~CTimeDiscreteMicroMockBase()
    {
        if (maxOrder != NULL)
        {
            free(maxOrder);
            maxOrder = NULL;
        }
    }

    void RunUntilTick(_In_ UINT32 nTick)
    {
        totalTicksPlayed += nTick;
        if (nTick >= MAXTICK)
        {
            MOCK_THROW(_T("time detected bigger than MAXTICK"));
        }
        for (UINT32 i = 0; i <= nTick; i++)
        {
            m_currentTick = i;
            theTick = m_currentTick;

            if (maxOrder[i] != 0xFFFFFFFF)
            {
                for (UINT32 order = 0; order <= maxOrder[i]; order++)
                {
                    stims_base::PlayTick(i, order);
                }
            }
            m_theTask();
        }
    }

    _Must_inspect_result_
        UINT32 getCurrentTick(void)
    {
            return m_currentTick;
    }

    _Must_inspect_result_
        UINT32 getAndIncOrder(_In_ UINT32 time)
    {
            if (time<MAXTICK)
            {
                if (maxOrder[time] == 0xFFFFFFFF)
                {
                    maxOrder[time] = 0;
                }
                else
                {
                    maxOrder[time] = maxOrder[time] + 1;
                }
            }
            else
            {
                MOCK_THROW(_T("time detected bigger than MAXTICK"));
            }

            return maxOrder[time];
    }
};

template<class T>
class CTimeDiscreteMicroMock: public T, public CTimeDiscreteMicroMockBase
{
public:
    CTimeDiscreteMicroMock(void (*theTask)(void)):CTimeDiscreteMicroMockBase(theTask)
    {
    }
};


/*variable exports*/
/*function exports*/

#endif
