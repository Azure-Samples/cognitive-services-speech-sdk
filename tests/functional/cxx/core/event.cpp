//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <string>
#include <vector>
#include <event.h>
#include <unordered_map>
#include "test_utils.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Impl;

static size_t publicCount = 0;
static vector<string> publicCallbacks;

static void reset()
{
    publicCount = 0;
    publicCallbacks.clear();
}

static void HandleVoidCallback()
{
    publicCount++;
}

static void HandleStringCallback(const string& a)
{
    publicCount++;
    publicCallbacks.push_back(a);
}

static void HandleMixedCallback(const string& a, int b)
{
    publicCount++;
    publicCallbacks.push_back(a + ", " + to_string(b));
}

class callbacks : public enable_shared_from_this<callbacks>
{
public:
    void HandleCallback()
    {
        publicCount++;
        m_count++;
    }

    void HandleCallback(const string& s)
    {
        publicCount++;
        m_count++;
        m_stringCallbacks.push_back(s);
    }

    void HandleCallback(const string& a, int b)
    {
        publicCount++;
        m_count++;
        m_stringCallbacks.push_back(a + ", " + to_string(b));
    }

    vector<string> m_stringCallbacks;
    size_t m_count;
};


SPXTEST_CASE_BEGIN("No arg event callbacks", "[core][event][callbacks][no_args]")
{
    reset();

    shared_ptr<callbacks> c = make_shared<callbacks>();

    event<> evt;
    evt += HandleVoidCallback;
    evt.add(c, &callbacks::HandleCallback);

    evt();
    SPXTEST_REQUIRE(2 == publicCount);
    SPXTEST_REQUIRE(1 == c->m_count);

    evt.clear();
    evt();
    SPXTEST_REQUIRE(2 == publicCount);
    SPXTEST_REQUIRE(1 == c->m_count);
}SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN("One arg event callbacks", "[core][event][callbacks][one_arg]")
{
    reset();

    shared_ptr<callbacks> c = make_shared<callbacks>();

    event<const string&> evt;
    evt += HandleStringCallback;
    evt.add(c, &callbacks::HandleCallback);

    string arg("Yes");

    evt(arg);
    SPXTEST_REQUIRE(2 == publicCount);
    SPXTEST_REQUIRE(1 == publicCallbacks.size());
    SPXTEST_REQUIRE(arg == publicCallbacks[0]);
    SPXTEST_REQUIRE(1 == c->m_count);
    SPXTEST_REQUIRE(1 == c->m_stringCallbacks.size());
    SPXTEST_REQUIRE(arg == c->m_stringCallbacks[0]);

    evt.clear();
    evt(arg);
    SPXTEST_REQUIRE(2 == publicCount);
    SPXTEST_REQUIRE(1 == publicCallbacks.size());
    SPXTEST_REQUIRE(arg == publicCallbacks[0]);
    SPXTEST_REQUIRE(1 == c->m_count);
    SPXTEST_REQUIRE(1 == c->m_stringCallbacks.size());
    SPXTEST_REQUIRE(arg == c->m_stringCallbacks[0]);
}SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN("Two arg event callbacks", "[core][event][callbacks][two_arg]")
{
    reset();

    shared_ptr<callbacks> c = make_shared<callbacks>();

    event<const string&, int> evt;
    evt += HandleMixedCallback;
    evt.add(c, &callbacks::HandleCallback);

    string arg0("Yes");
    int arg1 = 12;
    string expected0 = arg0 + ", " + to_string(arg1);

    evt(arg0, arg1);
    SPXTEST_REQUIRE(2 == publicCount);
    SPXTEST_REQUIRE(1 == publicCallbacks.size());
    SPXTEST_REQUIRE(expected0 == publicCallbacks[0]);
    SPXTEST_REQUIRE(1 == c->m_count);
    SPXTEST_REQUIRE(1 == c->m_stringCallbacks.size());
    SPXTEST_REQUIRE(expected0 == c->m_stringCallbacks[0]);

    arg0 = "no";
    arg1 = -24;
    string expected1 = arg0 + ", " + to_string(arg1);

    evt(arg0, arg1);
    SPXTEST_REQUIRE(4 == publicCount);
    SPXTEST_REQUIRE(2 == publicCallbacks.size());
    SPXTEST_REQUIRE(expected0 == publicCallbacks[0]);
    SPXTEST_REQUIRE(expected1 == publicCallbacks[1]);
    SPXTEST_REQUIRE(2 == c->m_count);
    SPXTEST_REQUIRE(2 == c->m_stringCallbacks.size());
    SPXTEST_REQUIRE(expected0 == c->m_stringCallbacks[0]);
    SPXTEST_REQUIRE(expected1 == c->m_stringCallbacks[1]);
}SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN("Remove callbacks", "[core][event][callbacks][remove]")
{
    reset();

    auto instance = make_shared<callbacks>();
    vector<size_t> callbackIds;

    event<const string&> evt;

    // add callbacks twice
    callbackIds.push_back(evt += HandleStringCallback);
    evt += HandleStringCallback;
    callbackIds.push_back(evt.add(instance, &callbacks::HandleCallback));
    evt.add(instance, &callbacks::HandleCallback);

    SPXTEST_REQUIRE(0 == callbackIds[0]);
    SPXTEST_REQUIRE(2 == callbackIds[1]);

    // now attempt to remove duplicates
    for (const auto& id : callbackIds)
    {
        evt.remove(id);
    }

    // raise and validate
    string arg("This is a test");
    evt(arg);

    SPXTEST_REQUIRE(2 == publicCount);
    SPXTEST_REQUIRE(1 == publicCallbacks.size());
    SPXTEST_REQUIRE(arg == publicCallbacks[0]);
    SPXTEST_REQUIRE(1 == instance->m_count);
    SPXTEST_REQUIRE(1 == instance->m_stringCallbacks.size());
    SPXTEST_REQUIRE(arg == instance->m_stringCallbacks[0]);
}SPXTEST_CASE_END()
