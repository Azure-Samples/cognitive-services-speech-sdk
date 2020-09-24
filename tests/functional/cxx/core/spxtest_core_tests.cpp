//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include "test_utils.h"

#if 0
SPXTEST_CASE_BEGIN("SPXTEST core", "[core][spxtest]")
{
    SPXTEST_SECTION("Testing new SPXTEST_ MACROS")
    {
        SPXTEST_WHEN("a = true; b = false; SPXTEST_CHECK(a == b)")
        {
            bool a = true;
            bool b = false;
            SPXTEST_CHECK(a == b);
        }

        SPXTEST_WHEN("a = true; b = true; SPXTEST_CHECK_FALSE(a == b)")
        {
            bool a = true;
            bool b = true;
            SPXTEST_CHECK_FALSE(a == b);
        }

        SPXTEST_WHEN("a = true; b = false; SPXTEST_REQUIRE(a == b)")
        {
            bool a = true;
            bool b = false;
            SPXTEST_REQUIRE(a == b);
        }

        SPXTEST_WHEN("a = true; b = true; SPXTEST_REQUIRE_FALSE(a == b)")
        {
            bool a = true;
            bool b = true;
            SPXTEST_REQUIRE_FALSE(a == b);
        }

        SPXTEST_WHEN("Using SPXTEST_CHECK_THAT")
        {
            auto a = "test";
            auto b = "not test";
            SPXTEST_CHECK_THAT(a, Catch::Equals(b));
            SPXTEST_CHECK(a != b);
        }

        SPXTEST_WHEN("Using SPXTEST_REQUIRE_THAT")
        {
            auto a = "test";
            auto b = "not test";
            SPXTEST_REQUIRE_THAT(a, Catch::Equals(b));
            SPXTEST_CHECK(a != b);
        }
    }
}SPXTEST_CASE_END()
#endif

