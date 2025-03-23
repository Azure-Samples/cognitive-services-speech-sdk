//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#ifndef __AJV_TESTS_H_DEFINED
#define __AJV_TESTS_H_DEFINED

#ifdef AJV_INCLUDE_TESTS

#include <ajv.h>
#include <catch.hpp>

TEST_CASE("ajv::JsonBuilder... size", "[ajv][basics][builder]") {

    auto json = "{ \"test\": 0 }";
    auto builder = ajv::json::Build(json);
    REQUIRE(builder.IsOk());

    auto iterations = 1000;

    SECTION("Testing pure 'linear'...")
    {
        auto writer = builder.Writer();
        for (int i = 0; i < iterations; i++)
        {
            writer[i].Parse(json);

            auto now = builder.AsJson();
            CAPTURE(i,now);
            auto ok = ajv::json::Parse(now).IsOk();
            REQUIRE(ok);
        }

        CAPTURE(builder.AsJson());
    }
}

TEST_CASE("ajv::JsonView... basics", "[ajv][basics][view]") {

    ajv::JsonView view;

    SECTION("parsing")
    {
        WHEN("empty, soft fails (0)")
        {
            for (auto check: { "", " ", "  ", "\r\n\t " })
            {
                auto root = view.Parse(check, strlen(check));
                auto kind = view.GetKind(root);
                auto count = view.Count(root);
                auto iname = view.Name(root);
                auto inext = view.Next(root);
                auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                REQUIRE(root == 0);
                REQUIRE(kind == 0);
                REQUIRE(count == 0);
                REQUIRE(iname == 0);
                REQUIRE(inext == 0);
                REQUIRE(ifind == -1);

                size_t jsize = SIZE_MAX, ssize = SIZE_MAX;
                const char* jptr = __FUNCTION__, *sptr = __FUNCTION__;
                REQUIRE(view.GetJson(root, &jptr, &jsize) == false);
                REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                REQUIRE(jptr == nullptr); REQUIRE(jsize == 0);
                REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);

                REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == 0);
                REQUIRE(view.GetJson(root, nullptr, nullptr) == false);
                REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                REQUIRE(view.GetBool(root, nullptr) == false);
                REQUIRE(view.GetInt64(root, nullptr) == false);
                REQUIRE(view.GetUint64(root, nullptr) == false);
                REQUIRE(view.GetDouble(root, nullptr) == false);
            }
        }

        WHEN("non-empty, fails (-1)")
        {
            WHEN("bad token")
            {
                for (auto check: { "a", ".", "," })
                {
                    auto root = view.Parse(check, strlen(check));
                    auto kind = view.GetKind(root);
                    auto count = view.Count(root);
                    auto iname = view.Name(root);
                    auto inext = view.Next(root);
                    auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                    REQUIRE(root == -1);
                    REQUIRE(kind == -1);
                    REQUIRE(count == 0);
                    REQUIRE(iname == -1);
                    REQUIRE(inext == -1);
                    REQUIRE(ifind == -1);

                    size_t jsize = SIZE_MAX, ssize = SIZE_MAX;
                    const char* jptr = __FUNCTION__, *sptr = __FUNCTION__;
                    REQUIRE(view.GetJson(root, &jptr, &jsize) == false);
                    REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                    REQUIRE(jptr == nullptr); REQUIRE(jsize == 0);
                    REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);

                    REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == -1);
                    REQUIRE(view.GetJson(root, nullptr, nullptr) == false);
                    REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                    REQUIRE(view.GetBool(root, nullptr) == false);
                    REQUIRE(view.GetInt64(root, nullptr) == false);
                    REQUIRE(view.GetUint64(root, nullptr) == false);
                    REQUIRE(view.GetDouble(root, nullptr) == false);
                }
            }

            WHEN("unterminated")
            {
                WHEN("string")
                {
                    for(auto check: { "\"", "\" ", "\"a", "\"a" })
                    {
                        auto root = view.Parse(check, strlen(check));
                        auto kind = view.GetKind(root);
                        auto count = view.Count(root);
                        auto iname = view.Name(root);
                        auto inext = view.Next(root);
                        auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                        REQUIRE(root == -1);
                        REQUIRE(kind == -1);
                        REQUIRE(count == 0);
                        REQUIRE(iname == -1);
                        REQUIRE(inext == -1);
                        REQUIRE(ifind == -1);

                        size_t jsize = SIZE_MAX, ssize = SIZE_MAX;
                        const char* jptr = __FUNCTION__, *sptr = __FUNCTION__;
                        REQUIRE(view.GetJson(root, &jptr, &jsize) == false);
                        REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                        REQUIRE(jptr == nullptr); REQUIRE(jsize == 0);
                        REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);

                        REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == -1);
                        REQUIRE(view.GetJson(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetBool(root, nullptr) == false);
                        REQUIRE(view.GetInt64(root, nullptr) == false);
                        REQUIRE(view.GetUint64(root, nullptr) == false);
                        REQUIRE(view.GetDouble(root, nullptr) == false);
                    }
                }

                WHEN("bool")
                {
                    for(auto check: { "t", "tru", "f", "fals" })
                    {
                        auto root = view.Parse(check, strlen(check));
                        auto kind = view.GetKind(root);
                        auto count = view.Count(root);
                        auto iname = view.Name(root);
                        auto inext = view.Next(root);
                        auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                        REQUIRE(root == -1);
                        REQUIRE(kind == -1);
                        REQUIRE(count == 0);
                        REQUIRE(iname == -1);
                        REQUIRE(inext == -1);
                        REQUIRE(ifind == -1);

                        size_t jsize = SIZE_MAX, ssize = SIZE_MAX;
                        const char* jptr = __FUNCTION__, *sptr = __FUNCTION__;
                        REQUIRE(view.GetJson(root, &jptr, &jsize) == false);
                        REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                        REQUIRE(jptr == nullptr); REQUIRE(jsize == 0);
                        REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);

                        REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == -1);
                        REQUIRE(view.GetJson(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetBool(root, nullptr) == false);
                        REQUIRE(view.GetInt64(root, nullptr) == false);
                        REQUIRE(view.GetUint64(root, nullptr) == false);
                        REQUIRE(view.GetDouble(root, nullptr) == false);
                    }
                }

                WHEN("number")
                {
                    for(auto check: { "27x", "+27", "2x7", "x27", "027", "2.7ex" })
                    {
                        auto root = view.Parse(check, strlen(check));
                        auto kind = view.GetKind(root);
                        auto count = view.Count(root);
                        auto iname = view.Name(root);
                        auto inext = view.Next(root);
                        auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                        REQUIRE(root == -1);
                        REQUIRE(kind == -1);
                        REQUIRE(count == 0);
                        REQUIRE(iname == -1);
                        REQUIRE(inext == -1);
                        REQUIRE(ifind == -1);

                        size_t jsize = SIZE_MAX, ssize = SIZE_MAX;
                        const char* jptr = __FUNCTION__, *sptr = __FUNCTION__;
                        REQUIRE(view.GetJson(root, &jptr, &jsize) == false);
                        REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                        REQUIRE(jptr == nullptr); REQUIRE(jsize == 0);
                        REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);

                        REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == -1);
                        REQUIRE(view.GetJson(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetBool(root, nullptr) == false);
                        REQUIRE(view.GetInt64(root, nullptr) == false);
                        REQUIRE(view.GetUint64(root, nullptr) == false);
                        REQUIRE(view.GetDouble(root, nullptr) == false);
                    }
                }

                WHEN("null")
                {
                    for(auto check: { "n", "nul" })
                    {
                        auto root = view.Parse(check, strlen(check));
                        auto kind = view.GetKind(root);
                        auto count = view.Count(root);
                        auto iname = view.Name(root);
                        auto inext = view.Next(root);
                        auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                        REQUIRE(root == -1);
                        REQUIRE(kind == -1);
                        REQUIRE(count == 0);
                        REQUIRE(iname == -1);
                        REQUIRE(inext == -1);
                        REQUIRE(ifind == -1);

                        size_t jsize = SIZE_MAX, ssize = SIZE_MAX;
                        const char* jptr = __FUNCTION__, *sptr = __FUNCTION__;
                        REQUIRE(view.GetJson(root, &jptr, &jsize) == false);
                        REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                        REQUIRE(jptr == nullptr); REQUIRE(jsize == 0);
                        REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);

                        REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == -1);
                        REQUIRE(view.GetJson(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetBool(root, nullptr) == false);
                        REQUIRE(view.GetInt64(root, nullptr) == false);
                        REQUIRE(view.GetUint64(root, nullptr) == false);
                        REQUIRE(view.GetDouble(root, nullptr) == false);
                    }
                }

                WHEN("object")
                {
                    for(auto check: { "{", "{ \"a\"", "{ true", "{ 0", "{ null", "{ {}", "{ []" })
                    {
                        auto root = view.Parse(check, strlen(check));
                        auto kind = view.GetKind(root);
                        auto count = view.Count(root);
                        auto iname = view.Name(root);
                        auto inext = view.Next(root);
                        auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                        REQUIRE(root == -1);
                        REQUIRE(kind == -1);
                        REQUIRE(count == 0);
                        REQUIRE(iname == -1);
                        REQUIRE(inext == -1);
                        REQUIRE(ifind == -1);
                                               
                        size_t jsize = SIZE_MAX, ssize = SIZE_MAX;
                        const char* jptr = __FUNCTION__, *sptr = __FUNCTION__;
                        REQUIRE(view.GetJson(root, &jptr, &jsize) == false);
                        REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                        REQUIRE(jptr == nullptr); REQUIRE(jsize == 0);
                        REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);

                        REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == -1);
                        REQUIRE(view.GetJson(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetBool(root, nullptr) == false);
                        REQUIRE(view.GetInt64(root, nullptr) == false);
                        REQUIRE(view.GetUint64(root, nullptr) == false);
                        REQUIRE(view.GetDouble(root, nullptr) == false);
                    }
                }

                WHEN("array")
                {
                    for(auto check: { "[", "[ \"a\"", "[ true", "[ 0", "[ null", "[ {}", "[ []", "[ \"a\",", "[ true,", "[ 0,", "[ null,", "[ {},", "[ []," })
                    {
                        auto root = view.Parse(check, strlen(check));
                        auto kind = view.GetKind(root);
                        auto count = view.Count(root);
                        auto iname = view.Name(root);
                        auto inext = view.Next(root);
                        auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                        REQUIRE(root == -1);
                        REQUIRE(kind == -1);
                        REQUIRE(count == 0);
                        REQUIRE(iname == -1);
                        REQUIRE(inext == -1);
                        REQUIRE(ifind == -1);
                        REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == -1);

                        size_t jsize = SIZE_MAX, ssize = SIZE_MAX;
                        const char* jptr = __FUNCTION__, *sptr = __FUNCTION__;
                        REQUIRE(view.GetJson(root, &jptr, &jsize) == false);
                        REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                        REQUIRE(jptr == nullptr); REQUIRE(jsize == 0);
                        REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);

                        REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == -1);
                        REQUIRE(view.GetJson(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                        REQUIRE(view.GetBool(root, nullptr) == false);
                        REQUIRE(view.GetInt64(root, nullptr) == false);
                        REQUIRE(view.GetUint64(root, nullptr) == false);
                        REQUIRE(view.GetDouble(root, nullptr) == false);
                    }
                }
            }
        }

        WHEN("non-empty, succeeds (>0)")
        {
            WHEN("string")
            {
                for(auto check: { "\"\"", "\" \"", "\"  \"", "\"a\"", "\"ab\"", "\"abc\"", "\"abcd\"", "\"abcde\"", "\"abcdef\"" })
                {
                    CAPTURE(check, strlen(check));
                    auto root = view.Parse(check, strlen(check));
                    auto kind = view.GetKind(root);
                    auto count = view.Count(root);
                    auto iname = view.Name(root);
                    auto inext = view.Next(root);
                    auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                    REQUIRE(root > 0);
                    REQUIRE(kind == '\"');
                    REQUIRE(kind == (int)ajv::JsonKind::String);
                    REQUIRE(count == 0);
                    REQUIRE(iname == 0);
                    REQUIRE(inext == 0);
                    REQUIRE(ifind == -1);

                    size_t jsize = 0, ssize = 0;
                    const char* jptr = nullptr, *sptr = nullptr;
                    REQUIRE(view.GetJson(root, &jptr, &jsize) == true);
                    REQUIRE(view.GetString(root, &sptr, &ssize) == true);
                    REQUIRE(jptr != nullptr); REQUIRE(jsize == strlen(check));
                    REQUIRE(sptr != nullptr); REQUIRE(ssize == strlen(check) - 2);
                    REQUIRE(jptr == check); REQUIRE(sptr > jptr); REQUIRE(sptr < jptr + strlen(check));

                    REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == '\"');
                    REQUIRE(view.GetJson(root, nullptr, nullptr) == true);
                    REQUIRE(view.GetString(root, nullptr, nullptr) == true);
                    REQUIRE(view.GetBool(root, nullptr) == false);
                    REQUIRE(view.GetInt64(root, nullptr) == false);
                    REQUIRE(view.GetUint64(root, nullptr) == false);
                    REQUIRE(view.GetDouble(root, nullptr) == false);
                }
            }

            WHEN("bool")
            {
                for(auto check: { "true", "false" })
                {
                    bool shouldBe = strchr(check, 't') != nullptr;
                    CAPTURE(check, strlen(check), shouldBe);
                    auto root = view.Parse(check, strlen(check));
                    auto kind = view.GetKind(root);
                    auto count = view.Count(root);
                    auto iname = view.Name(root);
                    auto inext = view.Next(root);
                    auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                    REQUIRE(root > 0);
                    REQUIRE(kind == 'b');
                    REQUIRE(kind == (int)ajv::JsonKind::Boolean);
                    REQUIRE(count == 0);
                    REQUIRE(iname == 0);
                    REQUIRE(inext == 0);
                    REQUIRE(ifind == -1);

                    size_t jsize = 0, ssize = SIZE_MAX;
                    const char* jptr = nullptr, *sptr = nullptr;
                    REQUIRE(view.GetJson(root, &jptr, &jsize) == true);
                    REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                    REQUIRE(jptr != nullptr); REQUIRE(jsize == strlen(check));
                    REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);
                    REQUIRE(jptr == check);

                    bool value = !shouldBe;
                    REQUIRE(view.GetBool(root, &value) == true);
                    REQUIRE(value == shouldBe);

                    REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == 'b');
                    REQUIRE(view.GetJson(root, nullptr, nullptr) == true);
                    REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                    REQUIRE(view.GetInt64(root, nullptr) == false);
                    REQUIRE(view.GetUint64(root, nullptr) == false);
                    REQUIRE(view.GetDouble(root, nullptr) == false);
                }
            }

            WHEN("number")
            {
                for (auto check : { 
                    "27", "27.0", "2.7e1", "0.27e2", "270e-1", "2.7E1", "0.27E2", "270E-1",
                    "-27", "-27.0", "-2.7e1", "-0.27e2", "-270e-1", "-2.7E1", "-0.27E2", "-270E-1",
                    "27.3", "2.73e1", "0.273e2", "273e-1", "2.73E1", "0.273E2", "273E-1"
                    })
                {
                    double shouldBe = strchr(check, '3') != nullptr ? 27.3
                        : check[0] == '-' ? -27
                        : 27;
                    CAPTURE(check, strlen(check), shouldBe);
                    auto root = view.Parse(check, strlen(check));
                    auto kind = view.GetKind(root);
                    auto count = view.Count(root);
                    auto iname = view.Name(root);
                    auto inext = view.Next(root);
                    auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                    REQUIRE(root > 0);
                    REQUIRE(kind == '1');
                    REQUIRE(kind == (int)ajv::JsonKind::Number);
                    REQUIRE(count == 0);
                    REQUIRE(iname == 0);
                    REQUIRE(inext == 0);
                    REQUIRE(ifind == -1);

                    size_t jsize = 0, ssize = SIZE_MAX;
                    const char* jptr = nullptr, *sptr = nullptr;
                    REQUIRE(view.GetJson(root, &jptr, &jsize) == true);
                    REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                    REQUIRE(jptr != nullptr); REQUIRE(jsize == strlen(check));
                    REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);
                    REQUIRE(jptr == check);

                    double d64 = 0;
                    int64_t i64 = 0;
                    uint64_t u64 = 0;
                    REQUIRE(view.GetInt64(root, &i64) == true);
                    REQUIRE(view.GetUint64(root, &u64) == true);
                    REQUIRE(view.GetDouble(root, &d64) == true);
                    
                    if (shouldBe == 27.3)
                    {
                        REQUIRE(d64 == 27.3);
                        REQUIRE(i64 == 27);
                        REQUIRE(u64 == 27);
                    }
                    else if (shouldBe == -27)
                    {
                        REQUIRE(d64 == -27);
                        REQUIRE(i64 == -27);
                        REQUIRE(u64 == 0);
                       
                    }
                    else if (shouldBe == 27)
                    {
                        REQUIRE(d64 == 27);
                        REQUIRE(i64 == 27);
                        REQUIRE(u64 == 27);
                    }

                    REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == '1');
                    REQUIRE(view.GetJson(root, nullptr, nullptr) == true);
                    REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                    REQUIRE(view.GetBool(root, nullptr) == false);
                    REQUIRE(view.GetInt64(root, nullptr) == true);
                    REQUIRE(view.GetUint64(root, nullptr) == true);
                    REQUIRE(view.GetDouble(root, nullptr) == true);
                }
            }

            WHEN("null")
            {
                for(auto check: { "null" })
                {
                    CAPTURE(check, strlen(check));
                    auto root = view.Parse(check, strlen(check));
                    auto kind = view.GetKind(root);
                    auto count = view.Count(root);
                    auto iname = view.Name(root);
                    auto inext = view.Next(root);
                    auto ifind = view.Find(root, 0, nullptr, nullptr, nullptr);
                    REQUIRE(root > 0);
                    REQUIRE(kind == 'n');
                    REQUIRE(kind == (int)ajv::JsonKind::Null);
                    REQUIRE(count == 0);
                    REQUIRE(iname == 0);
                    REQUIRE(inext == 0);
                    REQUIRE(ifind == -1);

                    size_t jsize = 0, ssize = SIZE_MAX;
                    const char* jptr = nullptr, *sptr = nullptr;
                    REQUIRE(view.GetJson(root, &jptr, &jsize) == true);
                    REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                    REQUIRE(jptr != nullptr); REQUIRE(jsize == strlen(check));
                    REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);
                    REQUIRE(jptr == check);

                    REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == 'n');
                    REQUIRE(view.GetJson(root, nullptr, nullptr) == true);
                    REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                    REQUIRE(view.GetBool(root, nullptr) == false);
                    REQUIRE(view.GetInt64(root, nullptr) == false);
                    REQUIRE(view.GetUint64(root, nullptr) == false);
                    REQUIRE(view.GetDouble(root, nullptr) == false);
                }
            }

            WHEN("object, 0..3 members")
            {
                for(auto check: {
                    "{}",
                    "{\"member0\":\"a\"}",
                        "{\"member0\":true}",
                        "{\"member0\":7}",
                        "{ \"member0\":null}",
                        "{\"member0\":{}}",
                        "{\"member0\":[]}",
                    "{\"member0\":\"a\",\"member1\":\"b\"}",
                        "{\"member0\":true,\"member1\":false}",
                        "{\"member0\":7,\"member1\":8}",
                        "{\"member0\":null,\"member1\":null}",
                        "{\"member0\":{},\"member1\":{}}",
                        "{\"member0\":[],\"member1\":[]}",
                    "{\"member0\":\"a\",\"member1\":\"b\",\"member2\":3}",
                        "{\"member0\":true,\"member1\":false,\"member2\":3}",
                        "{\"member0\":7,\"member1\":8,\"member2\":3}",
                        "{\"member0\":null,\"member1\":null,\"member2\":3}",
                        "{\"member0\":{},\"member1\":{},\"member2\":3}",
                        "{\"member0\":[],\"member1\":[],\"member2\":3}",
                     })
                {
                    auto expectedChildren = strlen(check) == 2 ? 0
                        : strchr(check, '3') != nullptr ? 3
                        : strchr(check, ',') != nullptr ? 2 
                        : 1;
                    CAPTURE(check, strlen(check), expectedChildren);
                    auto root = view.Parse(check, strlen(check));
                    auto kind = view.GetKind(root);
                    auto count = view.Count(root);
                    auto iname = view.Name(root);
                    auto inext = view.Next(root);
                    REQUIRE(root > 0);
                    REQUIRE(kind == '{');
                    REQUIRE(kind == (int)ajv::JsonKind::Object);
                    REQUIRE(iname == 0);
                    REQUIRE(inext == 0);

                    auto ifindNot = view.Find(root, 0, "not", nullptr, nullptr);            // ~object["not"]
                    auto ifindMember0 = view.Find(root, 0, "member0", nullptr, nullptr);    // ~object["member0"]
                    auto ifindMember1 = view.Find(root, 0, "member1", nullptr, nullptr);    // ~object["member1"]
                    auto ifindMember2 = view.Find(root, 0, "member2", nullptr, nullptr);    // ~object["member2"]
                    auto ifind0 = view.Find(root, 0, nullptr, nullptr, nullptr);            // ~object.Names()[0].value()
                    auto ifind1 = view.Find(root, 1, nullptr, nullptr, nullptr);            // ~object.Names()[1].value()
                    auto ifind2 = view.Find(root, 2, nullptr, nullptr, nullptr);            // ~object.Names()[2].value()
                    auto ifind0Next = view.Next(ifind0);                                    // ~object.Names()[i+1].value(),i=0
                    auto ifind1Next = view.Next(ifind1);                                    // ~object.Names()[i+1].value(),i=1
                    auto ifind2Next = view.Next(ifind2);                                    // ~object.Names()[i+1].value(),i=2

                    size_t nsize = 0;
                    const char* nptr = nullptr;
                    auto inameMember0 = view.Name(ifindMember0);
                    auto inameMember1 = view.Name(ifindMember1);
                    auto inameMember2 = view.Name(ifindMember2);
                    auto okName0 = view.GetString(inameMember0, &nptr, &nsize) == true && strncmp(nptr, "member0", nsize) == 0;
                    auto okName1 = view.GetString(inameMember1, &nptr, &nsize) == true && strncmp(nptr, "member1", nsize) == 0;
                    auto okName2 = view.GetString(inameMember2, &nptr, &nsize) == true && strncmp(nptr, "member2", nsize) == 0;

                    if (expectedChildren == 0)
                    {
                        REQUIRE(count == 0);
                        REQUIRE(ifindNot == 0);
                        REQUIRE(ifindMember0 == 0);
                        REQUIRE(ifindMember1 == 0);
                        REQUIRE(ifindMember2 == 0);
                        REQUIRE(ifind0 == 0);
                        REQUIRE(ifind1 == 0);
                        REQUIRE(ifind2 == 0);
                        REQUIRE(ifind0Next == 0);
                        REQUIRE(ifind1Next == 0);
                        REQUIRE(ifind2Next == 0);
                        REQUIRE(inameMember0 == 0);
                        REQUIRE(inameMember1 == 0);
                        REQUIRE(inameMember2 == 0);
                        REQUIRE(okName0 == false);
                        REQUIRE(okName1 == false);
                        REQUIRE(okName2 == false);
                    }
                    else if (expectedChildren == 1)
                    {
                        REQUIRE(count == 1);
                        REQUIRE(ifindNot == 0);
                        REQUIRE(ifindMember0 > 0);
                        REQUIRE(ifindMember1 == 0);
                        REQUIRE(ifindMember2 == 0);
                        REQUIRE(ifind0 == ifindMember0);
                        REQUIRE(ifind1 == 0);
                        REQUIRE(ifind2 == 0);
                        REQUIRE(ifind0Next == 0);
                        REQUIRE(ifind1Next == 0);
                        REQUIRE(ifind2Next == 0);
                        REQUIRE(inameMember0 > 0);
                        REQUIRE(inameMember1 == 0);
                        REQUIRE(inameMember2 == 0);
                        REQUIRE(okName0 == true);
                        REQUIRE(okName1 == false);
                        REQUIRE(okName2 == false);
                    }
                    else if (expectedChildren == 2)
                    {
                        REQUIRE(count == 2);
                        REQUIRE(ifindNot == 0);
                        REQUIRE(ifindMember0 > 0);
                        REQUIRE(ifindMember1 > 0);
                        REQUIRE(ifindMember2 == 0);
                        REQUIRE(ifind0 == ifindMember0);
                        REQUIRE(ifind1 == ifindMember1);
                        REQUIRE(ifind2 == 0);
                        REQUIRE(ifind0Next == ifindMember1);
                        REQUIRE(ifind1Next == 0);
                        REQUIRE(ifind2Next == 0);
                        REQUIRE(inameMember0 > 0);
                        REQUIRE(inameMember1 > 0);
                        REQUIRE(inameMember2 == 0);
                        REQUIRE(okName0 == true);
                        REQUIRE(okName1 == true);
                        REQUIRE(okName2 == false);
                    }
                    else if (expectedChildren == 3)
                    {
                        REQUIRE(count == 3);
                        REQUIRE(ifindNot == 0);
                        REQUIRE(ifindMember0 > 0);
                        REQUIRE(ifindMember1 > 0);
                        REQUIRE(ifindMember2 > 0);
                        REQUIRE(ifind0 == ifindMember0);
                        REQUIRE(ifind1 == ifindMember1);
                        REQUIRE(ifind2 == ifindMember2);
                        REQUIRE(ifind0Next == ifindMember1);
                        REQUIRE(ifind1Next == ifindMember2);
                        REQUIRE(ifind2Next == 0);
                        REQUIRE(inameMember0 > 0);
                        REQUIRE(inameMember1 > 0);
                        REQUIRE(inameMember2 > 0);
                        REQUIRE(okName0 == true);
                        REQUIRE(okName1 == true);
                        REQUIRE(okName2 == true);
                    }

                    size_t jsize = 0, ssize = SIZE_MAX;
                    const char* jptr = nullptr, *sptr = nullptr;
                    REQUIRE(view.GetJson(root, &jptr, &jsize) == true);
                    REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                    REQUIRE(jptr != nullptr); REQUIRE(jsize == strlen(check));
                    REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);
                    REQUIRE(jptr == check);

                    REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == '{');
                    REQUIRE(view.GetJson(root, nullptr, nullptr) == true);
                    REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                    REQUIRE(view.GetBool(root, nullptr) == false);
                    REQUIRE(view.GetInt64(root, nullptr) == false);
                    REQUIRE(view.GetUint64(root, nullptr) == false);
                    REQUIRE(view.GetDouble(root, nullptr) == false);
                }
            }

            WHEN("array, 0..3 elements")
            {
                for(auto check: {
                    "[]",
                    "[\"a\"]", "[true]", "[7]", "[null]", "[{}]", "[[]]",
                    "[\"a\",\"b\"]", "[true,false]", "[7,8]", "[null, null]", "[{},{}]", "[[],[]]",
                    "[\"a\",\"b\",3]", "[true,false,3]", "[7,8,3]", "[null,null,3]", "[{},{},3]", "[[],[],3]" })
                {
                    auto expectedChildren = strlen(check) == 2 ? 0
                        : strchr(check, '3') != nullptr ? 3
                        : strchr(check, ',') != nullptr ? 2 
                        : 1;
                    CAPTURE(check, strlen(check), expectedChildren);
                    auto root = view.Parse(check, strlen(check));
                    auto kind = view.GetKind(root);
                    auto count = view.Count(root);
                    auto iname = view.Name(root);
                    auto inext = view.Next(root);
                    REQUIRE(root > 0);
                    REQUIRE(kind == '[');
                    REQUIRE(kind == (int)ajv::JsonKind::Array);
                    REQUIRE(iname == 0);
                    REQUIRE(inext == 0);

                    auto ifindNot = view.Find(root, 0, "not", nullptr, nullptr);    // ~array["not"]
                    auto ifind0 = view.Find(root, 0, nullptr, nullptr, nullptr);    // ~array[0]
                    auto ifind1 = view.Find(root, 1, nullptr, nullptr, nullptr);    // ~array[1]
                    auto ifind2 = view.Find(root, 2, nullptr, nullptr, nullptr);    // ~array[2]
                    auto ifind0Next = view.Next(ifind0);                            // ~array[i+1],i=0
                    auto ifind1Next = view.Next(ifind1);                            // ~array[i+1],i=1
                    auto ifind2Next = view.Next(ifind2);                            // ~array[i+1],i=2
                    auto ifind0Name = view.Name(ifind0);
                    if (expectedChildren == 0)
                    {
                        REQUIRE(count == 0);
                        REQUIRE(ifindNot == -1);
                        REQUIRE(ifind0 == 0);
                        REQUIRE(ifind1 == 0);
                        REQUIRE(ifind2 == 0);
                        REQUIRE(ifind0Next == 0);
                        REQUIRE(ifind1Next == 0);
                        REQUIRE(ifind2Next == 0);
                        REQUIRE(ifind0Name == 0);
                    }
                    else if (expectedChildren == 1)
                    {
                        REQUIRE(count == 1);
                        REQUIRE(ifindNot == -1);
                        REQUIRE(ifind0 > 0);
                        REQUIRE(ifind1 == 0);
                        REQUIRE(ifind2 == 0);
                        REQUIRE(ifind0Next == 0);
                        REQUIRE(ifind1Next == 0);
                        REQUIRE(ifind2Next == 0);
                        REQUIRE(ifind0Name == 0);
                    }
                    else if (expectedChildren == 2)
                    {
                        REQUIRE(count == 2);
                        REQUIRE(ifindNot == -1);
                        REQUIRE(ifind0 > 0);
                        REQUIRE(ifind1 > 0);
                        REQUIRE(ifind2 == 0);
                        REQUIRE(ifind0Next > 0);
                        REQUIRE(ifind0Next == ifind1);
                        REQUIRE(ifind1Next == 0);
                        REQUIRE(ifind2Next == 0);
                        REQUIRE(ifind0Name == 0);
                    }
                    else if (expectedChildren == 3)
                    {
                        REQUIRE(count == 3);
                        REQUIRE(ifindNot == -1);
                        REQUIRE(ifind0 > 0);
                        REQUIRE(ifind1 > 0);
                        REQUIRE(ifind2 > 0);
                        REQUIRE(ifind0Next > 0);
                        REQUIRE(ifind0Next == ifind1);
                        REQUIRE(ifind1Next > 0);
                        REQUIRE(ifind1Next == ifind2);
                        REQUIRE(ifind2Next == 0);
                        REQUIRE(ifind0Name == 0);
                    }

                    size_t jsize = 0, ssize = SIZE_MAX;
                    const char* jptr = nullptr, *sptr = nullptr;
                    REQUIRE(view.GetJson(root, &jptr, &jsize) == true);
                    REQUIRE(view.GetString(root, &sptr, &ssize) == false);
                    REQUIRE(jptr != nullptr); REQUIRE(jsize == strlen(check));
                    REQUIRE(sptr == nullptr); REQUIRE(ssize == 0);
                    REQUIRE(jptr == check);

                    REQUIRE(view.GetKind(root, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == '[');
                    REQUIRE(view.GetJson(root, nullptr, nullptr) == true);
                    REQUIRE(view.GetString(root, nullptr, nullptr) == false);
                    REQUIRE(view.GetBool(root, nullptr) == false);
                    REQUIRE(view.GetInt64(root, nullptr) == false);
                    REQUIRE(view.GetUint64(root, nullptr) == false);
                    REQUIRE(view.GetDouble(root, nullptr) == false);
                }
            }
        }
    }

    SECTION("other")
    {
        auto ptr = "  0";
        auto root = view.Parse(ptr, strlen(ptr));
        auto kind = view.GetKind(root);
        REQUIRE(root > 0);
        REQUIRE(kind == '1');

        ptr = "  {\"a\":\"b\", \"c\":\"d\"}";
        root = view.Parse(ptr, strlen(ptr));
        kind = view.GetKind(root);
        REQUIRE(root > 0);
        REQUIRE(kind == '{');

        auto item = view.Find(root, 0, "c", nullptr, nullptr);
        kind = view.GetKind(item);
        REQUIRE(item > 0);
        REQUIRE(kind == '\"');

        size_t size = 0;
        REQUIRE(view.GetString(item, &ptr, &size));
        REQUIRE(size == 1);
        REQUIRE(ptr[0] == 'd');

        auto iname = view.Name(item);
        REQUIRE(iname > 0);
        REQUIRE(view.GetString(iname, &ptr, &size));
        REQUIRE(size == 1);
        REQUIRE(ptr[0] == 'c');

        item = view.Find(root, 0, "a", nullptr, nullptr);
        kind = view.GetKind(item);
        REQUIRE(item > 0);
        REQUIRE(kind == '\"');

        REQUIRE(view.GetString(item, &ptr, &size));
        REQUIRE(size == 1);
        REQUIRE(ptr[0] == 'b');

        iname = view.Name(item);
        REQUIRE(iname > 0);
        REQUIRE(view.GetString(iname, &ptr, &size));
        REQUIRE(size == 1);
        REQUIRE(ptr[0] == 'a');

        size = view.Count(root);
        REQUIRE(size == 2);

        item = view.Next(root);
        REQUIRE(item == 0);
        item = view.Find(root, 0, nullptr, &ptr, &size);
        REQUIRE(item > 0);
        REQUIRE(ptr != nullptr);
        REQUIRE(size == 1);
        REQUIRE(ptr[0] == 'a');

        item = view.Next(item);
        kind = view.GetKind(item, nullptr, nullptr, &ptr, &size, nullptr, nullptr, nullptr);
        REQUIRE(item > 0);
        REQUIRE(kind == '\"');
        REQUIRE(ptr != nullptr);
        REQUIRE(size == 1);
        REQUIRE(ptr[0] == 'd');

        iname = view.Name(item);
        REQUIRE(iname > 0);
        REQUIRE(view.GetString(iname, &ptr, &size));
        REQUIRE(size == 1);
        REQUIRE(ptr[0] == 'c');
    }
}

template <class T = ajv::JsonReader>
void TestCommon1(const T& check, const std::string& json)
{
    REQUIRE(check.IsOk());
    REQUIRE(!check.IsError()); REQUIRE(!check.IsEnd()); REQUIRE(!check.IsEmpty());

    auto kind = check.Kind();
    auto count = check.ValueCount();
    auto vat0 = check.ValueAt(0);
    auto vatM = check.ValueAt("member");
    auto v1st = check.FirstValue();
    auto nat0 = check.NameAt(0);
    auto n1st = check.FirstName();
    auto op0 = check[0];
    auto opM = check["member"];
    auto obj = check.AsObject();
    auto arr = check.AsArray();

    auto isString = json.find("string") != std::string::npos;
    auto isBool = json.find("true") != std::string::npos;
    auto isNum = json.find("1") != std::string::npos;
    auto isNull = json.find("null") != std::string::npos;
    auto isArray = json.find("array") != std::string::npos;
    auto isObject = json.find("object") != std::string::npos;

    auto expectedString = isString ? json.substr(1, json.size() - 2) : "";
    auto expectedBool = isBool ? (json == "true") : false;
    auto expectedNum = isNum ? std::atof(json.c_str()) : 0;
    int64_t expectedInt = isNum ? std::strtoll(json.c_str(), nullptr, 10) : 0;
    uint64_t expectedUint = isNum && json[0] != '-' ? std::strtoull(json.c_str(), nullptr, 10) : 0;

    REQUIRE(check.AsJson() == json);
    REQUIRE(check.AsString() == expectedString);
    REQUIRE(check.AsBool() == expectedBool);
    REQUIRE(check.AsNumber() == expectedNum);
    REQUIRE(check.AsInt() == expectedInt);
    REQUIRE(check.AsUint() == expectedUint);

    if (isString)
    {
        REQUIRE(kind == ajv::JsonKind::String);
        REQUIRE(check.IsString()); REQUIRE(!check.IsBool()); REQUIRE(!check.IsNumber()); REQUIRE(!check.IsNull());
        REQUIRE(!check.IsArray()); REQUIRE(arr.IsError()); REQUIRE(arr.IsEnd()); REQUIRE(arr.IsEmpty()); REQUIRE(!arr.IsOk());
        REQUIRE(!check.IsObject()); REQUIRE(obj.IsError()); REQUIRE(obj.IsEnd()); REQUIRE(obj.IsEmpty()); REQUIRE(!obj.IsOk());
        REQUIRE(!check.IsContainer());
        REQUIRE(count == 0);
        REQUIRE(vat0.IsError()); REQUIRE(vat0.IsEnd()); REQUIRE(vat0.IsEmpty()); REQUIRE(!vat0.IsOk());
        REQUIRE(vatM.IsError()); REQUIRE(vatM.IsEnd()); REQUIRE(vatM.IsEmpty()); REQUIRE(!vatM.IsOk());
        REQUIRE(v1st.IsError()); REQUIRE(v1st.IsEnd()); REQUIRE(v1st.IsEmpty()); REQUIRE(!v1st.IsOk());
        REQUIRE(nat0.IsError()); REQUIRE(nat0.IsEnd()); REQUIRE(nat0.IsEmpty()); REQUIRE(!nat0.IsOk());
        REQUIRE(n1st.IsError()); REQUIRE(n1st.IsEnd()); REQUIRE(n1st.IsEmpty()); REQUIRE(!n1st.IsOk());
        REQUIRE(op0.IsError()); REQUIRE(op0.IsEnd()); REQUIRE(op0.IsEmpty()); REQUIRE(!op0.IsOk());
        REQUIRE(opM.IsError()); REQUIRE(opM.IsEnd()); REQUIRE(opM.IsEmpty()); REQUIRE(!opM.IsOk());
    }
    
    if (isBool)
    {
        REQUIRE(kind == ajv::JsonKind::Boolean);
        REQUIRE(!check.IsString()); REQUIRE(check.IsBool()); REQUIRE(!check.IsNumber()); REQUIRE(!check.IsNull());
        REQUIRE(!check.IsArray()); REQUIRE(arr.IsError()); REQUIRE(arr.IsEnd()); REQUIRE(arr.IsEmpty()); REQUIRE(!arr.IsOk());
        REQUIRE(!check.IsObject()); REQUIRE(obj.IsError()); REQUIRE(obj.IsEnd()); REQUIRE(obj.IsEmpty()); REQUIRE(!obj.IsOk());
        REQUIRE(!check.IsContainer());
        REQUIRE(count == 0);
        REQUIRE(vat0.IsError()); REQUIRE(vat0.IsEnd()); REQUIRE(vat0.IsEmpty()); REQUIRE(!vat0.IsOk());
        REQUIRE(vatM.IsError()); REQUIRE(vatM.IsEnd()); REQUIRE(vatM.IsEmpty()); REQUIRE(!vatM.IsOk());
        REQUIRE(v1st.IsError()); REQUIRE(v1st.IsEnd()); REQUIRE(v1st.IsEmpty()); REQUIRE(!v1st.IsOk());
        REQUIRE(nat0.IsError()); REQUIRE(nat0.IsEnd()); REQUIRE(nat0.IsEmpty()); REQUIRE(!nat0.IsOk());
        REQUIRE(n1st.IsError()); REQUIRE(n1st.IsEnd()); REQUIRE(n1st.IsEmpty()); REQUIRE(!n1st.IsOk());
        REQUIRE(op0.IsError()); REQUIRE(op0.IsEnd()); REQUIRE(op0.IsEmpty()); REQUIRE(!op0.IsOk());
        REQUIRE(opM.IsError()); REQUIRE(opM.IsEnd()); REQUIRE(opM.IsEmpty()); REQUIRE(!opM.IsOk());
    }

    if (isNum)
    {
        REQUIRE(kind == ajv::JsonKind::Number);
        REQUIRE(!check.IsString()); REQUIRE(!check.IsBool()); REQUIRE(check.IsNumber()); REQUIRE(!check.IsNull());
        REQUIRE(!check.IsArray()); REQUIRE(arr.IsError()); REQUIRE(arr.IsEnd()); REQUIRE(arr.IsEmpty()); REQUIRE(!arr.IsOk());
        REQUIRE(!check.IsObject()); REQUIRE(obj.IsError()); REQUIRE(obj.IsEnd()); REQUIRE(obj.IsEmpty()); REQUIRE(!obj.IsOk());
        REQUIRE(!check.IsContainer());
        REQUIRE(count == 0);
        REQUIRE(vat0.IsError()); REQUIRE(vat0.IsEnd()); REQUIRE(vat0.IsEmpty()); REQUIRE(!vat0.IsOk());
        REQUIRE(vatM.IsError()); REQUIRE(vatM.IsEnd()); REQUIRE(vatM.IsEmpty()); REQUIRE(!vatM.IsOk());
        REQUIRE(v1st.IsError()); REQUIRE(v1st.IsEnd()); REQUIRE(v1st.IsEmpty()); REQUIRE(!v1st.IsOk());
        REQUIRE(nat0.IsError()); REQUIRE(nat0.IsEnd()); REQUIRE(nat0.IsEmpty()); REQUIRE(!nat0.IsOk());
        REQUIRE(n1st.IsError()); REQUIRE(n1st.IsEnd()); REQUIRE(n1st.IsEmpty()); REQUIRE(!n1st.IsOk());
        REQUIRE(op0.IsError()); REQUIRE(op0.IsEnd()); REQUIRE(op0.IsEmpty()); REQUIRE(!op0.IsOk());
        REQUIRE(opM.IsError()); REQUIRE(opM.IsEnd()); REQUIRE(opM.IsEmpty()); REQUIRE(!opM.IsOk());
    }

    if (isNull)
    {
        REQUIRE(kind == ajv::JsonKind::Null);
        REQUIRE(!check.IsString()); REQUIRE(!check.IsBool()); REQUIRE(!check.IsNumber()); REQUIRE(check.IsNull());
        REQUIRE(!check.IsArray()); REQUIRE(arr.IsError()); REQUIRE(arr.IsEnd()); REQUIRE(arr.IsEmpty()); REQUIRE(!arr.IsOk());
        REQUIRE(!check.IsObject()); REQUIRE(obj.IsError()); REQUIRE(obj.IsEnd()); REQUIRE(obj.IsEmpty()); REQUIRE(!obj.IsOk());
        REQUIRE(!check.IsContainer());
        REQUIRE(count == 0);
        REQUIRE(vat0.IsError()); REQUIRE(vat0.IsEnd()); REQUIRE(vat0.IsEmpty()); REQUIRE(!vat0.IsOk());
        REQUIRE(vatM.IsError()); REQUIRE(vatM.IsEnd()); REQUIRE(vatM.IsEmpty()); REQUIRE(!vatM.IsOk());
        REQUIRE(v1st.IsError()); REQUIRE(v1st.IsEnd()); REQUIRE(v1st.IsEmpty()); REQUIRE(!v1st.IsOk());
        REQUIRE(nat0.IsError()); REQUIRE(nat0.IsEnd()); REQUIRE(nat0.IsEmpty()); REQUIRE(!nat0.IsOk());
        REQUIRE(n1st.IsError()); REQUIRE(n1st.IsEnd()); REQUIRE(n1st.IsEmpty()); REQUIRE(!n1st.IsOk());
        REQUIRE(op0.IsError()); REQUIRE(op0.IsEnd()); REQUIRE(op0.IsEmpty()); REQUIRE(!op0.IsOk());
        REQUIRE(opM.IsError()); REQUIRE(opM.IsEnd()); REQUIRE(opM.IsEmpty()); REQUIRE(!opM.IsOk());
    }

    if (isArray)
    {
        REQUIRE(kind == ajv::JsonKind::Array);
        REQUIRE(!check.IsString()); REQUIRE(!check.IsBool()); REQUIRE(!check.IsNumber()); REQUIRE(!check.IsNull());
        REQUIRE(check.IsArray()); REQUIRE(!arr.IsError()); REQUIRE(!arr.IsEnd()); REQUIRE(!arr.IsEmpty()); REQUIRE(arr.IsOk());
        REQUIRE(!check.IsObject()); REQUIRE(obj.IsError()); REQUIRE(obj.IsEnd()); REQUIRE(obj.IsEmpty()); REQUIRE(!obj.IsOk());
        REQUIRE(check.IsContainer());
        REQUIRE(count == 1);
        REQUIRE(vatM.IsError()); REQUIRE(vatM.IsEnd()); REQUIRE(vatM.IsEmpty()); REQUIRE(!vatM.IsOk());
        REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());
        REQUIRE(!v1st.IsError()); REQUIRE(!v1st.IsEnd()); REQUIRE(!v1st.IsEmpty()); REQUIRE(v1st.IsOk());
        REQUIRE(!op0.IsError()); REQUIRE(!op0.IsEnd()); REQUIRE(!op0.IsEmpty()); REQUIRE(op0.IsOk());
        REQUIRE(vat0 == v1st); REQUIRE(op0 == vat0);
        REQUIRE(nat0.IsError()); REQUIRE(nat0.IsEnd()); REQUIRE(nat0.IsEmpty()); REQUIRE(!nat0.IsOk());
        REQUIRE(n1st.IsError()); REQUIRE(n1st.IsEnd()); REQUIRE(n1st.IsEmpty()); REQUIRE(!n1st.IsOk());
        REQUIRE(opM.IsError()); REQUIRE(opM.IsEnd()); REQUIRE(opM.IsEmpty()); REQUIRE(!opM.IsOk());
    }

    if (isObject)
    {
        REQUIRE(kind == ajv::JsonKind::Object);
        REQUIRE(!check.IsString()); REQUIRE(!check.IsBool()); REQUIRE(!check.IsNumber()); REQUIRE(!check.IsNull());
        REQUIRE(!check.IsArray()); REQUIRE(arr.IsError()); REQUIRE(arr.IsEnd()); REQUIRE(arr.IsEmpty()); REQUIRE(!arr.IsOk());
        REQUIRE(check.IsObject()); REQUIRE(!obj.IsError()); REQUIRE(!obj.IsEnd()); REQUIRE(!obj.IsEmpty()); REQUIRE(obj.IsOk());
        REQUIRE(check.IsContainer());
        REQUIRE(count == 1);
        REQUIRE(!vatM.IsError()); REQUIRE(!vatM.IsEnd()); REQUIRE(!vatM.IsEmpty()); REQUIRE(vatM.IsOk());
        REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());
        REQUIRE(!v1st.IsError()); REQUIRE(!v1st.IsEnd()); REQUIRE(!v1st.IsEmpty()); REQUIRE(v1st.IsOk());
        REQUIRE(!op0.IsError()); REQUIRE(!op0.IsEnd()); REQUIRE(!op0.IsEmpty()); REQUIRE(op0.IsOk());
        REQUIRE(!opM.IsError()); REQUIRE(!opM.IsEnd()); REQUIRE(!opM.IsEmpty()); REQUIRE(opM.IsOk());
        REQUIRE(vat0 == v1st); REQUIRE(op0 == vat0); REQUIRE(vatM == vat0); REQUIRE(opM == vat0);
        REQUIRE(!nat0.IsError()); REQUIRE(!nat0.IsEnd()); REQUIRE(!nat0.IsEmpty()); REQUIRE(nat0.IsOk());
        REQUIRE(!n1st.IsError()); REQUIRE(!n1st.IsEnd()); REQUIRE(!n1st.IsEmpty()); REQUIRE(n1st.IsOk());
        REQUIRE(nat0 == n1st);
    }
}

template <class T = ajv::JsonBuilder>
void TestWriter1(T& check, const std::string& json)
{
    const auto& ccheck = check;

    auto count = ccheck.ValueCount();
    auto kind = ajv::JsonKind::End;
    auto vat0 = ccheck.End();
    auto vat1 = ccheck.End();
    auto vat2 = ccheck.End();
    auto vatM = ccheck.End();
    auto vatN = ccheck.End();
    auto v1st = ccheck.End();
    auto nat0 = ccheck.NameEnd();
    auto nat1 = ccheck.NameEnd();
    auto nat2 = ccheck.NameEnd();
    auto n1st = ccheck.NameEnd();
    auto op0 = ccheck.End();
    auto op1 = ccheck.End();
    auto op2 = ccheck.End();
    auto opM = ccheck.End();
    auto opN = ccheck.End();

    auto vat0Next = ccheck.End();
    auto vat1Next = ccheck.End();
    auto vat2Next = ccheck.End();
    auto vatMNext = ccheck.End();
    auto vatNNext = ccheck.End();
    auto v1stNext = ccheck.End();
    auto nat0Next = ccheck.NameEnd();
    auto nat1Next = ccheck.NameEnd();
    auto nat2Next = ccheck.NameEnd();
    auto n1stNext = ccheck.NameEnd();
    auto op0Next = ccheck.End();
    auto op1Next = ccheck.End();
    auto op2Next = ccheck.End();
    auto opMNext = ccheck.End();
    auto opNNext = ccheck.End();

    auto obj = ccheck.End();
    auto arr = ccheck.End();

    auto update = [&]() {

        CAPTURE(check.AsJson().c_str());

        count = ccheck.ValueCount();
        kind = ccheck.Kind();
        vat0 = ccheck.ValueAt(0);
        vat1 = ccheck.ValueAt(1);
        vat2 = ccheck.ValueAt(2);
        vatM = ccheck.ValueAt("member");
        vatN = ccheck.ValueAt("new");
        v1st = ccheck.FirstValue();
        nat0 = ccheck.NameAt(0);
        nat1 = ccheck.NameAt(1);
        nat2 = ccheck.NameAt(2);
        n1st = ccheck.FirstName();
        op0 = ccheck[0];
        op1 = ccheck[1];
        opM = ccheck["member"];
        opN = ccheck["new"];

        vat0Next = vat0.Next();
        vat1Next = vat1.Next();
        vat2Next = vat2.Next();
        vatMNext = vatM.Next();
        vatNNext = vatN.Next();
        v1stNext = v1st.Next();
        nat0Next = nat0.Next();
        nat1Next = nat1.Next();
        nat2Next = nat2.Next();
        n1stNext = n1st.Next();
        op0Next = op0.Next();
        op1Next = op1.Next();
        op2Next = op2.Next();
        opMNext = opM.Next();
        opNNext = opN.Next();

        obj = ccheck.AsObject();
        arr = ccheck.AsArray();
    };

    auto arrayOk = [&](int expectedChildren) {

        CAPTURE(check.AsJson().c_str(), kind);

        REQUIRE(kind == ajv::JsonKind::Array);
        REQUIRE(!check.IsString()); REQUIRE(!check.IsBool()); REQUIRE(!check.IsNumber()); REQUIRE(!check.IsNull());
        REQUIRE(check.IsArray()); REQUIRE(!arr.IsError()); REQUIRE(!arr.IsEnd()); REQUIRE(!arr.IsEmpty()); REQUIRE(arr.IsOk());
        REQUIRE(!check.IsObject()); REQUIRE(obj.IsError()); REQUIRE(obj.IsEnd()); REQUIRE(obj.IsEmpty()); REQUIRE(!obj.IsOk());
        REQUIRE(check.IsContainer());

        REQUIRE(vatM.IsError()); REQUIRE(vatM.IsEnd()); REQUIRE(vatM.IsEmpty()); REQUIRE(!vatM.IsOk());
        REQUIRE(vatN.IsError()); REQUIRE(vatN.IsEnd()); REQUIRE(vatN.IsEmpty()); REQUIRE(!vatN.IsOk());
        REQUIRE(vatMNext.IsError()); REQUIRE(vatMNext.IsEnd()); REQUIRE(vatMNext.IsEmpty()); REQUIRE(!vatMNext.IsOk());
        REQUIRE(vatNNext.IsError()); REQUIRE(vatNNext.IsEnd()); REQUIRE(vatNNext.IsEmpty()); REQUIRE(!vatNNext.IsOk());

        REQUIRE(opM.IsError()); REQUIRE(opM.IsEnd()); REQUIRE(opM.IsEmpty()); REQUIRE(!opM.IsOk());
        REQUIRE(opN.IsError()); REQUIRE(opN.IsEnd()); REQUIRE(opN.IsEmpty()); REQUIRE(!opN.IsOk());
        REQUIRE(opMNext.IsError()); REQUIRE(opMNext.IsEnd()); REQUIRE(opMNext.IsEmpty()); REQUIRE(!opMNext.IsOk());
        REQUIRE(opNNext.IsError()); REQUIRE(opNNext.IsEnd()); REQUIRE(opNNext.IsEmpty()); REQUIRE(!opNNext.IsOk());

        REQUIRE(nat0.IsError()); REQUIRE(nat0.IsEnd()); REQUIRE(nat0.IsEmpty()); REQUIRE(!nat0.IsOk());
        REQUIRE(nat1.IsError()); REQUIRE(nat1.IsEnd()); REQUIRE(nat1.IsEmpty()); REQUIRE(!nat1.IsOk());
        REQUIRE(nat2.IsError()); REQUIRE(nat2.IsEnd()); REQUIRE(nat2.IsEmpty()); REQUIRE(!nat2.IsOk());
        REQUIRE(n1st.IsError()); REQUIRE(n1st.IsEnd()); REQUIRE(n1st.IsEmpty()); REQUIRE(!n1st.IsOk());
        REQUIRE(nat0Next.IsError()); REQUIRE(nat0Next.IsEnd()); REQUIRE(nat0Next.IsEmpty()); REQUIRE(!nat0Next.IsOk());
        REQUIRE(nat1Next.IsError()); REQUIRE(nat1Next.IsEnd()); REQUIRE(nat1Next.IsEmpty()); REQUIRE(!nat1Next.IsOk());
        REQUIRE(nat2Next.IsError()); REQUIRE(nat2Next.IsEnd()); REQUIRE(nat2Next.IsEmpty()); REQUIRE(!nat2Next.IsOk());
        REQUIRE(n1stNext.IsError()); REQUIRE(n1stNext.IsEnd()); REQUIRE(n1stNext.IsEmpty()); REQUIRE(!n1stNext.IsOk());

        if (expectedChildren == 1)
        {
            REQUIRE(count == 1);
            REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());
            REQUIRE(!vat1.IsError()); REQUIRE(vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());
            REQUIRE(!vat2.IsError()); REQUIRE(vat2.IsEnd()); REQUIRE(vat2.IsEmpty()); REQUIRE(!vat2.IsOk());
            REQUIRE(!v1st.IsError()); REQUIRE(!v1st.IsEnd()); REQUIRE(!v1st.IsEmpty()); REQUIRE(v1st.IsOk());
            REQUIRE(!op0.IsError()); REQUIRE(!op0.IsEnd()); REQUIRE(!op0.IsEmpty()); REQUIRE(op0.IsOk());
            REQUIRE(!op1.IsError()); REQUIRE(op1.IsEnd()); REQUIRE(op1.IsEmpty()); REQUIRE(!op1.IsOk());
            REQUIRE(!op2.IsError()); REQUIRE(op2.IsEnd()); REQUIRE(op2.IsEmpty()); REQUIRE(!op2.IsOk());
            REQUIRE(vat0 == v1st); REQUIRE(op0 == vat0);

            REQUIRE(!vat0Next.IsError()); REQUIRE(vat0Next.IsEnd()); REQUIRE(vat0Next.IsEmpty()); REQUIRE(!vat0Next.IsOk());
            REQUIRE(!vat1Next.IsError()); REQUIRE(vat1Next.IsEnd()); REQUIRE(vat1Next.IsEmpty()); REQUIRE(!vat1Next.IsOk());
            REQUIRE(!vat2Next.IsError()); REQUIRE(vat2Next.IsEnd()); REQUIRE(vat2Next.IsEmpty()); REQUIRE(!vat2Next.IsOk());
            REQUIRE(!v1stNext.IsError()); REQUIRE(v1stNext.IsEnd()); REQUIRE(v1stNext.IsEmpty()); REQUIRE(!v1stNext.IsOk());
            REQUIRE(!op0Next.IsError()); REQUIRE(op0Next.IsEnd()); REQUIRE(op0Next.IsEmpty()); REQUIRE(!op0Next.IsOk());
            REQUIRE(!op1Next.IsError()); REQUIRE(op1Next.IsEnd()); REQUIRE(op1Next.IsEmpty()); REQUIRE(!op1Next.IsOk());
            REQUIRE(!op2Next.IsError()); REQUIRE(op2Next.IsEnd()); REQUIRE(op2Next.IsEmpty()); REQUIRE(!op2Next.IsOk());
        }
        else if (expectedChildren == 2)
        {
            REQUIRE(count == 2);
            REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());
            REQUIRE(!vat1.IsError()); REQUIRE(!vat1.IsEnd()); REQUIRE(vat1.IsEmpty() == vat1.IsUnspecified()); REQUIRE(vat1.IsOk() != vat1.IsUnspecified());
            REQUIRE(!vat2.IsError()); REQUIRE(vat2.IsEnd()); REQUIRE(vat2.IsEmpty()); REQUIRE(!vat2.IsOk());
            REQUIRE(!v1st.IsError()); REQUIRE(!v1st.IsEnd()); REQUIRE(!v1st.IsEmpty()); REQUIRE(v1st.IsOk());
            REQUIRE(!op0.IsError()); REQUIRE(!op0.IsEnd()); REQUIRE(!op0.IsEmpty()); REQUIRE(op0.IsOk());
            REQUIRE(!op1.IsError()); REQUIRE(!op1.IsEnd()); REQUIRE(op1.IsEmpty() == vat1.IsUnspecified()); REQUIRE(op1.IsOk() != vat1.IsUnspecified());
            REQUIRE(!op2.IsError()); REQUIRE(op2.IsEnd()); REQUIRE(op2.IsEmpty()); REQUIRE(!op2.IsOk());
            REQUIRE(vat0 == v1st); REQUIRE(op0 == vat0);
            REQUIRE(vat1 == v1stNext); REQUIRE(op1 == vat1);

            REQUIRE(!vat0Next.IsError()); REQUIRE(!vat0Next.IsEnd()); REQUIRE(vat0Next.IsEmpty() == vat1.IsUnspecified()); REQUIRE(vat0Next.IsOk() != vat1.IsUnspecified());
            REQUIRE(!vat1Next.IsError()); REQUIRE(vat1Next.IsEnd()); REQUIRE(vat1Next.IsEmpty()); REQUIRE(!vat1Next.IsOk());
            REQUIRE(!vat2Next.IsError()); REQUIRE(vat2Next.IsEnd()); REQUIRE(vat2Next.IsEmpty()); REQUIRE(!vat2Next.IsOk());
            REQUIRE(!v1stNext.IsError()); REQUIRE(!v1stNext.IsEnd()); REQUIRE(v1stNext.IsEmpty() == vat1.IsUnspecified()); REQUIRE(v1stNext.IsOk() != vat1.IsUnspecified());
            REQUIRE(!op0Next.IsError()); REQUIRE(!op0Next.IsEnd()); REQUIRE(op0Next.IsEmpty() == vat1.IsUnspecified()); REQUIRE(op0Next.IsOk() != vat1.IsUnspecified());
            REQUIRE(!op1Next.IsError()); REQUIRE(op1Next.IsEnd()); REQUIRE(op1Next.IsEmpty()); REQUIRE(!op1Next.IsOk());
            REQUIRE(!op2Next.IsError()); REQUIRE(op2Next.IsEnd()); REQUIRE(op2Next.IsEmpty()); REQUIRE(!op2Next.IsOk());
        }
    };

    auto objectOk = [&](int expectedChildren) {

        CAPTURE(check.AsJson().c_str());

        REQUIRE(kind == ajv::JsonKind::Object);
        REQUIRE(!check.IsString()); REQUIRE(!check.IsBool()); REQUIRE(!check.IsNumber()); REQUIRE(!check.IsNull());
        REQUIRE(!check.IsArray()); REQUIRE(arr.IsError()); REQUIRE(arr.IsEnd()); REQUIRE(arr.IsEmpty()); REQUIRE(!arr.IsOk());
        REQUIRE(check.IsObject()); REQUIRE(!obj.IsError()); REQUIRE(!obj.IsEnd()); REQUIRE(!obj.IsEmpty()); REQUIRE(obj.IsOk());
        REQUIRE(check.IsContainer());

        REQUIRE(count == expectedChildren);
        if (expectedChildren == 1)
        {
            REQUIRE(count == 1);
            REQUIRE(!vatM.IsError()); REQUIRE(!vatM.IsEnd()); REQUIRE(!vatM.IsEmpty()); REQUIRE(vatM.IsOk());
            REQUIRE(!vatN.IsError()); REQUIRE(vatN.IsEnd()); REQUIRE(vatN.IsEmpty()); REQUIRE(!vatN.IsOk());
            REQUIRE(!vatMNext.IsError()); REQUIRE(vatMNext.IsEnd()); REQUIRE(vatMNext.IsEmpty()); REQUIRE(!vatMNext.IsOk());
            REQUIRE(!vatNNext.IsError()); REQUIRE(vatNNext.IsEnd()); REQUIRE(vatNNext.IsEmpty()); REQUIRE(!vatNNext.IsOk());

            REQUIRE(!opM.IsError()); REQUIRE(!opM.IsEnd()); REQUIRE(!opM.IsEmpty()); REQUIRE(opM.IsOk());
            REQUIRE(!opN.IsError()); REQUIRE(opN.IsEnd()); REQUIRE(opN.IsEmpty()); REQUIRE(!opN.IsOk());
            REQUIRE(!opMNext.IsError()); REQUIRE(opMNext.IsEnd()); REQUIRE(opMNext.IsEmpty()); REQUIRE(!opMNext.IsOk());
            REQUIRE(!opNNext.IsError()); REQUIRE(opNNext.IsEnd()); REQUIRE(opNNext.IsEmpty()); REQUIRE(!opNNext.IsOk());

            REQUIRE(!nat0.IsError()); REQUIRE(!nat0.IsEnd()); REQUIRE(!nat0.IsEmpty()); REQUIRE(nat0.IsOk());
            REQUIRE(!nat1.IsError()); REQUIRE(nat1.IsEnd()); REQUIRE(nat1.IsEmpty()); REQUIRE(!nat1.IsOk());
            REQUIRE(!nat2.IsError()); REQUIRE(nat2.IsEnd()); REQUIRE(nat2.IsEmpty()); REQUIRE(!nat2.IsOk());
            REQUIRE(!n1st.IsError()); REQUIRE(!n1st.IsEnd()); REQUIRE(!n1st.IsEmpty()); REQUIRE(n1st.IsOk());
            REQUIRE(!nat0Next.IsError()); REQUIRE(nat0Next.IsEnd()); REQUIRE(nat0Next.IsEmpty()); REQUIRE(!nat0Next.IsOk());
            REQUIRE(!nat1Next.IsError()); REQUIRE(nat1Next.IsEnd()); REQUIRE(nat1Next.IsEmpty()); REQUIRE(!nat1Next.IsOk());
            REQUIRE(!nat2Next.IsError()); REQUIRE(nat2Next.IsEnd()); REQUIRE(nat2Next.IsEmpty()); REQUIRE(!nat2Next.IsOk());
            REQUIRE(!n1stNext.IsError()); REQUIRE(n1stNext.IsEnd()); REQUIRE(n1stNext.IsEmpty()); REQUIRE(!n1stNext.IsOk());

            REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());
            REQUIRE(!vat1.IsError()); REQUIRE(vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());
            REQUIRE(!vat2.IsError()); REQUIRE(vat2.IsEnd()); REQUIRE(vat2.IsEmpty()); REQUIRE(!vat2.IsOk());
            REQUIRE(!v1st.IsError()); REQUIRE(!v1st.IsEnd()); REQUIRE(!v1st.IsEmpty()); REQUIRE(v1st.IsOk());
            REQUIRE(!op0.IsError()); REQUIRE(!op0.IsEnd()); REQUIRE(!op0.IsEmpty()); REQUIRE(op0.IsOk());
            REQUIRE(!op1.IsError()); REQUIRE(op1.IsEnd()); REQUIRE(op1.IsEmpty()); REQUIRE(!op1.IsOk());
            REQUIRE(!op2.IsError()); REQUIRE(op2.IsEnd()); REQUIRE(op2.IsEmpty()); REQUIRE(!op2.IsOk());
            REQUIRE(vat0 == v1st); REQUIRE(op0 == vat0);

            REQUIRE(!vat0Next.IsError()); REQUIRE(vat0Next.IsEnd()); REQUIRE(vat0Next.IsEmpty()); REQUIRE(!vat0Next.IsOk());
            REQUIRE(!vat1Next.IsError()); REQUIRE(vat1Next.IsEnd()); REQUIRE(vat1Next.IsEmpty()); REQUIRE(!vat1Next.IsOk());
            REQUIRE(!vat2Next.IsError()); REQUIRE(vat2Next.IsEnd()); REQUIRE(vat2Next.IsEmpty()); REQUIRE(!vat2Next.IsOk());
            REQUIRE(!v1stNext.IsError()); REQUIRE(v1stNext.IsEnd()); REQUIRE(v1stNext.IsEmpty()); REQUIRE(!v1stNext.IsOk());
            REQUIRE(!op0Next.IsError()); REQUIRE(op0Next.IsEnd()); REQUIRE(op0Next.IsEmpty()); REQUIRE(!op0Next.IsOk());
            REQUIRE(!op1Next.IsError()); REQUIRE(op1Next.IsEnd()); REQUIRE(op1Next.IsEmpty()); REQUIRE(!op1Next.IsOk());
            REQUIRE(!op2Next.IsError()); REQUIRE(op2Next.IsEnd()); REQUIRE(op2Next.IsEmpty()); REQUIRE(!op2Next.IsOk());
        }
        else if (expectedChildren == 2)
        {
            REQUIRE(count == 2);
            REQUIRE(!vatM.IsError()); REQUIRE(!vatM.IsEnd()); REQUIRE(!vatM.IsEmpty()); REQUIRE(vatM.IsOk());
            REQUIRE(!vatN.IsError()); REQUIRE(!vatN.IsEnd()); REQUIRE(!vatN.IsEmpty()); REQUIRE(vatN.IsOk());
            REQUIRE(!vatMNext.IsError()); REQUIRE(!vatMNext.IsEnd()); REQUIRE(!vatMNext.IsEmpty()); REQUIRE(vatMNext.IsOk());
            REQUIRE(vatNNext.IsError()); REQUIRE(vatNNext.IsEnd()); REQUIRE(vatNNext.IsEmpty()); REQUIRE(!vatNNext.IsOk());
            REQUIRE(vatMNext == vatN); 

            REQUIRE(!opM.IsError()); REQUIRE(!opM.IsEnd()); REQUIRE(!opM.IsEmpty()); REQUIRE(opM.IsOk());
            REQUIRE(!opN.IsError()); REQUIRE(!opN.IsEnd()); REQUIRE(!opN.IsEmpty()); REQUIRE(opN.IsOk());
            REQUIRE(!opMNext.IsError()); REQUIRE(!opMNext.IsEnd()); REQUIRE(!opMNext.IsEmpty()); REQUIRE(opMNext.IsOk());
            REQUIRE(opNNext.IsError()); REQUIRE(opNNext.IsEnd()); REQUIRE(opNNext.IsEmpty()); REQUIRE(!opNNext.IsOk());
            REQUIRE(opMNext == opN);

            REQUIRE(!nat0.IsError()); REQUIRE(!nat0.IsEnd()); REQUIRE(!nat0.IsEmpty()); REQUIRE(nat0.IsOk());
            REQUIRE(!nat1.IsError()); REQUIRE(!nat1.IsEnd()); REQUIRE(!nat1.IsEmpty()); REQUIRE(nat1.IsOk());
            REQUIRE(nat2.IsError()); REQUIRE(nat2.IsEnd()); REQUIRE(nat2.IsEmpty()); REQUIRE(!nat2.IsOk());
            REQUIRE(!n1st.IsError()); REQUIRE(!n1st.IsEnd()); REQUIRE(!n1st.IsEmpty()); REQUIRE(n1st.IsOk());
            REQUIRE(!nat0Next.IsError()); REQUIRE(!nat0Next.IsEnd()); REQUIRE(!nat0Next.IsEmpty()); REQUIRE(nat0Next.IsOk());
            REQUIRE(nat1Next.IsError()); REQUIRE(nat1Next.IsEnd()); REQUIRE(nat1Next.IsEmpty()); REQUIRE(!nat1Next.IsOk());
            REQUIRE(nat2Next.IsError()); REQUIRE(nat2Next.IsEnd()); REQUIRE(nat2Next.IsEmpty()); REQUIRE(!nat2Next.IsOk());
            REQUIRE(!n1stNext.IsError()); REQUIRE(!n1stNext.IsEnd()); REQUIRE(!n1stNext.IsEmpty()); REQUIRE(n1stNext.IsOk());
            REQUIRE(nat0Next == nat1); REQUIRE(n1stNext == nat1);

            REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());
            REQUIRE(!vat1.IsError()); REQUIRE(!vat1.IsEnd()); REQUIRE(!vat1.IsEmpty()); REQUIRE(vat1.IsOk());
            REQUIRE(vat2.IsError()); REQUIRE(vat2.IsEnd()); REQUIRE(vat2.IsEmpty()); REQUIRE(!vat2.IsOk());
            REQUIRE(!v1st.IsError()); REQUIRE(!v1st.IsEnd()); REQUIRE(!v1st.IsEmpty()); REQUIRE(v1st.IsOk());
            REQUIRE(!op0.IsError()); REQUIRE(!op0.IsEnd()); REQUIRE(!op0.IsEmpty()); REQUIRE(op0.IsOk());
            REQUIRE(!op1.IsError()); REQUIRE(!op1.IsEnd()); REQUIRE(!op1.IsEmpty()); REQUIRE(op1.IsOk());
            REQUIRE(op2.IsError()); REQUIRE(op2.IsEnd()); REQUIRE(op2.IsEmpty()); REQUIRE(!op2.IsOk());
            REQUIRE(vat0 == v1st); REQUIRE(op0 == vat0);
            REQUIRE(vat1 == v1stNext); REQUIRE(op1 == vat1);

            REQUIRE(!vat0Next.IsError()); REQUIRE(!vat0Next.IsEnd()); REQUIRE(!vat0Next.IsEmpty()); REQUIRE(vat0Next.IsOk());
            REQUIRE(vat1Next.IsError()); REQUIRE(vat1Next.IsEnd()); REQUIRE(vat1Next.IsEmpty()); REQUIRE(!vat1Next.IsOk());
            REQUIRE(vat2Next.IsError()); REQUIRE(vat2Next.IsEnd()); REQUIRE(vat2Next.IsEmpty()); REQUIRE(!vat2Next.IsOk());
            REQUIRE(!v1stNext.IsError()); REQUIRE(!v1stNext.IsEnd()); REQUIRE(!v1stNext.IsEmpty()); REQUIRE(v1stNext.IsOk());
            REQUIRE(!op0Next.IsError()); REQUIRE(!op0Next.IsEnd()); REQUIRE(!op0Next.IsEmpty()); REQUIRE(op0Next.IsOk());
            REQUIRE(!op1Next.IsError()); REQUIRE(!op1Next.IsEnd()); REQUIRE(!op1Next.IsEmpty()); REQUIRE(op1Next.IsOk());
            REQUIRE(op2Next.IsError()); REQUIRE(op2Next.IsEnd()); REQUIRE(op2Next.IsEmpty()); REQUIRE(!op2Next.IsOk());
        }
    };

    update();

    if (arr.IsOk())
    {
        SECTION("Testing... const access")
        {
            arrayOk(1);
        }

        SECTION("Testing... non-const access")
        {
            // these non-const methods shouldn't change anything, as called
            vat0 = check.ValueAt(0, false);
            vat1 = check.ValueAt(1, false);
            vatM = check.ValueAt("member", false);
            vatN = check.ValueAt("new", false);
            v1st = check.FirstValue();
            nat0 = check.NameAt(0);
            nat1 = check.NameAt(1);
            n1st = check.FirstName();
            op0 = check[0];

            count = ccheck.ValueCount();

            kind = check.Kind();
            vat0Next = vat0.Next();
            vat1Next = vat1.Next();
            vatMNext = vatM.Next();
            vatNNext = vatN.Next();
            v1stNext = v1st.Next();
            nat0Next = nat0.Next();
            nat1Next = nat1.Next();
            n1stNext = n1st.Next();
            op0Next = op0.Next();
            op1Next = op1.Next();

            // check to make sure they didn't
            arrayOk(1);
            update();
            arrayOk(1);
        }

        SECTION("Testing... ValueAt(0, true)")
        {
            vat0 = check.ValueAt(0, true);
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());

            update();
            arrayOk(1);
        }

        SECTION("Testing... ValueAt(0, true) = Parse(...)")
        {
            vat0 = check.ValueAt(0, true);
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());

            vat0 = ajv::json::Parse(vat0.AsJson());
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());

            update();
            arrayOk(1);
        }

        SECTION("Testing... ValueAt(1, false)")
        {
            vat1 = check.ValueAt(1, false);
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            update();
            arrayOk(1);
        }

        SECTION("Testing... ValueAt(1, false) = Parse(...)")
        {
            vat1 = check.ValueAt(1, false);
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            vat1 = ajv::json::Parse(vat0.AsJson());
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(vat1.IsError()); REQUIRE(vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            update();
            arrayOk(1);
        }

        SECTION("Testing... ValueAt(1, true)")
        {
            vat1 = check.ValueAt(1, true);
            REQUIRE(vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(!vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            update();
            arrayOk(2);
        }

        SECTION("Testing... ValueAt(1, true) = Parse(...)")
        {
            vat1 = check.ValueAt(1, true);
            REQUIRE(vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(!vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            vat1 = ajv::json::Parse(vat0.AsJson());
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(!vat1.IsEnd()); REQUIRE(!vat1.IsEmpty()); REQUIRE(vat1.IsOk());

            update();
            arrayOk(2);
        }

        SECTION("Testing... ValueAt(2, false)")
        {
            vat2 = check.ValueAt(2, false);
            REQUIRE(!vat2.IsUnspecified());
            REQUIRE(!vat2.IsError()); REQUIRE(vat2.IsEnd()); REQUIRE(vat2.IsEmpty()); REQUIRE(!vat2.IsOk());

            update();
            arrayOk(1);
        }

        SECTION("Testing... ValueAt(2, false) = Parse(...)")
        {
            vat2 = check.ValueAt(2, false);
            REQUIRE(!vat2.IsUnspecified());
            REQUIRE(!vat2.IsError()); REQUIRE(vat2.IsEnd()); REQUIRE(vat2.IsEmpty()); REQUIRE(!vat2.IsOk());

            vat2 = ajv::json::Parse(vat0.AsJson());
            REQUIRE(!vat2.IsUnspecified());
            REQUIRE(vat2.IsError()); REQUIRE(vat2.IsEnd()); REQUIRE(vat2.IsEmpty()); REQUIRE(!vat2.IsOk());

            update();
            arrayOk(1);
        }
    }

    if (obj.IsOk())
    {
        SECTION("Testing... const access")
        {
            objectOk(1);
        }

        SECTION("Testing... non-const access")
        {
            // these non-const methods shouldn't change anything, as called
            vat0 = check.ValueAt(0, false);
            vat1 = check.ValueAt(1, false);
            vatM = check.ValueAt("member", false);
            vatN = check.ValueAt("new", false);
            v1st = check.FirstValue();
            nat0 = check.NameAt(0);
            nat1 = check.NameAt(1);
            n1st = check.FirstName();
            op0 = check[0];

            kind = check.Kind();
            vat0Next = vat0.Next();
            vat1Next = vat1.Next();
            vatMNext = vatM.Next();
            vatNNext = vatN.Next();
            v1stNext = v1st.Next();
            nat0Next = nat0.Next();
            nat1Next = nat1.Next();
            n1stNext = n1st.Next();
            op0Next = op0.Next();
            op1Next = op1.Next();
            opMNext = opM.Next();
            opNNext = opN.Next();

            // check to make sure they didn't
            objectOk(1);
            update();
            objectOk(1);
        }

        SECTION("Testing... ValueAt(0, true)")
        {
            vat0 = check.ValueAt(0, true);
            REQUIRE(!vat0.IsUnspecified());
            REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());

            update();
            objectOk(1);
        }

        SECTION("Testing... ValueAt(0, true) = Parse(...)")
        {
            vat0 = check.ValueAt(0, true);
            REQUIRE(!vat0.IsUnspecified());
            REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());
            
            vat0 = ajv::json::Parse(vat0.AsJson());
            REQUIRE(!vat0.IsUnspecified());
            REQUIRE(!vat0.IsError()); REQUIRE(!vat0.IsEnd()); REQUIRE(!vat0.IsEmpty()); REQUIRE(vat0.IsOk());

            update();
            objectOk(1);
        }

        SECTION("Testing... ValueAt(1, false)")
        {
            vat1 = check.ValueAt(1, false);
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            update();
            objectOk(1);
        }

        SECTION("Testing... ValueAt(1, false) = Parse(...)")
        {
            vat1 = check.ValueAt(1, false);
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            vat1 = ajv::json::Parse(vat0.AsJson());
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(vat1.IsError()); REQUIRE(vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            update();
            objectOk(1);
        }

        SECTION("Testing... ValueAt(1, true)")
        {
            vat1 = check.ValueAt(1, true);
            REQUIRE(vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(!vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            update();
            arrayOk(2);
        }

        SECTION("Testing... ValueAt(1, true) = Parse(...)")
        {
            vat1 = check.ValueAt(1, true);
            REQUIRE(vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(!vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            vat1 = ajv::json::Parse(vat0.AsJson());
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(!vat1.IsEnd()); REQUIRE(!vat1.IsEmpty()); REQUIRE(vat1.IsOk());

            update();
            arrayOk(2);
        }

        SECTION("Testing... ValueAt(2, false)")
        {
            vat2 = check.ValueAt(2, false);
            REQUIRE(!vat2.IsUnspecified());
            REQUIRE(!vat2.IsError()); REQUIRE(vat2.IsEnd()); REQUIRE(vat2.IsEmpty()); REQUIRE(!vat2.IsOk());

            update();
            objectOk(1);
        }

        SECTION("Testing... ValueAt(2, false) = Parse(...)")
        {
            vat2 = check.ValueAt(2, false);
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            vat2 = ajv::json::Parse(vat0.AsJson());
            REQUIRE(!vat1.IsUnspecified());
            REQUIRE(!vat1.IsError()); REQUIRE(vat1.IsEnd()); REQUIRE(vat1.IsEmpty()); REQUIRE(!vat1.IsOk());

            update();
            objectOk(1);
        }
    }
}

TEST_CASE("ajv::JsonReader... basics", "[ajv][basics][reader]") {

    for (auto check: { "\"string\"", "true", "1", "null", "[\"array\"]", "{\"member\":\"object\"}" })
    {
        GIVEN("JsonReader w/ check=" << check)
        {
            ajv::JsonView view;
            auto root = view.Parse(check, strlen(check));
            auto reader = ajv::JsonReader(view, root);

            SECTION("Testing... TestCommon1") {
                TestCommon1<ajv::JsonReader>(reader, check);
            }

            SECTION("Testing... Name()") {
                auto name = reader.Name();
                REQUIRE(!name.IsError()); REQUIRE(name.IsEnd()); REQUIRE(name.IsEmpty()); REQUIRE(!name.IsOk());
            }

            SECTION("Testing... Next()") {
                auto next = reader.Next();
                REQUIRE(!next.IsError()); REQUIRE(next.IsEnd()); REQUIRE(next.IsEmpty()); REQUIRE(!next.IsOk());
            }

            SECTION("Testing... operator++()") {
                auto opPP = ++reader;
                REQUIRE(!opPP.IsError()); REQUIRE(opPP.IsEnd()); REQUIRE(opPP.IsEmpty()); REQUIRE(!opPP.IsOk());
            }
        }

        GIVEN("JsonReaderView w/ check=" << check)
        {
            auto parsed = ajv::JsonReaderView::Parse(check, strlen(check));

            SECTION("Testing... TestCommon1") {
                TestCommon1<ajv::JsonReaderView>(parsed, check);
            }

            SECTION("Testing... TestCommon1 w/ JsonReaderView's Reader") {
                TestCommon1<ajv::JsonReader>(parsed.Reader(), check);
            }
        }

        GIVEN("JsonParser w/ check=" << check)
        {
            auto parsed = ajv::JsonParser::Parse(check);

            SECTION("Testing... TestCommon1") {
                TestCommon1<ajv::JsonParser>(parsed, check);
            }

            SECTION("Testing... TestCommon1 w/ JsonParser's Reader") {
                TestCommon1<ajv::JsonReader>(parsed.Reader(), check);
            }
        }

        GIVEN("JsonBuilder w/ check=" << check)
        {
            auto parsed = ajv::JsonBuilder::Build(check);

            SECTION("Testing... TestCommon1") {
                TestCommon1<ajv::JsonBuilder>(parsed, check);
            }

            SECTION("Testing... TestCommon1 w/ JsonBuilder's JsonWriter") {
                TestCommon1<ajv::JsonBuilder::JsonWriter>(parsed.Writer(), check);
            }

            SECTION("Testing... Name() w/ JsonBuilder's JsonWriter") {
                auto name = parsed.Writer().Name();
                REQUIRE(!name.IsError()); REQUIRE(name.IsEnd()); REQUIRE(name.IsEmpty()); REQUIRE(!name.IsOk());
            }

            SECTION("Testing... Next() w/ JsonBuilder's JsonWriter") {
                auto next = parsed.Writer().Next();
                REQUIRE(!next.IsError()); REQUIRE(next.IsEnd()); REQUIRE(next.IsEmpty()); REQUIRE(!next.IsOk());
            }

            SECTION("Testing... operator++() w/ JsonBuilder's JsonWriter") {
                auto opPP = ++parsed.Writer();
                REQUIRE(!opPP.IsError()); REQUIRE(opPP.IsEnd()); REQUIRE(opPP.IsEmpty()); REQUIRE(!opPP.IsOk());
            }
        }
    }
}

TEST_CASE("ajv::JsonBuilder... basics", "[ajv][basics][builder]") {

    for (auto check: { "[\"array\"]", "{\"member\":\"object\"}" })
    {
        GIVEN("JsonBuilder w/ check=" << check)
        {
            auto parsed = ajv::JsonBuilder::Build(check);

            SECTION("Testing... access/creation") {

                TestWriter1<ajv::JsonBuilder>(parsed, check);
            }

            SECTION("Testing... access/creation w/ JsonBuilder's JsonWriter") {

                TestWriter1<ajv::JsonBuilder::JsonWriter>(parsed.Writer(), check);
            }
        }
    }
}

TEST_CASE("ajv::JsonParser... array basics", "[ajv][basics][parser][array]" ) {

    auto json = "[\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"]";

    auto parsed = ajv::JsonParser::Parse(json, strlen(json));
    assert(parsed.IsOk());

    WHEN("using array operator[](int) indexing, AsStringPtr")
    {
        size_t size;
        const char* ptr = parsed[0].AsStringPtr(&size); // AsStringPtr returns pointer into original string
        REQUIRE(ptr != nullptr);
        REQUIRE(size == strlen("Sunday"));
        REQUIRE(strncmp(ptr, "Sunday", size) == 0);

        REQUIRE(strncmp(parsed[6].AsStringPtr(&size), "Saturday", size) == 0);
        REQUIRE(strncmp(parsed[5].AsStringPtr(&size), "Friday", size) == 0);
        REQUIRE(strncmp(parsed[4].AsStringPtr(&size), "Thursday", size) == 0);
        REQUIRE(strncmp(parsed[3].AsStringPtr(&size), "Wednesday", size) == 0);
        REQUIRE(strncmp(parsed[2].AsStringPtr(&size), "Tuesday", size) == 0);
        REQUIRE(strncmp(parsed[1].AsStringPtr(&size), "Monday", size) == 0);
    }

    WHEN("using operator++() iteration on array, AsStringPtr")
    {
        size_t size = 0;
        auto item = parsed[0];
        auto ptr = item.AsStringPtr(&size);
        REQUIRE(ptr != nullptr);
        REQUIRE(size == strlen("Sunday"));
        REQUIRE(strncmp(item.AsStringPtr(&size), "Sunday", size) == 0);

        // use operator++ to iterate thru an array
        REQUIRE(strncmp((++item).AsStringPtr(&size), "Monday", size) == 0);
        REQUIRE(strncmp((++item).AsStringPtr(&size), "Tuesday", size) == 0);
        REQUIRE(strncmp((++item).AsStringPtr(&size), "Wednesday", size) == 0);
        REQUIRE(strncmp((++item).AsStringPtr(&size), "Thursday", size) == 0);
        REQUIRE(strncmp((++item).AsStringPtr(&size), "Friday", size) == 0);
        REQUIRE(strncmp((++item).AsStringPtr(&size), "Saturday", size) == 0);
    }

    WHEN("using operator++(int) iteration on array, AsStringPtr")
    {
        size_t size = 0;
        auto item = parsed[0];
        auto ptr = item.AsStringPtr(&size);
        REQUIRE(ptr != nullptr);
        REQUIRE(size == strlen("Sunday"));
        REQUIRE(strncmp(item.AsStringPtr(&size), "Sunday", size) == 0);

        // use operator++(int) to iterate thru an array
        REQUIRE(strncmp((item++).AsStringPtr(&size), "Sunday", size) == 0);
        REQUIRE(strncmp((item++).AsStringPtr(&size), "Monday", size) == 0);
        REQUIRE(strncmp((item++).AsStringPtr(&size), "Tuesday", size) == 0);
        REQUIRE(strncmp((item++).AsStringPtr(&size), "Wednesday", size) == 0);
        REQUIRE(strncmp((item++).AsStringPtr(&size), "Thursday", size) == 0);
        REQUIRE(strncmp((item++).AsStringPtr(&size), "Friday", size) == 0);
        REQUIRE(strncmp((item++).AsStringPtr(&size), "Saturday", size) == 0);
    }

    WHEN("using array operator[](int) indexing, AsString")
    {
        auto str = parsed[0].AsString(); // AsString returns new std::string's with new allocated memory
        REQUIRE(str.size() == strlen("Sunday"));
        REQUIRE(str == "Sunday");

        REQUIRE(parsed[1].AsString() == "Monday");
        REQUIRE(parsed[2].AsString() == "Tuesday");
        REQUIRE(parsed[3].AsString() == "Wednesday");
        REQUIRE(parsed[4].AsString() == "Thursday");
        REQUIRE(parsed[5].AsString() == "Friday");
        REQUIRE(parsed[6].AsString() == "Saturday");
    }
}

TEST_CASE("ajv::JsonParser... object basics", "[ajv][basics][parser][object]") {

    SECTION("simple object")
    {
        auto json = "{\"name\":\"rob\", \"email\":\"robch@github.com\"}";

        auto parsed = ajv::JsonParser::Parse(json, strlen(json)); // light weight parse, using caller memory
        REQUIRE(parsed.IsOk());

        size_t size;
        const char* ptr = parsed["name"].AsStringPtr(&size); // use operator[] to access objects
        REQUIRE(ptr != nullptr);
        REQUIRE(size == strlen("rob"));

        REQUIRE(strncmp(ptr, "rob", size) == 0);
        REQUIRE(strncmp(parsed["email"].AsStringPtr(&size), "robch@github.com", size) == 0);
    }

    SECTION("object nested")
    {
        auto json = "{"
            "\"buyer\": {\"name\":\"your name\", \"email\":\"you@github.com\"},"
            "\"seller\": {\"name\":\"rob\", \"email\":\"robch@github.com\"}"
            "}";

        auto parsed = ajv::JsonParser::Parse(json, strlen(json));
        REQUIRE(parsed.IsOk());

        auto buyerName = parsed["buyer"]["name"]; // use operator[] repeatedly to access sub objects
        auto sellerName = parsed["seller"]["name"];

        size_t size = 0;
        REQUIRE(strncmp(buyerName.AsStringPtr(&size), "your name", size) == 0);
        REQUIRE(strncmp(sellerName.AsStringPtr(&size), "rob", size) == 0);
    }
}

#endif

#endif // __AJV_TESTS_H_DEFINED
