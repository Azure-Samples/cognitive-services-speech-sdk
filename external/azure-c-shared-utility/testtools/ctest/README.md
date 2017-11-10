# ctest

azure-ctest is a simple portable C test runner.

## Setup

1. Clone **azure-ctest**

2. Create a folder called *cmake*. This can be any other folder name, there are no restrictions on using specifically cmake as folder name.

3. Switch to the *cmake* folder and run
```
cmake ..
```

## Example

```c
#include "ctest.h"
#include "SomeUnitUnderTest.h"

CTEST_BEGIN_TEST_SUITE(SimpleTestSuiteOneTest)

CTEST_FUNCTION(Test1)
{
    // arrange

    // act
    int x = SomeFunction();

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 42, x);
}

CTEST_END_TEST_SUITE(SimpleTestSuiteOneTest)
```
