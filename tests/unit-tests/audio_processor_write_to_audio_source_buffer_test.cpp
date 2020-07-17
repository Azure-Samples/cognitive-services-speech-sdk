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

TEST_CASE("CSpxAudioProcessorWriteToAudioSourceBuffer tests", "[cxx][audio][audio_processor_write_to_audio_source_buffer]")
{
    auto makeBaseTestEnv = []()
    {
        auto notifyMe = std::make_shared<MockAudioProcessorNotifyMe>();
        auto proxyFactory = std::make_shared<ObjectFactoryProxy>();
        auto bufferDataWriter = std::make_shared<MockAudioSourceBufferDataWriter>();
        auto bufferProperties = std::make_shared<MockAudioSourceBufferProperties>();
        auto mockSite = std::make_shared<MockSite>();
        mockSite->AddService<Carbon::ISpxObjectFactory>(proxyFactory);
        mockSite->AddService<Carbon::ISpxAudioProcessorNotifyMe>(notifyMe);
        mockSite->AddService("AudioSourceBufferData", bufferDataWriter);
        mockSite->AddService("AudioSourceBufferProperties", bufferProperties);
        auto processor = Carbon::SpxCreateObjectWithSite<Carbon::ISpxAudioProcessor>("CSpxAudioProcessorWriteToAudioSourceBuffer", mockSite);
        return MakeEnvironment(notifyMe, bufferDataWriter, bufferProperties, mockSite, processor);
    };

    GIVEN("A newly initialized processor")
    {
        auto env = makeBaseTestEnv();
        auto notify = env.Get<MockAudioProcessorNotifyMe>();
        auto writer = env.Get<MockAudioSourceBufferDataWriter>();
        auto properties = env.Get<MockAudioSourceBufferProperties>();
        auto processor = env.Get<Carbon::ISpxAudioProcessor>();
        auto mockFormat = ConstructDefaultFormat();

        WHEN("Receiving set format != nullptr")
        {
            bool notifyStarted{ false };
            bool writerCalled{ false };
            notify->NotifyMeHandler = [&](const std::shared_ptr<Carbon::ISpxAudioProcessor>& processor)
            {
                notifyStarted = processor != nullptr;
            };
            writer->WriteHandler = [&](uint8_t*, uint32_t)
            {
                writerCalled = true;
            };
            bool setBufferPropertyCalled{ false };
            properties->SetBufferPropertyHandler = [&](const char*, const char*)
            {
                setBufferPropertyCalled = true;
            };
            processor->SetFormat(mockFormat.get());
            THEN("Should notify started (processor != nullptr)")
            {
                REQUIRE(notifyStarted);
            }
            THEN("Buffer data writer should not be called")
            {
                REQUIRE_FALSE(writerCalled);
            }
            THEN("SetBufferProperty should not be called")
            {
                REQUIRE_FALSE(setBufferPropertyCalled);
            }
        }

        WHEN("Receiving audio")
        {
            bool receivedEvent{ false };
            bool writerCalled{ false };
            notify->NotifyMeHandler = [&](const std::shared_ptr<Carbon::ISpxAudioProcessor>&)
            {
                receivedEvent = true;
            };
            writer->WriteHandler = [&](uint8_t*, uint32_t)
            {
                writerCalled = true;
            };
            bool setBufferPropertyCalled{ false };
            properties->SetBufferPropertyHandler = [&](const char*, const char*)
            {
                setBufferPropertyCalled = true;
            };
            auto chunk = std::make_shared<Carbon::DataChunk>(Carbon::SpxAllocSharedAudioBuffer(1), 1);
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                processor->ProcessAudio(chunk);
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
            THEN("No event should be raised")
            {
                REQUIRE_FALSE(receivedEvent);
            }
            THEN("Buffer writer should not be called")
            {
                REQUIRE_FALSE(writerCalled);
            }
            THEN("SetBufferProperty should not be called")
            {
                REQUIRE_FALSE(setBufferPropertyCalled);
            }
        }

        WHEN("Receiving set format == nullptr")
        {
            bool receivedEvent{ false };
            bool writerCalled{ false };
            notify->NotifyMeHandler = [&](const std::shared_ptr<Carbon::ISpxAudioProcessor>&)
            {
                receivedEvent = true;
            };
            writer->WriteHandler = [&](uint8_t*, uint32_t)
            {
                writerCalled = true;
            };
            bool setBufferPropertyCalled{ false };
            properties->SetBufferPropertyHandler = [&](const char*, const char*)
            {
                setBufferPropertyCalled = true;
            };
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                processor->SetFormat(nullptr);
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
            THEN("Notify should not be called")
            {
                REQUIRE_FALSE(receivedEvent);
            }
            THEN("Buffer data writer should not be called")
            {
                REQUIRE_FALSE(writerCalled);
            }
            THEN("SetBufferProperty should not be called")
            {
                REQUIRE_FALSE(setBufferPropertyCalled);
            }
        }
    }

    GIVEN("A processor that has already started (i.e received SetFormat with format != nullptr)")
    {
        auto env = makeBaseTestEnv();
        auto notify = env.Get<MockAudioProcessorNotifyMe>();
        auto writer = env.Get<MockAudioSourceBufferDataWriter>();
        auto properties = env.Get<MockAudioSourceBufferProperties>();
        auto processor = env.Get<Carbon::ISpxAudioProcessor>();
        auto format = ConstructDefaultFormat();
        processor->SetFormat(format.get());

        WHEN("Received set format != nullptr")
        {
            bool receivedEvent{ false };
            bool writerCalled{ false };
            notify->NotifyMeHandler = [&](const std::shared_ptr<Carbon::ISpxAudioProcessor>&)
            {
                receivedEvent = true;
            };
            writer->WriteHandler = [&](uint8_t*, uint32_t)
            {
                writerCalled = true;
            };
            bool setBufferPropertyCalled{ false };
            properties->SetBufferPropertyHandler = [&](const char*, const char*)
            {
                setBufferPropertyCalled = true;
            };
            bool exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                processor->SetFormat(format.get());
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
            THEN("Notify should not be called")
            {
                REQUIRE_FALSE(receivedEvent);
            }
            THEN("Buffer data writer should not be called")
            {
                REQUIRE_FALSE(writerCalled);
            }
            THEN("SetBufferProperty should not be called")
            {
                REQUIRE_FALSE(setBufferPropertyCalled);
            }
        }

        WHEN("Receiving audio")
        {
            bool receivedEvent{ false };
            bool writerCalled{ false };
            size_t receivedCount{ 0 };
            constexpr uint8_t val{ 0xab };
            uint8_t receivedVal{ 0x00 };
            notify->NotifyMeHandler = [&](const std::shared_ptr<Carbon::ISpxAudioProcessor>&)
            {
                receivedEvent = true;
            };
            writer->WriteHandler = [&](uint8_t* data, uint32_t count)
            {
                writerCalled = true;
                receivedCount = count;
                if (count > 0)
                {
                    receivedVal = data[0];
                }
            };
            int setBufferPropertyCallCount{ 0 };
            bool receivedDataBufferTimestamp{ false };
            bool receivedDataBufferUserId{ false };
            properties->SetBufferPropertyHandler = [&](const char* name, const char* value)
            {
                setBufferPropertyCallCount += 1;
                if ((name == nullptr) || (value == nullptr))
                {
                    return;
                }
                std::string timestampName{ Carbon::GetPropertyName(SDK::PropertyId::DataBuffer_TimeStamp) };
                std::string userIdName{ Carbon::GetPropertyName(SDK::PropertyId::DataBuffer_UserId) };
                if (timestampName == name)
                {
                    receivedDataBufferTimestamp = true;
                }
                else if (userIdName == name)
                {
                    receivedDataBufferUserId = true;
                }
            };
            auto chunk = std::make_shared<Carbon::DataChunk>(Carbon::SpxAllocSharedAudioBuffer(1), 1, "now", "user");
            (chunk->data.get())[0] = val;
            processor->ProcessAudio(chunk);
            THEN("An event should be raised")
            {
                REQUIRE(receivedEvent);
            }
            THEN("Buffer writer should be called")
            {
                REQUIRE(writerCalled);
                REQUIRE(receivedCount == 1);
                REQUIRE(val == receivedVal);
            }
            THEN("SetBufferProperty should be called twice for DataBuffer_TimeStamp and DataBuffer_UserId")
            {
                REQUIRE(setBufferPropertyCallCount == 2);
                REQUIRE(receivedDataBufferTimestamp);
                REQUIRE(receivedDataBufferUserId);
            }
        }

        WHEN("Receiving set format == nullptr")
        {
            bool notifyStopped{ false };
            bool writerCalled{ false };
            bool isBufferEmpty{ false };
            bool isCountZero{ false };
            notify->NotifyMeHandler = [&](const std::shared_ptr<Carbon::ISpxAudioProcessor>& processor)
            {
                notifyStopped = processor == nullptr;
            };
            writer->WriteHandler = [&](uint8_t* buffer, uint32_t count)
            {
                writerCalled = true;
                isBufferEmpty = buffer == nullptr;
                isCountZero = count == 0;
            };
            bool setBufferPropertyCalled{ false };
            properties->SetBufferPropertyHandler = [&](const char*, const char*)
            {
                setBufferPropertyCalled = true;
            };
            processor->SetFormat(nullptr);
            THEN("Should notify stopped (processor == nullptr)")
            {
                REQUIRE(notifyStopped);
            }
            THEN("Buffer data writer should be called with nullptr and 0")
            {
                REQUIRE(writerCalled);
                REQUIRE(isBufferEmpty);
                REQUIRE(isCountZero);
            }
            THEN("SetBufferProperty should not be called")
            {
                REQUIRE_FALSE(setBufferPropertyCalled);
            }
        }
    }

}
