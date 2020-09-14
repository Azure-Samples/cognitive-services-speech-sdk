//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include <exception.h>
#define __SPX_THROW_HR_IMPL(hr) Microsoft::CognitiveServices::Speech::Impl::ThrowWithCallstack(hr)

#include "audio_processor_write_to_audio_source_buffer.h"

#include <map>
#include <memory>
#include <set>

#include "unit_test_utils.h"
#include "test_utils.h"
#include "mocks.h"

#include <ispxinterfaces.h>
#include <create_object_helpers.h>
#include <property_id_2_name_map.h>

TEST_CASE("CSpxSession2 basics", "[cxx][session2]")
{
    auto makeBaseTestEnv = []()
    {
        auto site = std::make_shared<MockSite>();

        auto factory = std::make_shared<ObjectFactoryProxy>();
        site->AddService<Carbon::ISpxObjectFactory>(factory);

        auto session = Carbon::SpxCreateObjectWithSite<Carbon::ISpxSession2>("CSpxSession2", site);

        return MakeEnvironment(site, session);
    };

    SPXTEST_GIVEN("A newly initialized CSpxSession2 object w/mock site")
    {
        auto env = makeBaseTestEnv();

        auto session = env.Get<Carbon::ISpxSession2>();
        SPXTEST_REQUIRE(session != nullptr);

        SPXTEST_WHEN("Checking interfaces supported")
        {
            auto init = Carbon::SpxQueryInterface<Carbon::ISpxObjectInit>(session);
            SPXTEST_REQUIRE(init != nullptr);

            auto withsite = Carbon::SpxQueryInterface<Carbon::ISpxObjectWithSite>(session);
            SPXTEST_REQUIRE(withsite != nullptr);

            auto issite = Carbon::SpxQueryInterface<Carbon::ISpxGenericSite>(session);
            SPXTEST_REQUIRE(issite != nullptr);

            auto provider = Carbon::SpxQueryInterface<Carbon::ISpxServiceProvider>(session);
            SPXTEST_REQUIRE(provider != nullptr);
        }

        SPXTEST_WHEN("Checking GetSessionId()")
        {
            const char *id = nullptr;
            REQUIRE_NOTHROW(id = session->GetSessionId());

            auto dash = strstr(id, "-");
            SPXTEST_REQUIRE(dash == nullptr);

            auto cch = strlen(id);
            SPXTEST_REQUIRE(cch > 0);
        }
    }
}
