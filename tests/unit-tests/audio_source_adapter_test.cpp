//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include <exception.h>
#define __SPX_THROW_HR_IMPL(hr) Microsoft::CognitiveServices::Speech::Impl::ThrowWithCallstack(hr)

#include <microphone_audio_source_adapter.h>
#include <file_audio_source_adapter.h>
#include <stream_audio_source_adapter.h>

#include "unit_test_utils.h"
#include "test_utils.h"
#include "mocks.h"

#include <memory>
#include <chrono>

TEST_CASE("CSpxMicrophoneAudioSourceAdapter tests", "[cxx][audio]")
{
    MockInteractiveMicrophone* rawMicPtr = nullptr;
    auto makeBaseTestEnv = [&]()
    {
        auto notifyMe = std::make_shared<MockAudioSourceNotifyMe>();
        auto properties = std::make_shared<MockNamedProperties>();
        auto bufferData = std::make_shared<MockBufferData>();
        auto proxyFactory = std::make_shared<ObjectFactoryProxy>();
        proxyFactory->RegisterOverride("CSpxInteractiveMicrophone", [&]
        {
            rawMicPtr = new MockInteractiveMicrophone();
            return rawMicPtr;
        });
        auto mockSite = std::make_shared<MockSite>();
        mockSite->AddService<Carbon::ISpxObjectFactory>(proxyFactory);
        mockSite->AddService<Carbon::ISpxNamedProperties>(properties);
        mockSite->AddService("BufferData", bufferData);
        auto adapter = Carbon::SpxCreateObjectWithSite<Carbon::ISpxAudioSourceInit>("CSpxMicrophoneAudioSourceAdapter", mockSite);
        return MakeEnvironment(proxyFactory, mockSite, adapter, notifyMe);
    };

    GIVEN("A newly created adapter")
    {
        auto env = makeBaseTestEnv();
        auto audioSourceInit = env.Get<Carbon::ISpxAudioSourceInit>();
        auto audioSourceControl = Carbon::SpxQueryInterface<Carbon::ISpxAudioSourceControl>(audioSourceInit);
        auto audioSourceBufferData = Carbon::SpxQueryInterface<Carbon::ISpxBufferData>(audioSourceInit);
        auto audioSource = Carbon::SpxQueryInterface<Carbon::ISpxAudioSource>(audioSourceInit);
        auto audioProcessorNotifyMe = Carbon::SpxQueryInterface<Carbon::ISpxAudioProcessorNotifyMe>(audioSourceInit);
        auto site = env.Get<MockSite>();

        WHEN("[ISpxAudioSourceInit] Calling InitFromMicrophone")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceInit->InitFromMicrophone();
            });
            THEN("Should not throw an exception")
            {
                REQUIRE_FALSE(exceptionThrown);
            }
            THEN("Should create an interactive microphone object")
            {
                REQUIRE(rawMicPtr != nullptr);
            }
        }
        WHEN("[ISpxAudioSourceInit] Calling InitFromFile")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceInit->InitFromFile(L"some_file");
            });
            THEN("Should throw an exception")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxAudioSourceInit] Calling InitFromStream")
        {
            auto ptr = Carbon::SpxCreateObjectWithSite<Carbon::ISpxAudioStreamInitFormat>("CSpxPullAudioInputStream", site);
            auto stream = ptr->QueryInterface<Carbon::ISpxAudioStream>();
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                    audioSourceInit->InitFromStream(stream);
            });
            THEN("Should throw an exception")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxAudioSourceControl] Calling StartAudio")
        {
            auto notifyMe = env.Get<MockAudioSourceNotifyMe>();
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceControl->StartAudio(notifyMe);
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxAudioSourceControl] Calling StopAudio")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceControl->StopAudio();
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxBufferData] Calling GetOffset")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceBufferData->GetOffset();
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxBufferData] Calling Read")
        {
            std::array<uint8_t, 10> buffer{};
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceBufferData->Read(buffer.data(), static_cast<uint32_t>(buffer.size()));
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxBufferData] Calling ReadAt")
        {
            std::array<uint8_t, 10> buffer{};
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceBufferData->ReadAt(0, buffer.data(), static_cast<uint32_t>(buffer.size()));
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesDead")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceBufferData->GetBytesDead();
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesRead")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceBufferData->GetBytesRead();
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesReady")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceBufferData->GetBytesReady();
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesReadyMax")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSourceBufferData->GetBytesReadyMax();
            });
            THEN("An exception should be thrown")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxAudioSource] Calling GetState")
        {
            auto state = audioSource->GetState();
            THEN("State should be idle")
            {
                REQUIRE(Carbon::ISpxAudioSource::State::Idle == state);
            }
        }
        WHEN("[ISpxAudioSource] Calling GetFormat")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioSource->GetFormat();
            });
            THEN("Should throw an exception")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxAudioProcessorNotifyMe] NotifyMe with processor != nullptr gets called")
        {
            auto processor = std::make_shared<MockAudioProcessor>();
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioProcessorNotifyMe->NotifyMe(processor);
            });
            THEN("No exception should be thrown")
            {
                REQUIRE_FALSE(exceptionThrown);
            }
        }
        WHEN("[ISpxAudioProcessorNotifyMe] NotifyMe with processor == nullptr gets called")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioProcessorNotifyMe->NotifyMe(nullptr);
            });
            THEN("No exception should be thrown")
            {
                REQUIRE_FALSE(exceptionThrown);
            }
        }
    }

    GIVEN("An initialized adapter")
    {
        auto env = makeBaseTestEnv();
        auto audioSourceInit = env.Get<Carbon::ISpxAudioSourceInit>();
        auto audioSourceControl = Carbon::SpxQueryInterface<Carbon::ISpxAudioSourceControl>(audioSourceInit);
        auto audioSourceBufferData = Carbon::SpxQueryInterface<Carbon::ISpxBufferData>(audioSourceInit);
        auto audioSource = Carbon::SpxQueryInterface<Carbon::ISpxAudioSource>(audioSourceInit);
        auto audioProcessorNotifyMe = Carbon::SpxQueryInterface<Carbon::ISpxAudioProcessorNotifyMe>(audioSourceInit);
        audioSourceInit->InitFromMicrophone();
        auto microphoneKeepAlive = GetSharedPointer<Carbon::ISpxAudioPump>(rawMicPtr);

        WHEN("[ISpxAudioSourceControl] Calling StartAudio")
        {
            auto proxyFactory = env.Get<ObjectFactoryProxy>();
            auto processorCreated{ false };
            auto bufferDataCreated{ false };
            proxyFactory->RegisterOverride("CSpxAudioProcessorWriteToAudioSourceBuffer", [&]
            {
                processorCreated = true;
                return new MockAudioProcessor();
            });
            proxyFactory->RegisterOverride("CSpxBufferData", [&]
            {
                bufferDataCreated = true;
                return new MockBufferData();
            });
            auto startPumpCalled{ false };
            auto startPumpCalledWithProcessor{ false };
            rawMicPtr->StartPumpHandler = [&](std::shared_ptr<Carbon::ISpxAudioProcessor> processor)
            {
                startPumpCalled = true;
                startPumpCalledWithProcessor = processor != nullptr;
            };
            auto notifyMe = env.Get<MockAudioSourceNotifyMe>();
            auto state = Carbon::ISpxAudioSource::State::Idle;
            uint64_t bytesReady{ 0xFF };
            notifyMe->NotifyMeHandler = [&](const std::shared_ptr<Carbon::ISpxAudioSource>& source, const std::shared_ptr<Carbon::ISpxBufferData>& data)
            {
                state = source->GetState();
                bytesReady = data->GetBytesReady();
            };
            audioSourceControl->StartAudio(notifyMe);
            THEN("Should emit notification with state == Started and bytesReady == 0")
            {
                REQUIRE(state == Carbon::ISpxAudioSource::State::Started);
                REQUIRE(bytesReady == 0);
            }
            THEN("Should now be in the Started state")
            {
                REQUIRE(audioSource->GetState() == Carbon::ISpxAudioSource::State::Started);
            }
            THEN("Should create a CSpxAudioProcessorWriteToAudioSourceBuffer object")
            {
                REQUIRE(processorCreated);
            }
            THEN("Should call StartPump on the pump")
            {
                REQUIRE(startPumpCalled);
                REQUIRE(startPumpCalledWithProcessor);
            }
            THEN("Should create a CSpxBufferData object.")
            {
                REQUIRE(bufferDataCreated);
            }
        }
        WHEN("[ISpxAudioSourceControl] Calling StopAudio")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
                {
                    audioSourceControl->StopAudio();
                });
            THEN("Should throw an exception")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxBufferData] Calling GetOffset")
        {
            auto proxyFactory = env.Get<ObjectFactoryProxy>();
            auto bufferDataCreated{ false };
            auto getOffsetCallDelegated{ false };
            proxyFactory->RegisterOverride("CSpxBufferData", [&]
                {
                    bufferDataCreated = true;
                    auto rawBufferData = new MockBufferData();
                    rawBufferData->GetOffsetHandler = [&]()
                    {
                        getOffsetCallDelegated = true;
                        return 0;
                    };
                    return rawBufferData;
                });
            std::array<uint8_t, 10> buffer{};
            auto read = audioSourceBufferData->GetOffset();
            THEN("Creates a BufferData object (lazy init)")
            {
                REQUIRE(bufferDataCreated);
            }
            THEN("Delegates the Read to the underlying buffer")
            {
                REQUIRE(getOffsetCallDelegated);
                REQUIRE(read == 0);
            }
        }
        WHEN("[ISpxBufferData] Calling Read")
        {
            auto proxyFactory = env.Get<ObjectFactoryProxy>();
            auto bufferDataCreated{ false };
            auto readCallDelegated{ false };
            proxyFactory->RegisterOverride("CSpxBufferData", [&]
            {
                bufferDataCreated = true;
                auto rawBufferData = new MockBufferData();
                rawBufferData->ReadHandler = [&](uint8_t*, uint32_t)
                {
                    readCallDelegated = true;
                    return 0;
                };
                return rawBufferData;
            });
            std::array<uint8_t, 10> buffer{};
            auto read = audioSourceBufferData->Read(buffer.data(), static_cast<uint32_t>(buffer.size()));
            THEN("Creates a BufferData object (lazy init)")
            {
                REQUIRE(bufferDataCreated);
            }
            THEN("Delegates the Read to the underlying buffer")
            {
                REQUIRE(readCallDelegated);
                REQUIRE(read == 0);
            }
        }
        WHEN("[ISpxBufferData] Calling ReadAt")
        {
            auto proxyFactory = env.Get<ObjectFactoryProxy>();
            auto bufferDataCreated{ false };
            auto readAtCallDelegated{ false };
            proxyFactory->RegisterOverride("CSpxBufferData", [&]
            {
                bufferDataCreated = true;
                auto rawBufferData = new MockBufferData();
                rawBufferData->ReadAtHandler = [&](uint64_t, uint8_t*, uint32_t)
                {
                    readAtCallDelegated = true;
                    return 0;
                };
                return rawBufferData;
            });
            std::array<uint8_t, 10> buffer{};
            auto read = audioSourceBufferData->ReadAt(0, buffer.data(), static_cast<uint32_t>(buffer.size()));
            THEN("Creates a BufferData object (lazy init)")
            {
                REQUIRE(bufferDataCreated);
            }
            THEN("Delegates the ReadAt to the underlying buffer")
            {
                REQUIRE(readAtCallDelegated);
                REQUIRE(read == 0);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesDead")
        {
            auto proxyFactory = env.Get<ObjectFactoryProxy>();
            auto bufferDataCreated{ false };
            auto getBytesDeadCallDelegated{ false };
            proxyFactory->RegisterOverride("CSpxBufferData", [&]
            {
                bufferDataCreated = true;
                auto rawBufferData = new MockBufferData();
                rawBufferData->GetBytesDeadHandler = [&]()
                {
                    getBytesDeadCallDelegated = true;
                    return 0;
                };
                return rawBufferData;
            });
            auto read = audioSourceBufferData->GetBytesDead();
            THEN("Creates a BufferData object (lazy init)")
            {
                REQUIRE(bufferDataCreated);
            }
            THEN("Delegates the GetBytesDead to the underlying buffer")
            {
                REQUIRE(getBytesDeadCallDelegated);
                REQUIRE(read == 0);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesRead")
        {
            auto proxyFactory = env.Get<ObjectFactoryProxy>();
            auto bufferDataCreated{ false };
            auto getBytesReadCallDelegated{ false };
            proxyFactory->RegisterOverride("CSpxBufferData", [&]
            {
                bufferDataCreated = true;
                auto rawBufferData = new MockBufferData();
                rawBufferData->GetBytesReadHandler = [&]()
                {
                    getBytesReadCallDelegated = true;
                    return 0;
                };
                return rawBufferData;
            });
            auto read = audioSourceBufferData->GetBytesRead();
            THEN("Creates a BufferData object (lazy init)")
            {
                REQUIRE(bufferDataCreated);
            }
            THEN("Delegates the GetBytesRead to the underlying buffer")
            {
                REQUIRE(getBytesReadCallDelegated);
                REQUIRE(read == 0);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesReady")
        {
            auto proxyFactory = env.Get<ObjectFactoryProxy>();
            auto bufferDataCreated{ false };
            auto getBytesReadyCallDelegated{ false };
            proxyFactory->RegisterOverride("CSpxBufferData", [&]
            {
                bufferDataCreated = true;
                auto rawBufferData = new MockBufferData();
                rawBufferData->GetBytesReadyHandler = [&]()
                {
                    getBytesReadyCallDelegated = true;
                    return 0;
                };
                return rawBufferData;
            });
            auto read = audioSourceBufferData->GetBytesReady();
            THEN("Creates a BufferData object (lazy init)")
            {
                REQUIRE(bufferDataCreated);
            }
            THEN("Delegates the GetBytesReady to the underlying buffer")
            {
                REQUIRE(getBytesReadyCallDelegated);
                REQUIRE(read == 0);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesReadyMax")
        {
            auto proxyFactory = env.Get<ObjectFactoryProxy>();
            auto bufferDataCreated{ false };
            auto getBytesReadyMaxCallDelegated{ false };
            proxyFactory->RegisterOverride("CSpxBufferData", [&]
                {
                    bufferDataCreated = true;
                    auto rawBufferData = new MockBufferData();
                    rawBufferData->GetBytesReadyMaxHandler = [&]()
                    {
                        getBytesReadyMaxCallDelegated = true;
                        return 0;
                    };
                    return rawBufferData;
                });
            auto read = audioSourceBufferData->GetBytesReadyMax();
            THEN("Creates a BufferData object (lazy init)")
            {
                REQUIRE(bufferDataCreated);
            }
            THEN("Delegates the Read to the underlying buffer")
            {
                REQUIRE(getBytesReadyMaxCallDelegated);
                REQUIRE(read == 0);
            }
        }
        WHEN("[ISpxAudioSource] Calling GetState")
        {
            auto state = audioSource->GetState();
            THEN("State should be Idle")
            {
                REQUIRE(state == Carbon::ISpxAudioSource::State::Idle);
            }
        }
        WHEN("[ISpxAudioSource] Calling GetFormat")
        {
            auto getFormatCalled{ false };
            rawMicPtr->GetFormatHandler = [&](Carbon::SPXWAVEFORMATEX* format, uint16_t size)
            {
                getFormatCalled = true;
                return rawMicPtr->DefaultGetFormat(format, size);
            };
            auto format = audioSource->GetFormat();
            THEN("GetFormat should be called in the underlying pump")
            {
                REQUIRE(getFormatCalled);
            }
        }
        WHEN("[ISpxAudioProcessorNotifyMe] NotifyMe with processor != nullptr gets called")
        {
            auto processor = std::make_shared<MockAudioProcessor>();
            audioProcessorNotifyMe->NotifyMe(processor);
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioProcessorNotifyMe->NotifyMe(processor);
            });
            THEN("No exception should be thrown")
            {
                REQUIRE_FALSE(exceptionThrown);
            }
        }
        WHEN("[ISpxAudioProcessorNotifyMe] NotifyMe with processor == nullptr gets called")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
            {
                audioProcessorNotifyMe->NotifyMe(nullptr);
            });
            THEN("No exception should be thrown")
            {
                REQUIRE_FALSE(exceptionThrown);
            }
        }
    }

    GIVEN("An started adapter")
    {
        MockAudioProcessor* rawAudioProcessor = nullptr;
        MockBufferData* rawAudioSourceBufferData = nullptr;
        auto env = makeBaseTestEnv();
        auto notifyMe = env.Get<MockAudioSourceNotifyMe>();
        auto proxyFactory = env.Get<ObjectFactoryProxy>();
        proxyFactory->RegisterOverride("CSpxAudioProcessorWriteToAudioSourceBuffer", [&]
        {
            rawAudioProcessor = new MockAudioProcessor();
            return rawAudioProcessor;
        });
        proxyFactory->RegisterOverride("CSpxBufferData", [&]
        {
            rawAudioSourceBufferData = new MockBufferData();
            return rawAudioSourceBufferData;
        });
        auto audioSourceInit = env.Get<Carbon::ISpxAudioSourceInit>();
        auto audioSourceControl = Carbon::SpxQueryInterface<Carbon::ISpxAudioSourceControl>(audioSourceInit);
        auto audioSourceBufferData = Carbon::SpxQueryInterface<Carbon::ISpxBufferData>(audioSourceInit);
        auto audioSource = Carbon::SpxQueryInterface<Carbon::ISpxAudioSource>(audioSourceInit);
        auto audioProcessorNotifyMe = Carbon::SpxQueryInterface<Carbon::ISpxAudioProcessorNotifyMe>(audioSourceInit);
        audioSourceInit->InitFromMicrophone();
        auto microphoneKeepAlive = GetSharedPointer<Carbon::ISpxAudioPump>(rawMicPtr);
        audioSourceControl->StartAudio(notifyMe);
        auto processorKeepAlive = GetSharedPointer<Carbon::ISpxAudioProcessor>(rawAudioProcessor);
        auto bufferDataKeepAlive = GetSharedPointer<Carbon::ISpxBufferData>(rawAudioSourceBufferData);

        WHEN("[ISpxAudioSourceControl] Calling StartAudio")
        {
            auto exceptionThrown = CheckThrow<std::runtime_error>([&]
                {
                    audioSourceControl->StartAudio(notifyMe);
                });
            THEN("Should throw an exception")
            {
                REQUIRE(exceptionThrown);
            }
        }
        WHEN("[ISpxAudioSourceControl] Calling StopAudio")
        {
            auto stopPumpCalled{ false };
            rawMicPtr->StopPumpHandler = [&]()
            {
                stopPumpCalled = true;
            };
            auto notifyCalled{ false };
            auto notifyProperState{ false };
            notifyMe->NotifyMeHandler = [&](const std::shared_ptr<Carbon::ISpxAudioSource>& source, const std::shared_ptr<Carbon::ISpxBufferData>&)
            {
                notifyCalled = true;
                notifyProperState = source->GetState() == Carbon::ISpxAudioSource::State::Idle;
            };
            audioSourceControl->StopAudio();
            THEN("Notify should be called")
            {
                REQUIRE(notifyCalled);
                REQUIRE(notifyProperState);
            }
            THEN("StopPump should be called on the pump")
            {
                REQUIRE(stopPumpCalled);
            }
            THEN("Source should be in the Idle state")
            {
                REQUIRE(audioSource->GetState() == Carbon::ISpxAudioSource::State::Idle);
            }
        }
        WHEN("[ISpxBufferData] Calling GetOffset")
        {
            constexpr uint64_t val{ 0xABCD01234 };
            auto callDelegated{ false };
            rawAudioSourceBufferData->GetOffsetHandler = [&]()
            {
                callDelegated = true;
                return val;
            };
            auto offset = audioSourceBufferData->GetOffset();
            THEN("Call should be delegated to the underlying buffer")
            {
                REQUIRE(callDelegated);
                REQUIRE(offset == val);
            }
        }
        WHEN("[ISpxBufferData] Calling Read")
        {
            constexpr std::array<uint8_t, 5> val{ 0xAB, 0xDE, 0xF0, 0xBA, 0xCF };
            auto callDelegated{ false };
            rawAudioSourceBufferData->ReadHandler = [&](uint8_t* buffer, uint32_t size)
            {
                callDelegated = true;
                memcpy(buffer, val.data(), std::min(val.size(), static_cast<size_t>(size)));
                return static_cast<uint32_t>(val.size());
            };
            std::array<uint8_t, 5> buffer{};
            auto readBytes = audioSourceBufferData->Read(buffer.data(), static_cast<uint32_t>(buffer.size()));
            THEN("Call should be delegated to the underlying buffer")
            {
                REQUIRE(callDelegated);
                REQUIRE(readBytes == val.size());
                REQUIRE(std::equal(buffer.begin(), buffer.end(), val.begin()));
            }
        }
        WHEN("[ISpxBufferData] Calling ReadAt")
        {
            constexpr std::array<uint8_t, 5> val{ 0xAB, 0xDE, 0xF0, 0xBA, 0xCF };
            constexpr uint64_t off{ 0xABDEF0 };
            auto callDelegated{ false };
            auto properOffset{ false };
            rawAudioSourceBufferData->ReadAtHandler = [&](uint64_t offset, uint8_t* buffer, uint32_t size)
            {
                callDelegated = true;
                properOffset = offset == off;
                memcpy(buffer, val.data(), std::min(val.size(), static_cast<size_t>(size)));
                return static_cast<uint32_t>(val.size());
            };
            std::array<uint8_t, 5> buffer{};
            auto readBytes = audioSourceBufferData->ReadAt(off, buffer.data(), static_cast<uint32_t>(buffer.size()));
            THEN("Call should be delegated to the underlying buffer")
            {
                REQUIRE(callDelegated);
                REQUIRE(properOffset);
                REQUIRE(readBytes == val.size());
                REQUIRE(std::equal(buffer.begin(), buffer.end(), val.begin()));
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesDead")
        {
            constexpr uint64_t val{ 0xABDEF0 };
            auto callDelegated{ false };
            rawAudioSourceBufferData->GetBytesDeadHandler = [&]()
            {
                callDelegated = true;
                return val;
            };
            auto result = audioSourceBufferData->GetBytesDead();
            THEN("Call should be delegated to the underlying buffer")
            {
                REQUIRE(callDelegated);
                REQUIRE(result == val);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesRead")
        {
            constexpr uint64_t val{ 0xABDEF1 };
            auto callDelegated{ false };
            rawAudioSourceBufferData->GetBytesReadHandler = [&]()
            {
                callDelegated = true;
                return val;
            };
            auto result = audioSourceBufferData->GetBytesRead();
            THEN("Call should be delegated to the underlying buffer")
            {
                REQUIRE(callDelegated);
                REQUIRE(result == val);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesReady")
        {
            constexpr uint64_t val{ 0xABDEF2 };
            auto callDelegated{ false };
            rawAudioSourceBufferData->GetBytesReadyHandler = [&]()
            {
                callDelegated = true;
                return val;
            };
            auto result = audioSourceBufferData->GetBytesReady();
            THEN("Call should be delegated to the underlying buffer")
            {
                REQUIRE(callDelegated);
                REQUIRE(result == val);
            }
        }
        WHEN("[ISpxBufferData] Calling GetBytesReadyMax")
        {
            constexpr uint64_t val{ 0xABDEF3 };
            auto callDelegated{ false };
            rawAudioSourceBufferData->GetBytesReadyMaxHandler = [&]()
            {
                callDelegated = true;
                return val;
            };
            auto result = audioSourceBufferData->GetBytesReadyMax();
            THEN("Call should be delegated to the underlying buffer")
            {
                REQUIRE(callDelegated);
                REQUIRE(result == val);
            }
        }
        WHEN("[ISpxAudioSource] Calling GetState")
        {
            auto state = audioSource->GetState();
            THEN("Should be in the Started state")
            {
                REQUIRE(state == Carbon::ISpxAudioSource::State::Started);
            }
        }
        WHEN("[ISpxAudioSource] Calling GetFormat")
        {
            auto getFormatCalled{ false };
            rawMicPtr->GetFormatHandler = [&](Carbon::SPXWAVEFORMATEX* format, uint16_t size)
            {
                getFormatCalled = true;
                return rawMicPtr->DefaultGetFormat(format, size);
            };
            audioSource->GetFormat();
            THEN("Should delegate the call to the pump")
            {
                REQUIRE(getFormatCalled);
            }
        }
        WHEN("[ISpxAudioProcessorNotifyMe] NotifyMe with processor != nullptr gets called")
        {
            constexpr uint64_t val{ 42 };
            auto getBytesCalled{ false };
            rawAudioSourceBufferData->GetBytesReadyHandler = [&]
            {
                getBytesCalled = true;
                return val;
            };
            auto notifyMeCalled{ false };
            auto state{ Carbon::ISpxAudioSource::State::Idle };
            uint64_t byteCountRetrieved{ 0 };
            notifyMe->NotifyMeHandler = [&](const std::shared_ptr<Carbon::ISpxAudioSource>& source, const std::shared_ptr<Carbon::ISpxBufferData>& data)
            {
                notifyMeCalled = true;
                state = source->GetState();
                byteCountRetrieved = data->GetBytesReady();
            };
            audioProcessorNotifyMe->NotifyMe(processorKeepAlive);
            THEN("Should be in the DataAvailable state")
            {
                REQUIRE(audioSource->GetState() == Carbon::ISpxAudioSource::State::DataAvailable);
            }
            THEN("Should emit a NotifyMe event")
            {
                REQUIRE(notifyMeCalled);
                REQUIRE(getBytesCalled);
                REQUIRE(state == audioSource->GetState());
                REQUIRE(byteCountRetrieved == val);
            }
        }
        WHEN("[ISpxAudioProcessorNotifyMe] NotifyMe with processor == nullptr gets called")
        {
            auto notifyMeCalled{ false };
            auto state{ Carbon::ISpxAudioSource::State::DataAvailable };
            notifyMe->NotifyMeHandler = [&](const std::shared_ptr<Carbon::ISpxAudioSource>& source, const std::shared_ptr<Carbon::ISpxBufferData>&)
            {
                notifyMeCalled = true;
                state = source->GetState();
            };
            audioProcessorNotifyMe->NotifyMe(nullptr);
            THEN("Should be in the Idle state")
            {
                REQUIRE(audioSource->GetState() == Carbon::ISpxAudioSource::State::Idle);
            }
            THEN("Should emit a NotifyMe event")
            {
                REQUIRE(notifyMeCalled);
                REQUIRE(state == audioSource->GetState());
            }
        }
    }
}
