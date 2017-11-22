// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "stdafx.h"
#include "hypothetic_module.h"
#include <string>


using namespace std;

//MICROMOCK_ENUM_TO_STRING_DECLARE(MICROMOCK_EXCEPTION_TAG);

#ifdef _MSC_VER

MICROMOCK_ENUM_TO_STRING(MICROMOCK_EXCEPTION_TAG,
    L"MICROMOCK_EXCEPTION_INVALID_VALIDATE_BUFFERS",
    L"MICROMOCK_EXCEPTION_ALLOCATION_FAILURE",
    L"MICROMOCK_EXCEPTION_INVALID_ARGUMENT",
    L"MICROMOCK_EXCEPTION_INVALID_CALL_MODIFIER_COMBINATION",
    L"MICROMOCK_EXCEPTION_MOCK_NOT_FOUND",
    L"MICROMOCK_EXCEPTION_SET_TIME_BEFORE_CALL",
    L"MICROMOCK_EXCEPTION_SET_ARRAY_SIZE_BEFORE_CALL",
    L"MICROMOCK_EXCEPTION_INTERNAL_ERROR");



/*tests table of content*/

/*
TEST_FUNCTION(TimeDiscreteMicroMock_LoopbackAll)
TEST_FUNCTION(TimeDiscreteMicroMock_FileOrderIsPreserved_1)
TEST_FUNCTION(TimeDiscreteMicroMock_FileOrderIsPreserved_2)
TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved)
TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_2)
TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_3)
TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_4)
TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_5)
TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_understands_NULL_buffer)
TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_understands_buffer_of_1_element)
TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_understands_buffer_of_5_elements)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_NULL_NULL)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_NULL_1int)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_NULL_3int)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_1int_NULL)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_1int_1int)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_1int_3int)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_3int_NULL)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_3int_1int)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_3int_4int)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_can_be_reused)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_can_be_reused_at_different_times)
TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_can_copy_an_array_of_objects)
TEST_FUNCTION(TimeDiscreteMicroMock_FailsWhenUserCallsDirectly_setArraySize)
TEST_FUNCTION(TimeDiscreteMicroMock_FailsWhenUserCallsDirectly_setTime)
TEST_FUNCTION(TimeDiscreteMicroMock_Const_ParameterBuffersAreSaved_understands_NULL_buffer)
TEST_FUNCTION(TimeDiscreteMicroMock_Const_ParameterBuffersAreSaved_understands_buffer_of_1_element)
TEST_FUNCTION(TimeDiscreteMicroMock_Const_ParameterBuffersAreSaved_understands_buffer_of_5_elements)
TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_baseline)
TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_using_without_strict_with_real_time)
TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_time_mismatch_is_detected)
TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_time_mismatch_is_detected_without_STRICT)
TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_IgnoreArgument)
TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_IgnoreAllArguments)
*/


/*so sometimes some types that look like pointers and are not pointers have to be explictly implemented (specialized)*/
template <> class valueHolder <pVoidFunction>
{
private:
    pVoidFunction theValue;
    
public:
    valueHolder(pVoidFunction t)
    {
        theValue = t;
    }

    void operator=(pVoidFunction t)
    {
        theValue = t;
    }
    
    operator pVoidFunction()
    {
        return theValue;
    }
};

template<>
static bool operator==<char*>(const CMockValue<char*>& lhs, const CMockValue<char*>& rhs)
{
    bool result = false;
    if (lhs.GetValue() == rhs.GetValue())
    {
        result = true;
    } else
    {
        if ((NULL != lhs.GetValue()) &&
            (NULL != rhs.GetValue()) &&
            (strcmp(lhs.GetValue(), rhs.GetValue()) == 0))
        {
            result = true;
        }
    }
    return result;
}

extern "C" void whenzero(void);
extern "C" int whenizero(void);
extern "C" int whenone(_In_ int i);
extern "C" int whentwo(_In_ pChar s, _In_ int i);
extern "C" int whenthree(_In_ char c, _In_  pChar s, _In_ int i);
extern "C" int whenfour(_In_ unsigned short int si, _In_ char c, _In_ pChar s, _In_ int i);
extern "C" int whenfive(_In_ pVoidFunction pVoid, _In_ unsigned short int si, _In_ char c, _In_ pChar s, _In_ int i);
extern "C" int whensix(_In_ char c1, _In_ char c2, _In_ char c3, _In_ char c4, _In_ char c5, _In_ char c6);

DECLARE_STIM_STATIC_TD_METHOD_0(,void, zero);
DECLARE_STIM_STATIC_TD_METHOD_0(,int, izero);
DECLARE_STIM_STATIC_TD_METHOD_1(,int, one, _In_ int, i);
DECLARE_STIM_STATIC_TD_METHOD_2(,int, two, _In_ pChar, s, int, i);
DECLARE_STIM_STATIC_TD_METHOD_3(,int, three, _In_ char, c, _In_ pChar, s, _In_ int, i);
DECLARE_STIM_STATIC_TD_METHOD_4(,int, four, _In_ unsigned short int, si, _In_ char, c, _In_ pChar, s, _In_ int, i);
DECLARE_STIM_STATIC_TD_METHOD_5(,int, five, _In_ pVoidFunction, pVoid, _In_ unsigned short int, si, _In_ char, c, _In_ pChar, s, _In_ int, i);
DECLARE_STIM_STATIC_TD_METHOD_6(,int, six, _In_ char, c1, _In_ char, c2, _In_ char, c3, _In_ char, c4, _In_ char, c5, _In_ char, c6);




static int posIndex=0;
static int posRecorder[2];
extern "C" void takes1int_1(int i)
{
    if(posIndex<2)
    {
        posRecorder[posIndex++]=i;
    }
    else
    {
        ASSERT_FAIL(_T("too much posIndex for me"));
    }
}

extern "C" void takes1int_2(int i)
{
    if(posIndex<2)
    {
        posRecorder[posIndex++]=i;
    }
    else
    {
        ASSERT_FAIL(_T("too much posIndex for me"));
    }
}

DECLARE_STIM_STATIC_TD_METHOD_1(,void, takes1int_1, _In_ int, i);
DECLARE_STIM_STATIC_TD_METHOD_1(,void, takes1int_2, _In_ int, i);

TD_MOCK_CLASS(MyMocks3)
{
public: 
    STIM_STATIC_TD_METHOD_0(,void, zero);
    STIM_STATIC_TD_METHOD_0(,int, izero);
    STIM_STATIC_TD_METHOD_1(,int, one, _In_ int, i);
    STIM_STATIC_TD_METHOD_2(,int, two, _In_ pChar, s, _In_ int, i);
    STIM_STATIC_TD_METHOD_3(,int, three, _In_ char, c, _In_ pChar, s, _In_ int, i);
    STIM_STATIC_TD_METHOD_4(,int, four, _In_ unsigned short int, si, _In_ char, c, _In_ pChar, s, _In_ int, i);
    STIM_STATIC_TD_METHOD_5(,int, five, _In_ pVoidFunction, pVoid, _In_ unsigned short int, si, _In_ char, c, _In_ pChar, s, _In_ int, i);
    STIM_STATIC_TD_METHOD_6(,int, six, char, c1, char, c2, char, c3, char, c4, char, c5, char, c6);

    STIM_STATIC_TD_METHOD_1(,void, takes1int_1, _In_ int, i);
    STIM_STATIC_TD_METHOD_1(,void, takes1int_2, _In_ int, i);

    MOCK_STATIC_TD_METHOD_0(, void, whenzero)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_TD_METHOD_0(, int, whenizero)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_TD_METHOD_1(, int, whenone, _In_ int, i)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_TD_METHOD_2(, int, whentwo, _In_ pChar, s, _In_ int, i)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_TD_METHOD_3(, int, whenthree, _In_ char, c, _In_ pChar, s, _In_ int, i);
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_TD_METHOD_4(, int, whenfour, _In_ unsigned short int, si, _In_ char, c, _In_ pChar, s, _In_ int, i);
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_TD_METHOD_5(, int, whenfive, _In_ pVoidFunction, pVoid, _In_ unsigned short int, si, _In_ char, c, _In_ pChar, s, _In_ int, i);    
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_TD_METHOD_6(,int, whensix, _In_ char, c1, _In_ char, c2, _In_ char, c3, _In_ char, c4, _In_ char, c5, _In_ char, c6);
    MOCK_METHOD_END(int, 0)
};

DECLARE_GLOBAL_MOCK_METHOD_0(MyMocks3, , void, whenzero)
DECLARE_GLOBAL_MOCK_METHOD_0(MyMocks3, , int, whenizero)
DECLARE_GLOBAL_MOCK_METHOD_1(MyMocks3, , int, whenone, int, i)
DECLARE_GLOBAL_MOCK_METHOD_2(MyMocks3, , int, whentwo, pChar, s, int, i)
DECLARE_GLOBAL_MOCK_METHOD_3(MyMocks3, , int, whenthree, char, c, pChar, s, int, i)
DECLARE_GLOBAL_MOCK_METHOD_4(MyMocks3, , int, whenfour, unsigned short int, si, char, c, pChar, s, int, i)
DECLARE_GLOBAL_MOCK_METHOD_5(MyMocks3, , int, whenfive, pVoidFunction, pVoid, unsigned short int, si, char, c, pChar, s, int, i)
DECLARE_GLOBAL_MOCK_METHOD_6(MyMocks3, , int, whensix, char, c1, char, c2, char, c3, char, c4, char, c5, char, c6)

TCHAR g_STIM_FunctionThatTakesAStringParameter[2][1000]; /*1000 will be enough for our testing purposes*/
UINT32  g_STIM_FunctionThatTakesAStringParameterCalls=0;
void STIM_FunctionThatTakesAString(_In_z_ TCHAR* someString)
{
    if(g_STIM_FunctionThatTakesAStringParameterCalls<2)
    {
        _tcscpy_s(g_STIM_FunctionThatTakesAStringParameter[g_STIM_FunctionThatTakesAStringParameterCalls++], someString);
    }
    else
    {
        ASSERT_FAIL(_T("too many calls / test method"));
    }
}

int g_STIM_FunctionThatTakesAPointerToInt[2]; /*1000 will be enough for our testing purposes*/
UINT32  g_STIM_FunctionThatTakesAPointerToIntCalls=0;
void STIM_FunctionThatTakesAPointerToInt(_In_ int* a)
{
    if(g_STIM_FunctionThatTakesAPointerToIntCalls<2)
    {
        g_STIM_FunctionThatTakesAPointerToInt[g_STIM_FunctionThatTakesAPointerToIntCalls++]=*a;
    }
    else
    {
        ASSERT_FAIL(_T("too many calls / test method"));
    }
}

int g_STIM_FunctionThatTakesAInt[2]; /*1000 will be enough for our testing purposes*/
UINT32  g_STIM_FunctionThatTakesAIntCalls=0;
void STIM_FunctionThatTakesAInt(_In_ int a)
{
    if(g_STIM_FunctionThatTakesAIntCalls<2)
    {
        g_STIM_FunctionThatTakesAInt[g_STIM_FunctionThatTakesAIntCalls++]=a;
    }
    else
    {
        ASSERT_FAIL(_T("too many calls / test method"));
    }
}

int g_STIM_FunctionThatTakesAnArray[2][10]; 
bool g_STIM_FunctionThatTakesAnArrayWasParameterNULL=false;
int g_STIM_FunctionThatTakesAnArrayNParameters=0;
int g_STIM_FunctionThatTakesAnArray_nCalls=0;
void STIM_FunctionThatTakesAnArray(_In_reads_opt_(g_STIM_FunctionThatTakesAnArrayNParameters) int* array)
{
    //could very well be: while(array[i]!=0) cout<<array[i++];
    if(array==NULL)
    {
        g_STIM_FunctionThatTakesAnArrayWasParameterNULL = true;
    }
    else
    {
        if(g_STIM_FunctionThatTakesAnArray_nCalls<2)
        {
            for(int i=0;i<g_STIM_FunctionThatTakesAnArrayNParameters;i++)
            {
                
                g_STIM_FunctionThatTakesAnArray[g_STIM_FunctionThatTakesAnArray_nCalls][i]=array[i];
            }
            g_STIM_FunctionThatTakesAnArray_nCalls++;
        }
        else
        {
            ASSERT_FAIL(_T("catastrophic number of calls"));
        }
    }
}

int g_STIM_FunctionThatTakesAConstArray[2][10]; 
bool g_STIM_FunctionThatTakesAConstArrayWasParameterNULL=false;
int g_STIM_FunctionThatTakesAConstArrayNParameters=0;
int g_STIM_FunctionThatTakesAConstArray_nCalls=0;
void STIM_FunctionThatTakesAConstArray(_In_reads_opt_(g_STIM_FunctionThatTakesAConstArrayNParameters) const int* array)
{
    //could very well be: while(array[i]!=0) cout<<array[i++];
    if(array==NULL)
    {
        g_STIM_FunctionThatTakesAConstArrayWasParameterNULL = true;
    }
    else
    {
        if(g_STIM_FunctionThatTakesAConstArray_nCalls<2)
        {
            for(int i=0;i<g_STIM_FunctionThatTakesAConstArrayNParameters;i++)
            {
                
                g_STIM_FunctionThatTakesAConstArray[g_STIM_FunctionThatTakesAConstArray_nCalls][i]=array[i];
            }
            g_STIM_FunctionThatTakesAConstArray_nCalls++;
        }
        else
        {
            ASSERT_FAIL(_T("catastrophic number of calls"));
        }
    }
}

int* STIM_FunctionThatTakesTwoArrays_array1_called_pointer;
int STIM_FunctionThatTakesTwoArrays_array1_copy[100];
size_t STIM_FunctionThatTakesTwoArrays_nArray1;
int* STIM_FunctionThatTakesTwoArrays_array2_called_pointer;
int STIM_FunctionThatTakesTwoArrays_array2_copy[100];
size_t STIM_FunctionThatTakesTwoArrays_nArray2;
void STIM_FunctionThatTakesTwoArrays(_In_reads_opt_(nArray1) int* array1, _In_ size_t nArray1, _In_reads_opt_(nArray2) int* array2, _In_ size_t nArray2 )
{
    STIM_FunctionThatTakesTwoArrays_array1_called_pointer = array1;
    STIM_FunctionThatTakesTwoArrays_nArray1 = nArray1;
    if(array1!=NULL)
    {
        for(size_t i=0;i<nArray1;i++)
        {
            STIM_FunctionThatTakesTwoArrays_array1_copy[i]=array1[i];
        }
    }

    STIM_FunctionThatTakesTwoArrays_array2_called_pointer = array2;
    STIM_FunctionThatTakesTwoArrays_nArray2 = nArray2;
    if(array2!=NULL)
    {
        for(size_t i=0;i<nArray2;i++)
        {
            STIM_FunctionThatTakesTwoArrays_array2_copy[i]=array2[i];
        }
    }
}


void someTask(void)
{
}

static int s_someTaskThatCallsWhenTwoAtTimeEqualThree_nCalls=0;
void someTaskThatCallsWhenTwoAtTimeEqualThree(void)
{
    if(s_someTaskThatCallsWhenTwoAtTimeEqualThree_nCalls == 3)
    {
        whentwo("two", -4);
    }
    s_someTaskThatCallsWhenTwoAtTimeEqualThree_nCalls ++;
}


class someSortOfT
{
private:
    tstring someString;
    char* someChar;
public:
    //default constructor
    someSortOfT():someChar(NULL)
    {
    }

    //copy constructor
    someSortOfT(const someSortOfT & t):someString(t.someString)
    {
        if(t.someChar==NULL)
        {
            someChar = NULL;
        }
        else
        {
            size_t strLen = strlen(t.someChar);
            someChar = (char*) malloc(strLen+1);
            if(someChar==NULL)
            {
                ASSERT_FAIL(_T("catastrophic memory error"));
            }
            else
            {
                strcpy_s(someChar, strLen+1, t.someChar);
            }
        }

    }
    
    //init constructor
    someSortOfT(tstring s, _In_opt_z_ char* p)
    {
        someString = s;
        if(p==NULL)
        {
            someChar = NULL;
        }
        else
        {
            size_t strLen = strlen(p);
            someChar = (char*) malloc(strLen+1);
            if(someChar==NULL)
            {
                ASSERT_FAIL(_T("catastrophic memory error"));
            }
            else
            {
                strcpy_s(someChar, strLen+1, p);
            }
        }
    }

    someSortOfT& operator=(const someSortOfT & right)
    {
        if(this==&right)
        {
            //nice try
        }
        else
        {
            someString = right.someString;
            
            if(someChar!=NULL)
            {
                free(someChar);
                someChar=NULL;
            }

            if(right.someChar!=NULL)
            {
                size_t strLen = strlen(right.someChar);
                someChar = (char*)malloc(strLen+1);
                if(someChar==NULL)
                {
                    ASSERT_FAIL(_T("catastrphic memory error"));
                }
                else
                {
                    strcpy_s(someChar, strLen+1, right.someChar);
                }
            }

        }
        return *this;
    }

    bool operator==(const someSortOfT& right) const
    {
        if(this==&right)
        {
            return true;
        }
        else
        {
            if(someString!=right.someString)
            {
                return false;
            }
            else
            {
                if(someChar==NULL)
                {
                    if(right.someChar==NULL)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    if(right.someChar == NULL)
                    {
                        return false;
                    }
                    else
                    {
                        return (strcmp(someChar, right.someChar)==0);
                    }
                }
            }
        }
    }

    virtual ~someSortOfT()
    {
        if(someChar!=NULL)
        {
            free(someChar);
            someChar=NULL;
        }
    }
    const char* getSomeChar() const
    {
        return someChar;
    }
    tstring getSomeString() const
    {
        return someString;
    }
};

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template <> static std::wstring ToString <someSortOfT>(const someSortOfT& q)
            {
                //return q.getSomeString();
                std::string source1(q.getSomeChar());
                std::wstring a(source1.begin(), source1.end());

                std::string source2(q.getSomeString());
                std::wstring b(source2.begin(), source2.end());

                std::wstring toBeReturned (a + b);

                return toBeReturned;
            }
        }
    }
}


someSortOfT g_STIM_FunctionThatTakesAnArrayOfObjects[10][10]; 
bool g_STIM_FunctionThatTakesAnArrayOfObjectsWasParameterNULL=false;
int g_STIM_FunctionThatTakesAnArrayOfObjectsNParameters=2;
int g_STIM_FunctionThatTakesAnArrayOfObjects_nCalls=0;
void STIM_FunctionThatTakesAnArrayOfObjects(_In_opt_count_(g_STIM_FunctionThatTakesAnArrayOfObjectsNParameters) someSortOfT* array)
{

    if(array==NULL)
    {
        g_STIM_FunctionThatTakesAnArrayOfObjectsWasParameterNULL = true;
    }
    else
    {
        if(g_STIM_FunctionThatTakesAnArrayOfObjects_nCalls<10)
        {
            for(int i=0;i<g_STIM_FunctionThatTakesAnArrayOfObjectsNParameters;i++)
            {
                g_STIM_FunctionThatTakesAnArrayOfObjects[g_STIM_FunctionThatTakesAnArrayOfObjects_nCalls][i]=array[i];
            }
            g_STIM_FunctionThatTakesAnArrayOfObjects_nCalls++;
        }
        else
        {
            ASSERT_FAIL(_T("catastrophic number of calls"));
        }
    }
}

DECLARE_STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAString, _In_z_ TCHAR*, someString);
DECLARE_STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAPointerToInt, _In_ int*, a);
DECLARE_STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAInt, _In_ int, a);
DECLARE_STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAnArray, _In_reads_opt_(g_STIM_FunctionThatTakesAnArrayNParameters) int*, a);
DECLARE_STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAConstArray, _In_reads_opt_(g_STIM_FunctionThatTakesAConstArrayNParameters) const int*, a);
//DECLARE_STIM_STATIC_TD_METHOD_4(, void, STIM_FunctionThatTakesTwoArrays, _In_ int*, array1, _In_ size_t, nArray1, _In_ int*,array2, _In_ size_t, nArray2 );
DECLARE_STIM_STATIC_TD_METHOD_4(, void, STIM_FunctionThatTakesTwoArrays, _In_reads_opt_(nArray1) int*, array1, _In_ size_t, nArray1, _In_reads_opt_(nArray2) int*,array2, _In_ size_t, nArray2 );
DECLARE_STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAnArrayOfObjects, _In_reads_opt_(g_STIM_FunctionThatTakesAnArrayOfObjectsNParameters) someSortOfT*, array);

TD_MOCK_CLASS(CMocksForParameterBuffers)
{
public:
    STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAString, _In_z_ TCHAR*, someString);
    STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAPointerToInt, _In_ int*, a);
    STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAInt, _In_ int, a);
    STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAnArray, _In_reads_opt_(g_STIM_FunctionThatTakesAnArrayNParameters) int*, a);
    STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAConstArray, _In_reads_opt_(g_STIM_FunctionThatTakesAConstArrayNParameters) const int*, a);
    STIM_STATIC_TD_METHOD_4(, void, STIM_FunctionThatTakesTwoArrays, _In_reads_opt_(nArray1) int*, array1, _In_ size_t, nArray1, _In_reads_opt_(nArray2) int*,array2, _In_ size_t, nArray2 );
    STIM_STATIC_TD_METHOD_1(, void, STIM_FunctionThatTakesAnArrayOfObjects, _In_reads_opt_(g_STIM_FunctionThatTakesAnArrayOfObjectsNParameters) someSortOfT*, array);
};

static HANDLE g_testByTest=NULL;

#endif
BEGIN_TEST_SUITE(MicroMockStimTest)

#ifdef _MSC_VER
    TEST_SUITE_INITIALIZE(a)
    {
            g_testByTest = CreateMutex(NULL, FALSE, NULL);
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(HANDLE, (HANDLE)NULL, g_testByTest, _T("Failed to create mutex for test by test"));
    }

    TEST_SUITE_CLEANUP(b)
    {
        (void)CloseHandle(g_testByTest);
    }

    TEST_FUNCTION_INITIALIZE(c)
    {
        if (WaitForSingleObject(g_testByTest, INFINITE) != WAIT_OBJECT_0)
        {
            ASSERT_FAIL(_T("Failed acquiring mutex for test by test."));
        }

        g_STIM_FunctionThatTakesAStringParameterCalls = 0;

        g_STIM_FunctionThatTakesAnArrayWasParameterNULL = false;
        g_STIM_FunctionThatTakesAnArrayNParameters = 0;
        g_STIM_FunctionThatTakesAnArray_nCalls = 0;
        g_STIM_FunctionThatTakesAConstArrayWasParameterNULL = false;
        g_STIM_FunctionThatTakesAConstArrayNParameters = 0;
        g_STIM_FunctionThatTakesAConstArray_nCalls = 0;
        g_STIM_FunctionThatTakesAnArrayOfObjects_nCalls = 0;

        s_someTaskThatCallsWhenTwoAtTimeEqualThree_nCalls = 0;
    }

        /*missing lock doesn't work with CppUnitTests TEST_FUNCTION_CLEANUP/ TEST_FUNCTION_INITIALIZE*/
        #pragma warning(disable: 26135)
    TEST_FUNCTION_CLEANUP(d)
    {
        if (ReleaseMutex(g_testByTest) == 0)
        {
            ASSERT_FAIL(_T("failure in test framework at ReleaseMutex"));
        }
    }

        TEST_FUNCTION(TimeDiscreteMicroMock_LoopbackAll)
        {
            ///arrange
            MyMocks3 mocks(theTask);
            const int maxTick=60;
            char *someString ="abc";
            int i;

            for(i=0;i<=maxTick;i++)
            {
                STIM_CALL_AT(mocks, i, zero());
                STRICT_EXPECTED_CALL_AT(mocks, i, whenzero());
            }

            for(i=0;i<=maxTick;i++)
            {
                STIM_CALL_AT(mocks, i, izero());
                STRICT_EXPECTED_CALL_AT(mocks,i, whenizero());
            }

            for(i=0;i<=maxTick;i++)
            {
                STIM_CALL_AT(mocks, i, one(i));
                STRICT_EXPECTED_CALL_AT(mocks, i, whenone(i+1));
            }

            for(i=0;i<=maxTick;i+=2)
            {
                STIM_CALL_AT(mocks, i, two("a", i));
                STRICT_EXPECTED_CALL_AT(mocks, i, whentwo("a", i+2));
            }

            for(i=0;i<=maxTick;i+=3)
            {
                STIM_CALL_AT(mocks, i, three('a', someString, i));
                STRICT_EXPECTED_CALL_AT(mocks,i, whenthree('d', someString, i+3));
            }

            for(i=0;i<=maxTick;i+=4)
            {
                STIM_CALL_AT(mocks, i, four((unsigned short)i*10, 'a', someString, i));
                STRICT_EXPECTED_CALL_AT(mocks,i, whenfour((unsigned short)i*10+4, 'e', someString, i+4));
            }

            for(i=0;i<=maxTick;i+=5)
            {
                STIM_CALL_AT(mocks, i, five(NULL, (unsigned short)i*10, 'a', someString, i));
                STRICT_EXPECTED_CALL_AT(mocks,i, whenfive(NULL, (unsigned short)i*10+5, 'f', someString, i+5));
            }

            for(i=0;i<=maxTick;i+=6)
            {
                STIM_CALL_AT(mocks, i, six((char)i, (char)(i+1), (char)(i+2), (char)(i+3), (char)(i+4), (char)(i+5)));
                STRICT_EXPECTED_CALL_AT(mocks,i, whensix((char)(i+6), (char)(i+7), (char)(i+8), (char)(i+9), (char)(i+10), (char)(i+11)));
            }

            ///act

            mocks.RunUntilTick(maxTick);
            ///assert - left to uM
        }




        TEST_FUNCTION(TimeDiscreteMicroMock_FileOrderIsPreserved_1)
        {
            ///arrange
            MyMocks3 mocks(theTask);
            posIndex=0;
            
            STIM_CALL_AT(mocks, 0, takes1int_1(1));
            STIM_CALL_AT(mocks, 0, takes1int_2(2));

            ///act
            mocks.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(int, 1, posRecorder[0]);
            ASSERT_ARE_EQUAL(int, 2, posRecorder[1]);
            
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_FileOrderIsPreserved_2)
        {
            ///arrange
            MyMocks3 mocks(theTask);
            posIndex=0;
            STIM_CALL_AT(mocks, 0, takes1int_2(1));
            STIM_CALL_AT(mocks, 0, takes1int_1(2));

            ///act
            mocks.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(int, 1, posRecorder[0]);
            ASSERT_ARE_EQUAL(int, 2, posRecorder[1]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAString(_T("alfa")));
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(LPCTSTR, (LPCTSTR)_T("alfa"), (LPCTSTR)g_STIM_FunctionThatTakesAStringParameter[0]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_2)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);

            ///act
            {
                TCHAR* stackString1 = _T("alfa");
                STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAString(stackString1));
            }

            {
                TCHAR* stackString1 = _T("beta");
                STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAString(stackString1));
            }
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(LPCTSTR, (LPCTSTR)_T("alfa"), (LPCTSTR)g_STIM_FunctionThatTakesAStringParameter[0]);
            ASSERT_ARE_EQUAL(LPCTSTR, (LPCTSTR)_T("beta"), (LPCTSTR)g_STIM_FunctionThatTakesAStringParameter[1]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_3)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);

            ///act
            TCHAR *var;
            {
                var= _T("alfa");
                STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAString(var));
            }

            {
                var= _T("beta");
                STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAString(var));
            }
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(LPCTSTR, (LPCTSTR)_T("alfa"), (LPCTSTR)g_STIM_FunctionThatTakesAStringParameter[0]);
            ASSERT_ARE_EQUAL(LPCTSTR, (LPCTSTR)_T("beta"), (LPCTSTR)g_STIM_FunctionThatTakesAStringParameter[1]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_4)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);

            ///act
            TCHAR var[100];
            {
                _tcscpy_s (var,_T("alfa"));
                STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAString(var));
            }

            {
                _tcscpy_s (var, _T("beta"));
                STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAString(var));
            }
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(LPCTSTR, (LPCTSTR)_T("alfa"), (LPCTSTR)g_STIM_FunctionThatTakesAStringParameter[0]);
            ASSERT_ARE_EQUAL(LPCTSTR, (LPCTSTR)_T("beta"), (LPCTSTR)g_STIM_FunctionThatTakesAStringParameter[1]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_5)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);

            ///act
            int var1, var2;
            {
                var1=1;
                var2=11;
                STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAPointerToInt(&var1));
                STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAInt(var2));
            }

            {
                var1=2;
                var2=22;
                STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAPointerToInt(&var1));
                STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAInt(var2));
            }
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(int, 1, g_STIM_FunctionThatTakesAPointerToInt[0]);
            ASSERT_ARE_EQUAL(int, 2, g_STIM_FunctionThatTakesAPointerToInt[1]);
            ASSERT_ARE_EQUAL(int, 11, g_STIM_FunctionThatTakesAInt[0]);
            ASSERT_ARE_EQUAL(int, 22, g_STIM_FunctionThatTakesAInt[1]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_understands_NULL_buffer)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAnArray(NULL));
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(bool, true, g_STIM_FunctionThatTakesAnArrayWasParameterNULL);
        }


        TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_understands_buffer_of_1_element)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int var=7;
            g_STIM_FunctionThatTakesAnArrayNParameters=1; /*since the STIM doesn't really know how many they are*/

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAnArray(&var));
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(int, 7, g_STIM_FunctionThatTakesAnArray[0][0]);        
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_ParameterBuffersAreSaved_understands_buffer_of_5_elements)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int var[5]={1,2,3,4,5};
            g_STIM_FunctionThatTakesAnArrayNParameters=5; /*since the STIM doesn't really know how many they are*/

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAnArray(var))
                .setArraySize(1, 5);
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(int, 1, g_STIM_FunctionThatTakesAnArray[0][0]);        
            ASSERT_ARE_EQUAL(int, 2, g_STIM_FunctionThatTakesAnArray[0][1]);    
            ASSERT_ARE_EQUAL(int, 3, g_STIM_FunctionThatTakesAnArray[0][2]);    
            ASSERT_ARE_EQUAL(int, 4, g_STIM_FunctionThatTakesAnArray[0][3]);    
            ASSERT_ARE_EQUAL(int, 5, g_STIM_FunctionThatTakesAnArray[0][4]);    
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_NULL_NULL)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesTwoArrays(NULL, 0, NULL, 0));
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(bool, true, NULL==STIM_FunctionThatTakesTwoArrays_array1_called_pointer);        
            ASSERT_ARE_EQUAL(bool, true, NULL==STIM_FunctionThatTakesTwoArrays_array2_called_pointer);        
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_NULL_1int)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int v2=2;

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesTwoArrays(NULL, 0, &v2, 1));
            m.RunUntilTick(0);

            //assert
            ASSERT_ARE_EQUAL(bool, true, NULL==STIM_FunctionThatTakesTwoArrays_array1_called_pointer);        
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array2_called_pointer);        
            ASSERT_ARE_EQUAL(int, 2, STIM_FunctionThatTakesTwoArrays_array2_copy[0]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_NULL_3int)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int v2[3]={21,22,23};

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesTwoArrays(NULL, 0, v2, 3))
                .setArraySize(3, 3);
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(bool, true, NULL==STIM_FunctionThatTakesTwoArrays_array1_called_pointer);        
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array2_called_pointer);        
            ASSERT_ARE_EQUAL(int, 21, STIM_FunctionThatTakesTwoArrays_array2_copy[0]);
            ASSERT_ARE_EQUAL(int, 22, STIM_FunctionThatTakesTwoArrays_array2_copy[1]);
            ASSERT_ARE_EQUAL(int, 23, STIM_FunctionThatTakesTwoArrays_array2_copy[2]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_1int_NULL)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int v1=3;

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesTwoArrays(&v1, 1, NULL, 0));
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array1_called_pointer);        
            ASSERT_ARE_EQUAL(bool, true, NULL==STIM_FunctionThatTakesTwoArrays_array2_called_pointer);        
            ASSERT_ARE_EQUAL(int, 3, STIM_FunctionThatTakesTwoArrays_array1_copy[0]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_1int_1int)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int v1=11;
            int v2=22;

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesTwoArrays(&v1, 1, &v2, 1));
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array1_called_pointer);        
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array2_called_pointer);        
            ASSERT_ARE_EQUAL(int, 11, STIM_FunctionThatTakesTwoArrays_array1_copy[0]);
            ASSERT_ARE_EQUAL(int, 22, STIM_FunctionThatTakesTwoArrays_array2_copy[0]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_1int_3int)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int v1=11;
            int v2[3]={22,23,24};

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesTwoArrays(&v1, 1, v2, 3))
                .setArraySize(3,3);
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array1_called_pointer);        
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array2_called_pointer);        
            ASSERT_ARE_EQUAL(int, 11, STIM_FunctionThatTakesTwoArrays_array1_copy[0]);
            ASSERT_ARE_EQUAL(int, 22, STIM_FunctionThatTakesTwoArrays_array2_copy[0]);
            ASSERT_ARE_EQUAL(int, 23, STIM_FunctionThatTakesTwoArrays_array2_copy[1]);
            ASSERT_ARE_EQUAL(int, 24, STIM_FunctionThatTakesTwoArrays_array2_copy[2]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_3int_NULL)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int v1[3]={11,13,15};
            
            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesTwoArrays(v1, 3, NULL, 0))
                .setArraySize(1,3);
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array1_called_pointer);        
            ASSERT_ARE_EQUAL(bool, true, NULL==STIM_FunctionThatTakesTwoArrays_array2_called_pointer);                    
            ASSERT_ARE_EQUAL(int, 11, STIM_FunctionThatTakesTwoArrays_array1_copy[0]);
            ASSERT_ARE_EQUAL(int, 13, STIM_FunctionThatTakesTwoArrays_array1_copy[1]);
            ASSERT_ARE_EQUAL(int, 15, STIM_FunctionThatTakesTwoArrays_array1_copy[2]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_3int_1int)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int v1[3]={11,13,15};
            int v2=99;

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesTwoArrays(v1, 3, &v2, 1))
                .setArraySize(1,3);
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array1_called_pointer);        
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array2_called_pointer);                    
            ASSERT_ARE_EQUAL(int, 11, STIM_FunctionThatTakesTwoArrays_array1_copy[0]);
            ASSERT_ARE_EQUAL(int, 13, STIM_FunctionThatTakesTwoArrays_array1_copy[1]);
            ASSERT_ARE_EQUAL(int, 15, STIM_FunctionThatTakesTwoArrays_array1_copy[2]);
            ASSERT_ARE_EQUAL(int, 99, STIM_FunctionThatTakesTwoArrays_array2_copy[0]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_with_2buffers_still_works_3int_4int)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int v1[3]={11,13,15};
            int v2[4]={23,25,27, 29};

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesTwoArrays(v1, 3, v2, 4))
                .setArraySize(1,3)
                .setArraySize(3,4);
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array1_called_pointer);        
            ASSERT_ARE_EQUAL(bool, false, NULL==STIM_FunctionThatTakesTwoArrays_array2_called_pointer);                    
            ASSERT_ARE_EQUAL(int, 11, STIM_FunctionThatTakesTwoArrays_array1_copy[0]);
            ASSERT_ARE_EQUAL(int, 13, STIM_FunctionThatTakesTwoArrays_array1_copy[1]);
            ASSERT_ARE_EQUAL(int, 15, STIM_FunctionThatTakesTwoArrays_array1_copy[2]);
            ASSERT_ARE_EQUAL(int, 23, STIM_FunctionThatTakesTwoArrays_array2_copy[0]);
            ASSERT_ARE_EQUAL(int, 25, STIM_FunctionThatTakesTwoArrays_array2_copy[1]);
            ASSERT_ARE_EQUAL(int, 27, STIM_FunctionThatTakesTwoArrays_array2_copy[2]);
            ASSERT_ARE_EQUAL(int, 29, STIM_FunctionThatTakesTwoArrays_array2_copy[3]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_can_be_reused)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            g_STIM_FunctionThatTakesAnArrayNParameters =2;
            
            ///act
            int var1[2]={11,12};
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAnArray(var1))
                .setArraySize(1, 2);
            var1[0]=22;
            var1[1]=23;
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAnArray(var1))
                .setArraySize(1, 2);
            m.RunUntilTick(1);

            //assert
            ASSERT_ARE_EQUAL(int, 11, g_STIM_FunctionThatTakesAnArray[0][0]);
            ASSERT_ARE_EQUAL(int, 12, g_STIM_FunctionThatTakesAnArray[0][1]);

            ASSERT_ARE_EQUAL(int, 22, g_STIM_FunctionThatTakesAnArray[1][0]);
            ASSERT_ARE_EQUAL(int, 23, g_STIM_FunctionThatTakesAnArray[1][1]);

        }

        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_can_be_reused_at_different_times)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            g_STIM_FunctionThatTakesAnArrayNParameters =2;

            ///act
            int var1[2]={11,12};
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAnArray(var1))
                .setArraySize(1, 2);
            var1[0]=22;
            var1[1]=23;
            STIM_CALL_AT(m, 1, STIM_FunctionThatTakesAnArray(var1))
                .setArraySize(1, 2);
            m.RunUntilTick(1);

            ///assert
            ASSERT_ARE_EQUAL(int, 11, g_STIM_FunctionThatTakesAnArray[0][0]);
            ASSERT_ARE_EQUAL(int, 12, g_STIM_FunctionThatTakesAnArray[0][1]);
            ASSERT_ARE_EQUAL(int, 22, g_STIM_FunctionThatTakesAnArray[1][0]);
            ASSERT_ARE_EQUAL(int, 23, g_STIM_FunctionThatTakesAnArray[1][1]);
        }

        void justForStackFrame(CMocksForParameterBuffers& m)
        {
            
            someSortOfT var1[2];
            var1[0]=someSortOfT(_T("thisIsTheString00"), "thisIsTheCharP00");
            var1[1]=someSortOfT(_T("thisIsTheString01"), "thisIsTheCharP01");
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAnArrayOfObjects(var1))
                .setArraySize(1,2);
        }

        /*found as a code review item*/
        TEST_FUNCTION(TimeDiscreteMicroMock_A_STIM_can_copy_an_array_of_objects)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            
            ///act
            for(int i=0;i<1;i++)
            {
                justForStackFrame(m); /*hopefully the stack content is destroyed here*/
            }
            m.RunUntilTick(0);

            ///assert
            someSortOfT var0=someSortOfT(_T("thisIsTheString00"), "thisIsTheCharP00");
            someSortOfT var1=someSortOfT(_T("thisIsTheString01"), "thisIsTheCharP01");
            ASSERT_ARE_EQUAL(someSortOfT, var0, g_STIM_FunctionThatTakesAnArrayOfObjects[0][0]);
            ASSERT_ARE_EQUAL(someSortOfT, var1, g_STIM_FunctionThatTakesAnArrayOfObjects[0][1]);
        }

        /*found from code review*/
        TEST_FUNCTION(TimeDiscreteMicroMock_FailsWhenUserCallsDirectly_setArraySize)
        {
            CMocksForParameterBuffers m(someTask);
            try
            {
                m.STIM_FunctionThatTakesAnArrayOfObjects.setArraySize(1,2);
            }
            catch(CMicroMockException& c)
            {
                ASSERT_ARE_EQUAL_WITH_MSG(MICROMOCK_EXCEPTION, MICROMOCK_EXCEPTION_SET_ARRAY_SIZE_BEFORE_CALL, c.GetMicroMockExceptionCode(), _T("CMicroMockException was thrown, but the expected code was not right"));
            }
            catch(...)
            {
                ASSERT_FAIL(_T("unexpected exception thrown, was excepting CMicroMock with code MICROMOCK_EXCEPTION_SET_ARRAY_SIZE_BEFORE_CALL"));
            }
        }

        /*found from code review*/
        TEST_FUNCTION(TimeDiscreteMicroMock_FailsWhenUserCallsDirectly_setTime)
        {
            CMocksForParameterBuffers m(someTask);
            try
            {
                m.STIM_FunctionThatTakesAnArrayOfObjects.SetTime(1,1);
            }
            catch(CMicroMockException& c)
            {
                ASSERT_ARE_EQUAL_WITH_MSG(MICROMOCK_EXCEPTION, MICROMOCK_EXCEPTION_SET_TIME_BEFORE_CALL, c.GetMicroMockExceptionCode(), _T("CMicroMockException was thrown, but the expected code was not right"));
            }
            catch(...)
            {
                ASSERT_FAIL(_T("unexpected exception thrown, was excepting CMicroMock with code MICROMOCK_EXCEPTION_SET_TIME_BEFORE_CALL"));
            }
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_Const_ParameterBuffersAreSaved_understands_NULL_buffer)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAConstArray(NULL));
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(bool, true, g_STIM_FunctionThatTakesAConstArrayWasParameterNULL);
        }


        TEST_FUNCTION(TimeDiscreteMicroMock_Const_ParameterBuffersAreSaved_understands_buffer_of_1_element)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int var=7;
            g_STIM_FunctionThatTakesAConstArrayNParameters=1; /*since the STIM doesn't really know how many they are*/

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAConstArray(&var));
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(int, 7, g_STIM_FunctionThatTakesAConstArray[0][0]);        
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_Const_ParameterBuffersAreSaved_understands_buffer_of_5_elements)
        {
            ///arrange
            CMocksForParameterBuffers m(someTask);
            int var[5]={1,2,3,4,5};
            g_STIM_FunctionThatTakesAConstArrayNParameters=5; /*since the STIM doesn't really know how many they are*/

            ///act
            STIM_CALL_AT(m, 0, STIM_FunctionThatTakesAConstArray(var))
                .setArraySize(1, 5);
            m.RunUntilTick(0);

            ///assert
            ASSERT_ARE_EQUAL(int, 1, g_STIM_FunctionThatTakesAConstArray[0][0]);        
            ASSERT_ARE_EQUAL(int, 2, g_STIM_FunctionThatTakesAConstArray[0][1]);
            ASSERT_ARE_EQUAL(int, 3, g_STIM_FunctionThatTakesAConstArray[0][2]);
            ASSERT_ARE_EQUAL(int, 4, g_STIM_FunctionThatTakesAConstArray[0][3]);
            ASSERT_ARE_EQUAL(int, 5, g_STIM_FunctionThatTakesAConstArray[0][4]);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_baseline)
        {
            MyMocks3 m(someTaskThatCallsWhenTwoAtTimeEqualThree);
            STRICT_EXPECTED_CALL_AT(m,3,  whentwo("two", -4));
            m.RunUntilTick(10);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_using_without_strict_with_real_time)
        {
            MyMocks3 m(someTaskThatCallsWhenTwoAtTimeEqualThree);
            EXPECTED_CALL_AT(m, 3,  whentwo("two_tee_at_two", +4)); /*EXPECTED just makes hte call, doesn't validate parameters*/
            m.RunUntilTick(10);
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_time_mismatch_is_detected)
        {
            MyMocks3 m(someTaskThatCallsWhenTwoAtTimeEqualThree);
            STRICT_EXPECTED_CALL_AT(m, 4,  whentwo("two", -4));
            m.RunUntilTick(10);
            ASSERT_ARE_EQUAL(tstring, tstring(_T("[Expected:whentwo(two,-4,4)][Actual:whentwo(two,-4,3)]")), m.CompareActualAndExpectedCalls());
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_time_mismatch_is_detected_without_STRICT)
        {
            /*I want this to fail because time doesn't match*/
            MyMocks3 m(someTaskThatCallsWhenTwoAtTimeEqualThree);
            EXPECTED_CALL_AT(m, 4,  whentwo("two_tee_at_two", +4)); 
            m.RunUntilTick(10);
            ASSERT_ARE_EQUAL(tstring, tstring(_T("[Expected:whentwo(two_tee_at_two,4,4)][Actual:whentwo(two,-4,3)]")), m.CompareActualAndExpectedCalls());
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_IgnoreArgument)
        {
            /*this must fail, since time is not ignore-able*/
            MyMocks3 m(someTaskThatCallsWhenTwoAtTimeEqualThree);
            STRICT_EXPECTED_CALL_AT(m, 4,  whentwo("two", -4))
                .IgnoreArgument(3);
            m.RunUntilTick(10);
            ASSERT_ARE_EQUAL(tstring, tstring(_T("[Expected:whentwo(two,-4,4)][Actual:whentwo(two,-4,3)]")), m.CompareActualAndExpectedCalls());
        }

        TEST_FUNCTION(TimeDiscreteMicroMock_should_never_ignore_time_IgnoreAllArguments)
        {
            /*I want this to be user test code error*/
            MyMocks3 m(someTaskThatCallsWhenTwoAtTimeEqualThree);
            STRICT_EXPECTED_CALL_AT(m, 4,  whentwo("two", -4))
                .IgnoreAllArguments();
            m.RunUntilTick(10);
            ASSERT_ARE_EQUAL(tstring, tstring(_T("[Expected:whentwo(two,-4,4)][Actual:whentwo(two,-4,3)]")), m.CompareActualAndExpectedCalls());
        }
#endif
        END_TEST_SUITE(MicroMockStimTest);

