//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "test_utils.h"
#include "string_utils.h"


SPXTEST_CASE_BEGIN("strcpy and wcscpy", "[core][string utils]")
{
    const size_t srcSize = 1024;
    const size_t dstSize = srcSize / 2;
    const size_t maxCopySize = dstSize - 1;
    char src[srcSize];
    char dst[dstSize];
    wchar_t wsrc[srcSize];
    wchar_t wdst[dstSize];

    for (size_t i = 0; i < srcSize; i++)
    {
        src[i] = (char)(i % 255 + 1);
        wsrc[i] = (wchar_t)(i % 255 + 1);
    }

    SPXTEST_SECTION("truncate: true")
    {
        // source not null-terminated

        // strcpy
        dst[maxCopySize] = 1;
        REQUIRE_NOTHROW(PAL::strcpy(dst, dstSize, src, srcSize, true));
        SPXTEST_REQUIRE(PAL::strnicmp(src, dst, maxCopySize) == 0);
        SPXTEST_REQUIRE(dst[maxCopySize] == 0);

        // wcscpy
        wdst[maxCopySize] = 1;
        REQUIRE_NOTHROW(PAL::wcscpy(wdst, dstSize, wsrc, srcSize, true));
        SPXTEST_REQUIRE(PAL::wcsnicmp(wsrc, wdst, maxCopySize) == 0);
        SPXTEST_REQUIRE(wdst[maxCopySize] == 0);

        // source null-terminated at destination length

        // strcpy
        src[maxCopySize] = 0;
        dst[maxCopySize] = 1;
        REQUIRE_NOTHROW(PAL::strcpy(dst, dstSize, src, srcSize, true));
        SPXTEST_REQUIRE(PAL::strnicmp(src, dst, dstSize) == 0);
        SPXTEST_REQUIRE(dst[maxCopySize] == 0);
        src[maxCopySize] = 1;

        // wcscpy
        wsrc[maxCopySize] = 0;
        wdst[maxCopySize] = 1;
        REQUIRE_NOTHROW(PAL::wcscpy(wdst, dstSize, wsrc, srcSize, true));
        SPXTEST_REQUIRE(PAL::wcsnicmp(wsrc, wdst, dstSize) == 0);
        SPXTEST_REQUIRE(wdst[maxCopySize] == 0);
        wsrc[maxCopySize] = 1;
    }

    SPXTEST_SECTION("truncate: false")
    {
        // source too long

        // strcpy & wcscpy fail
        REQUIRE_THROWS_WITH(PAL::strcpy(dst, dstSize, src, srcSize, false), Catch::Contains("Destination buffer"));
        REQUIRE_THROWS_WITH(PAL::wcscpy(wdst, dstSize, wsrc, srcSize, false), Catch::Contains("Destination buffer"));

        // source not null-terminated

        // strcpy
        dst[maxCopySize] = 1;
        REQUIRE_NOTHROW(PAL::strcpy(dst, dstSize, src, maxCopySize, false));
        SPXTEST_REQUIRE(PAL::strnicmp(src, dst, maxCopySize) == 0);
        SPXTEST_REQUIRE(dst[maxCopySize] == 0);

        // wcscpy
        wdst[maxCopySize] = 1;
        REQUIRE_NOTHROW(PAL::wcscpy(wdst, dstSize, wsrc, maxCopySize, false));
        SPXTEST_REQUIRE(PAL::wcsnicmp(wsrc, wdst, maxCopySize) == 0);
        SPXTEST_REQUIRE(wdst[maxCopySize] == 0);

        // source null-terminated at destination length

        // strcpy
        src[maxCopySize] = 0;
        dst[maxCopySize] = 1;
        REQUIRE_NOTHROW(PAL::strcpy(dst, dstSize, src, srcSize, false));
        SPXTEST_REQUIRE(PAL::strnicmp(src, dst, dstSize) == 0);
        SPXTEST_REQUIRE(dst[maxCopySize] == 0);

        // wcscpy
        wsrc[maxCopySize] = 0;
        wdst[maxCopySize] = 1;
        REQUIRE_NOTHROW(PAL::wcscpy(wdst, dstSize, wsrc, srcSize, false));
        SPXTEST_REQUIRE(PAL::wcsnicmp(wsrc, wdst, dstSize) == 0);
        SPXTEST_REQUIRE(wdst[maxCopySize] == 0);
    }
}SPXTEST_CASE_END()
