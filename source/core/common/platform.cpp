//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// guid_utils.cpp: Utility classes/functions dealing with GUIDs
//

#include "platform.h"

#include <string_utils.h>
#include <sstream>
#include <tuple>

#if defined(_MSC_VER)
#include <Windows.h>
#include <VersionHelpers.h>

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP)
#include <winrt/base.h>
#include <winrt/Windows.System.Profile.h>
#endif

#elif defined(__linux__)
#include <sys/utsname.h>
#elif defined(__MACH__)
#include <sys/param.h>
#include <sys/sysctl.h>
#endif

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

#if defined(_MSC_VER)

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) // Desktop

OperatingSystemInfo getOperatingSystem()
{
    OperatingSystemInfo result { "Windows", "unknown", "unknown" };

    if (IsWindows10OrGreater())
    {
        result.version = "10";
    }
    else if (IsWindows8Point1OrGreater())
    {
        result.version = "8.1";
    }
    else if (IsWindows8OrGreater())
    {
        // Note: this will also be returned if Windows shims the version.
        result.version = "8";
    }

    if (IsWindowsServer())
    {
        result.name = "Server";
    }
    else
    {
        result.name = "Client";
    }
    return result;
}

#else // Windows Store WinRT app

OperatingSystemInfo getOperatingSystem()
{
    OperatingSystemInfo result { "Windows UWP", "unknown", "unknown" };

    try
    {
        auto versionInfo = winrt::Windows::System::Profile::AnalyticsInfo::VersionInfo();
        result.name = winrt::to_string(versionInfo.DeviceFamily());
        uint64_t versionCode = std::strtoull(winrt::to_string(versionInfo.DeviceFamilyVersion()).c_str(), nullptr, 10);
        std::stringstream versionSs;

        for (int i = 0; i < 4; i++, versionCode <<= 16) {
            versionSs << (versionCode >> 48);
            if (i < 3) {
                versionSs << '.';
            }
        }
        result.version = versionSs.str();
    }
    catch (winrt::hresult_error const& /*ex*/)
    {
        // Ignore exceptions thrown via WinRT APIs.
    }
    return result;
}

#endif

#elif defined(__linux__)

OperatingSystemInfo getOperatingSystem()
{
    OperatingSystemInfo result { "Linux", "unknown", "unknown" };
    struct utsname u;
    std::stringstream nameSs;
    if (uname(&u) == 0)
    {
        result.platform = u.sysname;
        nameSs << u.sysname << " " << u.release << " " << u.version << " " << u.machine; // "uname -s -r -v -m"
        result.name = nameSs.str();
        result.version = u.release;
    }
    return result;
}

#elif defined(__MACH__)

OperatingSystemInfo getOperatingSystem()
{
    int mib[] = { CTL_KERN, KERN_OSRELEASE };
    size_t len;
    sysctl(mib, 2, NULL, &len, NULL, 0);
    auto kernOsRelease = std::make_unique<char[]>(len);
    sysctl(mib, 2, kernOsRelease.get(), &len, NULL, 0);
    return {"Darwin", "Darwin", kernOsRelease.get()};
}

#endif

} // PAL
