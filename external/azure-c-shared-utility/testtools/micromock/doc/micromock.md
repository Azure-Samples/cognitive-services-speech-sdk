MicroMock 
  
#Why?
Micromock is intended to fulfill the needs of the (connected car networking) developers in mocking dependencies. The following are the main goals: 
- Allow runtime mocking by using Detours, since this is the main case where mocking is really needed. 
- Allow a less verbose mode of declaring the mock classes. The main goal here is that for one mocked API, one line of code should be written into the mock class and the line of code should resemble as much as possible the original mocked API. 
- Writing tests with the framework should not require verbosity. 
- Comparing expected calls and actual calls should explicitly state what the mismatch is.
 
#Cheat sheet 
##Declaring a mock class 
 ```c
TYPED_MOCK_CLASS(CMultimediaTimerMock, CRuntimeMock) 
{ 
public: 
    MOCK_HOOK_METHOD_5(WINAPI, timeSetEvent, MMRESULT, UINT, uDelay, UINT, uResolution, LPTIMECALLBACK, lpTimeProc, DWORD_PTR, dwUser, UINT, fuEvent) 
    MOCK_METHOD_END(MMRESULT, 0x0) 
}; 
```
The above mock class contains one static method that expects 5 arguments and returns a value of type MMRESULT. The method is prefixed with WINAPI in order to specify the __stdcall calling convention. 
Also the mock method by default returns 0, as specified by the MOCK_METHOD_END macro.
 
##Instantiating mocks 
 
CRuntimeMock<CMultimediaTimerMock, CNiceCallComparer> multimediaTimerMock; 


##Specifying expected calls in a test 
Specifying an expected call is done by using the EXPECTED_CALL macro:
```c 
EXPECTED_CALL(multimediaTimerMock, timeSetEvent(0, TEST_MILLISECONDS_DELAY, NULL, NULL, TIME_CALLBACK_FUNCTION)); 
```

##Overriding a return value for an expected call 
In order to instruct the mock to return a specific value for an expected call, one can specify the SetReturn modifier on the expected call:
```c 
EXPECTED_CALL(multimediaTimerMock, timeSetEvent(0, TEST_MILLISECONDS_DELAY, NULL, NULL, TIME_CALLBACK_FUNCTION)) 
.SetReturn(42); 
```

##Overriding a return value for an expected call with a fail return value 
In order to instruct the mock to return a specific value for an expected call, one can specify the SetFailReturn modifier on the expected call:
```c 
EXPECTED_CALL(multimediaTimerMock, timeSetEvent(0, TEST_MILLISECONDS_DELAY, NULL, NULL, TIME_CALLBACK_FUNCTION)) 
.SetFailReturn(42); 
```
In this case, the code (if any) in body of the mock shall not be executed and the mock shall return the fail return value 
as soon as it has matched the call.
 
##A simple test 
```c
TEST_METHOD(When_The_Multimedia_Timer_Creation_Is_Successful_DoWork_Succeeds) 
{ 
// arrange 
       CClassUnderTest classUnderTest; 
 
       CMultimediaTimerMock multimediaTimerMock; 
 
       EXPECTED_CALL(multimediaTimerMock, timeSetEvent(0, TEST_MILLISECONDS_DELAY, NULL, NULL, TIME_CALLBACK_FUNCTION)) 
        // classUnderTest.DoWork will see 0x42 as result to the timeSetEvent call 
.SetReturn(0x42); 
 
// act 
BOOL result = classUnderTest.DoWork(TEST_MILLISECONDS_DELAY); 
 
// assert 
Assert::AreEqual(TRUE, result, _T("DoWork should succeed when the timeSetEvent succeeds.")); 
} 
```
#Mock classes 
In order to declare a mock class, the TYPED_MOCK_CLASS macro can be used:
```c 
TYPED_MOCK_CLASS(CTestMock, CRuntimeMock) 
{ 
public: 
... 
}; 
```
Then the instantiation of the mock in the test looks like: 
```c
CTestMock testMock;
``` 
There are 4 types of mocks: 
##CMock 
This mock type is the simplest. It provides the ability to setup expected calls, record actual calls and compare the 2 lists. 
##CThreadSafeGlobalMock 
This mock type provides all the CMock functionality and in addition it ensures that only one instance of the same mock exists at one time by using a mutex. 
##CRuntimeMock 
This mock type provides all the CThreadSafeGlobalMock functionality and in addition it allows installing hooks by using Detours.
#Mock methods 
All mock methods start with one of the following macros: MOCK_METHOD_x, MOCK_STATIC_METHOD_x, MOCK_HOOK_METHOD_x or MOCK_HOOK_METHOD_EX_x, where x is the number of arguments for the mock method. 
Currently methods with up to 10 arguments are supported. 
A mock method without any arguments uses the MOCK_METHOD_0 macro. 
##MOCK_METHOD_x 
```c
MOCK_METHOD_x(prefix, returnType, name, arg1Type, arg1Value, arg2Type, …)
``` 
declares a mock class member method. 
##MOCK_STATIC_METHOD_x 
```c
MOCK_STATIC_METHOD_x(prefix, returnType, name, arg1Type, arg1Value, arg2Type, …)
``` 
declares a mock class member static method. A static method can only be used in a CThreadSafeGlobalMock or CRuntimeMock mock class. 
##MOCK_HOOK_METHOD_x
```c 
MOCK_HOOK_METHOD_x(prefix, returnType, name, arg1Type, arg1Value, arg2Type, …)
``` 
declares a mock class member static method and declares that the global namespace function with the same name will be hooked/detoured to the declared mock method. A hook method can only be used in a CRuntimeMock mock class. 
##MOCK_HOOK_METHOD_EX_x
```c 
MOCK_HOOK_METHOD_EX_x(prefix, returnType, name, arg1Type, arg1Value, arg2Type, …, functionToHook)
``` 
declares a hook mock method and additionally instructs the framework to hook a specific function (functionToHook) and redirect all calls to the mock method. 
#Actual vs Expected call comparison 
##CompareActualAndExpectedCalls 
In order to get the result of the comparison of actual and expected calls, a call to CompareActualAndExpectedCalls can be made and the resulting string can be directly used in an assert statement.
```c 
tstring result = multimediaTimerMock.CompareActualAndExpectedCalls(); 
Assert::AreEqual(tstring(_T("")), result, 
        _T("Incorrect mock function call result")); 
```

If a call was expected and no actual call was matched with it CompareActualAndExpectedCalls indicates the missing call in a format like “[Missing:timeSetEvent(42)]”. 
If a call was not expected, but an actual call occurred (the code under test called a mock function when not expected), CompareActualAndExpectedCalls indicates the unexpected call in a format like “[Unexpected:timeSetEvent(42)]”. 
##GetUnexpectedCalls/GetMissingCalls 
In order to facilitate writing asserts in a natural way, the following syntax can also be used:
```c 
Assert::AreEqual(testMock.GetMissingCalls(),testMock.GetUnexpectedCalls(), 
_T("Incorrect mock function call result"));
``` 
##Arguments in call comparisons 
When comparing an expected call and an actual call, arguments are taken into account according to what has been setup on the expected call. 
Using EXPECTED_CALL states that an expected call does not compare any arguments by default. The expected call can be modified to force argument comparison by using the ValidateArgument modifier. 
Using STRICT_EXPECTED_CALL states an expected call compares all arguments by default. To exclude specific arguments from the comparison the IgnoreArgument modifier can be used. 
ValidateArgument and IgnoreArgument are mutually exclusive. If both of them are used on the same expected call, the last one sticks.
##Special handling for some argument types 
###Structures 
Since functions can have arguments of other types than the basic ones, sometimes there is a need for specialized comparison. If a mocked function takes a structure argument for example, comparing the contents of the structure should only compare the members, not all the bytes in the structure. 
For this case in order for the framework to do the comparison and conversion of the call arguments to string, 2 operators are required to be implemented: 
- The equality operator 
- The stream insertion << operator 
###Defining pointer equality operators 
In order to allow the mocking framework to directly compare pointer types, the equality operator for the type itself needs to be defined.
```c  
_Must_inspect_result_ 
static bool operator==(_In_ SOCKADDR const& lhs, _In_ SOCKADDR const& rhs) 
{ 
    if (lhs.sa_family != rhs.sa_family) 
    { 
        return false; 
    } 

 
    if (lhs.sa_family == AF_INET) 
    { 
        return … ; // more comparison of the structure members should be done here 
    } 
 
    return (memcmp(lhs.sa_data, rhs.sa_data, sizeof(rhs.sa_data)) == 0); 
} 
```
Additionally a specialization of the == operator for the MockValue class (internally used by the framework) should be provided: 
MICROMOCK_POINTER_TYPE_EQUAL(SOCKADDR const); 
Then any test can specify the arguments of the type SOCKADDR const* to be compared as regular arguments in any expected function calls. 
 
##Call comparers 
 
Several call comparers are available for performing the comparison of expected calls and actual calls. If no comparer is specified then the StrictUnorderedCallComparer is used. 
###StrictUnorderedCallComparer 
This comparer checks that every actual call has a matching expected call and every expected call has a matching actual call, but without caring about the order in which they appeared. 
Practically, every time an actual call is made by the code under test, the first matching expected call (that was previously not matched) is paired. The search happens in the order in which the expected calls are defined in the test. 
###StrictOrderedCallComparer 
This comparer checks that every actual call has a matching expected call, making sure the actual calls appear in the order specified by the expected calls written in the test. 
###NiceCallComparer 
This comparer checks that every expected call has a matching actual call, and it completely ignores the unexpected calls from the code under test. Also, the order in which the calls happen is not relevant for the comparison. 
Example test: 
```c
TEST_METHOD(NiceCallComparer_Ignores_Unexpected_Calls) 
{ 
// arrange 
CNiceCallComparer<CTestStaticMock > testMock; 
 
FunctionUnderTest() // invokes CTestStaticMock::StaticTestFunction(43); 
 
// act 

tstring result = testMock.CompareActualAndExpectedCalls(); 
 
// assert 
Assert::AreEqual(tstring(_T("")), result, 
_T("Incorrect mock function call result")); 
} 
```
 
##Resetting expected and actual calls 
It is possible to reset the list of actual calls maintained by a mock object by a call to ResetActualCalls. This allows handling corner cases where executing code under test is desired and actual calls should only be taken into account from a certain point on in the test. 
It is also possible to reset the expected calls by issuing a call to ResetExpectedCalls. This feature is only provided for symmetry with resetting the actual calls. 
If it is desired to completely reset a mock, this can be achieved by a call to ResetAllCalls, which essentially resets both the actual and expected call list. 
Good tests do not typically require these features, so they should be used cautiously. 
#Non-compare actions 
In some cases it is needed to specify that a certain result is returned by a mocked function from a given point of the test execution forward, without actually checking that actual calls exist to match that call. 
A typical example is setting up the time for a piece of code under test. For example, let’s take the case when the code under test uses GetTickCount64 to retrieve the current system tick count. The test needs to inject a known tick count value to be used by all calls coming from the production code, so that any time dependency and flakiness of the test can be eliminated. 
Also the test needs in some cases to inject different values of the time to simulate passing of time and test how the code under test behaves. 
For these cases the construct WHEN_CALLED can be used, as shown below:
```c 
TEST_METHOD(When_Called_Is_Not_Used_In_Call_Comparison) 
{ 
// arrange 
CTestStaticMock tickCountMock; 
 
WHEN_CALLED(tickCountMock, GetTickCount64()) 
.SetReturn(42); 
 
FunctionUnderTest(); // invokes GetTickCount64() 7 times and all 7 calls return 42 
 
// act 
tstring result = testMock.CompareActualAndExpectedCalls(); 
 
// assert 
Assert::AreEqual(tstring(_T("")), result, 

_T("Incorrect mock function call result")); 
} 
```

WHEN_CALLED does not validate argument values by default. 
Also WHEN_CALLED_STRICT is available, which will by default validate all the call arguments. If the arguments do not match then the action specified by WHEN_CALLED (for example returning a specific value) is ignored. 
#Custom mocks 
It is possible to write custom mock code directly in the mock class.
```c 
TYPED_MOCK_CLASS(CTestStaticMock, CRuntimeMock) 
{ 
public: 
    MOCK_STATIC_METHOD_1(, UINT8, StaticTestFunctionWith1Arg, UINT8, arg1) 
        if (4200 == arg1) 
        { 
            return 45; 
        } 
    MOCK_METHOD_END(UINT8, 0) 
}; 
```
#Automatic runtime hooks installation 
In order to avoid installing the hooks in every test, it is possible to specify in the class declaration which hooks should be installed:
```c 
TYPED_MOCK_CLASS(CTestStaticMock, CRuntimeMock) 
{ 
public: 
    MOCK_HOOK_METHOD_0(, UINT64, GetTickCount64) 
    MOCK_METHOD_END(UINT64, 0) 
}; 
```

Runtime hooks work only with the CRuntimeMock class. 
Hooks can be declared by : 
- specifying mock hook methods (with MOCK_HOOK_METHOD_x or MOCK_HOOK_METHOD_EX_x) 
- using the INSTALL_HOOK macro. 
 
The INSTALL_HOOK macro instructs the framework to hook a function every time the mock class is instantiated. Example: 
```c 
TYPED_MOCK_CLASS(CTestStaticMock, CRuntimeMock) 
{ 
public: 
    MOCK_HOOK_METHOD_0(, UINT64, MyGetTickCount64) 
    MOCK_METHOD_END(UINT64, 0) 
 
    INSTALL_HOOK(::GetTickCount64, MyGetTickCount64) 
}; 
```

  
All hooks specified by MOCK_HOOK_xxx methods or INSTALL_HOOK are automatically reverted when the mock class is destroyed. 
#Expectation modifier details 
##SetReturn 
This modifier allows specifying that for a given expected call a certain value should be returned, overriding the default value that would normally be returned by the mock function. 
Example test: 
```c
TYPED_MOCK_CLASS(CTestStaticMock, CThreadSafeGlobalMock) 
{ 
public: 
    MOCK_STATIC_METHOD_0(, UINT8, StaticTestFunction) 
    MOCK_METHOD_END(UINT8, 1) 
}; 
 
UINT8 FunctionUnderTest() 
{ 
    return StaticTestFunction(); 
} 
 
TEST_METHOD(SetReturn_Overrides_The_Return_Value_Of_The_Mock) 
{ 
// arrange 
CTestStaticMock testMock; 
 
STRICT_EXPECTED_CALL(testMock, StaticTestFunction()) 
.SetReturn(42); 
 
// act 
UINT8 result = FunctionUnderTest(); // StaticTestFunction returns 42 
 
// assert 
Assert::AreEqual((UINT8)42, result, 
_T("Incorrect mock function call result")); 
} 
```

Test result: Pass 
##NeverInvoked 
This expectation modifier specifies that the call is not expected. Should the call occur, the call comparison will return a mismatch, explicitly stating that the call was unexpected. 
Example test:
```c 
TEST_METHOD(NeverInvoked_When_An_Actual_Call_Is_Made_Indicates_A_Mismatch) 
{ 
// arrange 
CTestStaticMock testMock; 

 
STRICT_EXPECTED_CALL(testMock, StaticTestFunction()) 
.NeverInvoked(); 
 
FunctionUnderTest(); // invokes StaticTestFunction() 
 
// act 
tstring result = testMock.CompareActualAndExpectedCalls(); 
 
// assert 
Assert::AreEqual(tstring(_T("")), result, 
_T("Incorrect mock function call result")); 
} 
```

Test result: 
Assert failed. Expected:<> Actual:<[Unexpected: CTestStaticMock::StaticTestFunction()]> - Incorrect mock function call result 
##IgnoreArgument 
This expectation modifier specifies that a specific argument is to be ignored when matching expected and actual calls. The argument is identified by its index, starting with the first argument having index 1. 
Example test:
```c 
TEST_METHOD(IgnoreArgument_Ignores_An_Argument_By_Its_Index) 
{ 
// arrange 
CTestStaticMock testMock; 
 
STRICT_EXPECTED_CALL(testMock, StaticTestFunction(42)) 
.IgnoreArgument(1); 
 
FunctionUnderTest(); // invokes StaticTestFunction(43) 
 
// act 
tstring result = testMock.CompareActualAndExpectedCalls(); 
 
// assert 
Assert::AreEqual(tstring(_T("")), result, 
_T("Incorrect mock function call result")); 
} 
```

Test result: Pass 
##ValidateArgument 
This expectation modifier specifies that a specific argument is to be validated when matching expected and actual calls. The argument is identified by its index, starting with the first argument having index 1. 
Example test:
```c 
TEST_METHOD(ValidateArgument_Forces_Validation_For_An_Argument_By_Its_Index) 
{ 

// arrange 
CTestStaticMock testMock; 
 
EXPECTED_CALL(testMock, StaticTestFunction(42)) 
.ValidateArgument(1); 
 
FunctionUnderTest(); // invokes StaticTestFunction(43) 
 
// act 
tstring result = testMock.CompareActualAndExpectedCalls(); 
 
// assert 
Assert::AreEqual(tstring(_T("")), result, 
_T("Incorrect mock function call result")); 
} 
```

Test result: 
Assert failed. Expected:<> Actual:<[Unexpected: CTestStaticMock::StaticTestFunction(42)] [Missing: CTestStaticMock::StaticTestFunction(43)]> - Incorrect mock function call result 
##ValidateArgumentBuffer 
This expectation modifier specifies that a specific argument is to be treated as a pointer and its buffer contents should be validated against a user supplied buffer. 
The modifier takes as arguments the argument number, the expected contents as a buffer, the number of bytes to compare and an offset where the comparison should start.  
The expected data to be compared against is copied by the mocking framework internally, so there is no need to maintain that data unchanged for the entire test lifetime. 
Example test:
```c 
TEST_METHOD(MicroMock_ValidateArgumentBuffer_When_Expected_Memory_Does_Not_Match_Indicate s_A_Call_Mismatch) 
{ 
// arrange 
CTestOutArgMock testMock; 
UINT8 expectedBuffer[1] = {43}; 
UINT8 actualBuffer [1] = {42}; 
 
STRICT_EXPECTED_CALL(testMock, TestFunction(NULL)) 
.ValidateArgumentBuffer(1, expectedBuffer, sizeof(expectedBuffer)); 
 
// act 
testMock.TestFunction(actualBuffer); 
 
// assert 
Assert::AreNotEqual(tstring(_T("")), testMock.CompareActualAndExpectedCalls(), 
_T("Incorrect mock function call result")); 
} 
```
 
Test result: 

Assert failed.  
To compare only the second byte of a 2 bytes out argument’s buffer, a call like below would be used, indicating to start the compare at offset 1: 
STRICT_EXPECTED_CALL(testMock, TestFunction(expectedBuffer)) 
.ValidateArgumentBuffer(1, expectedBuffer, 1, 1); 
##CopyOutArgumentBuffer 
This expectation modifier specifies that data should be copied into the buffer pointed to by a specific argument. 
The modifier takes as arguments the argument number, the data to be injected, the number of bytes to be injected and an offset where the data copy into the out argument buffer should start. 
The supplied data to be injected is copied by the mocking framework internally, so there is no need to maintain that data unchanged for the entire test lifetime. 
Example test: 
```c
TEST_METHOD(MicroMock_CopyOutArgumentBuffer_Only_First_Byte_Out_Of_2_Bytes_Is_Copied) 
{ 
// arrange 
       CTestArgBufferMock testMock; 
       UINT8 injectedOutBuffer[2] = {0x42, 0x42}; 
       UINT8 actualArgumentBuffer[2] = {0, 0}; 
 
       STRICT_EXPECTED_CALL(testMock, TestFunction(NULL)) 
        .CopyOutArgumentBuffer(1, injectedOutBuffer, 1); 
 
// act 
       testMock.TestFunction(actualArgumentBuffer); 
 
// assert 
       Assert::AreEqual(tstring(_T("[42 0]")), BytesToString(actualArgumentBuffer, sizeof(actualArgumentBuffer)), 
        _T("Only the first byte should be copied")); 
} 
``` 
Test result: 
Pass.  
To compare only the second byte of a 2 bytes out argument’s buffer, a call like below would be used, indicating to start the data copy at offset 1:
```c 
STRICT_EXPECTED_CALL(testMock, TestFunction(NULL)) 
        .CopyOutArgumentBuffer(1, injectedOutBuffer, 1, 1);
``` 
