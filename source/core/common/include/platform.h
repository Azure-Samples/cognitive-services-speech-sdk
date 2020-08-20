//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once


#include <string>
#include <typeinfo>

namespace PAL {

std::string demangle(const char* name);

template <class T>
std::string GetTypeName() {
    return demangle(typeid(T).name());
}

template <class T>
std::string GetTypeName(const T& t) {
    return demangle(typeid(t).name());
}

struct OperatingSystemInfo {
    std::string platform;
    std::string name;
    std::string version;
};

OperatingSystemInfo getOperatingSystem();

std::string SpxGetEnv(const char* name, const char* defaultValue);

} // PAL
