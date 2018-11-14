# Adding Carbon Unit Tests 

For the time being, we're using Catch2 as a unit-testing framework. The main reason being, that it's cross-platform and header-only, so extremely easy to get it up and running. 

When creating a new unit-test project just put `$(SolutionDir)external\Catch2;`as an additional include directory, and add the header: `#include "catch.hpp"`. If the project consists of a single cpp file, also add this macro: `#define CATCH_CONFIG_MAIN`. If there are multiple source files, it's a good idea to have a separate main.cpp with both of those (the include and the macro). See ./functional/cxx/core for an example of a unit-test project that uses Catch2.

To get more info, please take a look at the Catch2 [git repo](https://github.com/catchorg/Catch2). 


