//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// guid_utils.cpp: Utility classes/functions dealing with GUIDs
//

#include "platform.h"

// The following snippet comes from
// https://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
#endif


namespace PAL {

#ifdef __GNUG__
std::string demangle(const char* name) {

int status = -4; // some arbitrary value to eliminate the compiler warning
std::unique_ptr<char, void(*)(void*)> res{
    abi::__cxa_demangle(name, NULL, NULL, &status),
    std::free
};

return (status == 0) ? res.get() : name;
}

#else

// does nothing if not g++
std::string demangle(const char* name) {
    return name;
}

#endif

} // PAL
