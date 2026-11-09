// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md in the project root.

#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace live_interpreter
{
struct SessionState
{
    std::mutex mutex;
    std::condition_variable changed;
    bool done = false;
    bool failed = false;
    unsigned int audioIndex = 0;
};

template <typename Action>
void OnEvent(const std::shared_ptr<SessionState>& state, Action action) noexcept
{
    std::lock_guard<std::mutex> lock(state->mutex);
    try
    {
        action();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Live Interpreter callback failed: " << error.what() << '\n';
        state->failed = true;
    }
    if (state->done)
        state->changed.notify_all();
}

inline std::string RequiredEnvironment(const char* name)
{
#ifdef _WIN32
    char* value = nullptr;
    size_t size = 0;
    if (_dupenv_s(&value, &size, name) != 0)
        throw std::runtime_error(std::string("Cannot read environment variable: ") + name);
    const std::string result = value == nullptr ? "" : value;
    std::free(value);
#else
    const auto value = std::getenv(name);
    const std::string result = value == nullptr ? "" : value;
#endif
    if (result.empty())
        throw std::runtime_error(std::string("Missing required environment variable: ") + name);
    return result;
}

inline uint32_t LittleEndian(const unsigned char* bytes, size_t count)
{
    uint32_t value = 0;
    for (size_t i = 0; i < count; ++i)
        value |= static_cast<uint32_t>(bytes[i]) << (8 * i);
    return value;
}

// This sample deliberately accepts only 16-kHz, mono, 16-bit PCM RIFF/WAVE.
inline void ValidateWav(std::istream& input)
{
    const auto invalid = [] {
        throw std::runtime_error("Invalid WAV: use a nonempty 16-kHz, mono, 16-bit PCM RIFF/WAVE file.");
    };
    input.seekg(0, std::ios::end);
    const auto end = input.tellg();
    if (end < 12)
        invalid();
    const auto length = static_cast<uint64_t>(end);
    input.seekg(0);
    unsigned char header[12]{};
    if (!input.read(reinterpret_cast<char*>(header), sizeof(header)) ||
        std::memcmp(header, "RIFF", 4) != 0 || std::memcmp(header + 8, "WAVE", 4) != 0 ||
        static_cast<uint64_t>(LittleEndian(header + 4, 4)) + 8 != length)
        invalid();

    bool formatFound = false;
    bool dataFound = false;
    uint64_t position = 12;
    while (position < length)
    {
        unsigned char chunk[8]{};
        if (length - position < 8 || !input.read(reinterpret_cast<char*>(chunk), sizeof(chunk)))
            invalid();
        const uint64_t size = LittleEndian(chunk + 4, 4);
        position += 8;
        const uint64_t paddedSize = size + (size % 2);
        if (paddedSize > length - position)
            invalid();
        if (std::memcmp(chunk, "fmt ", 4) == 0)
        {
            unsigned char format[16]{};
            if (formatFound || dataFound || size < sizeof(format) ||
                !input.read(reinterpret_cast<char*>(format), sizeof(format)))
                invalid();
            if (LittleEndian(format, 2) != 1 || LittleEndian(format + 2, 2) != 1 ||
                LittleEndian(format + 4, 4) != 16000 || LittleEndian(format + 8, 4) != 32000 ||
                LittleEndian(format + 12, 2) != 2 || LittleEndian(format + 14, 2) != 16)
                invalid();
            formatFound = true;
        }
        else if (std::memcmp(chunk, "data", 4) == 0)
        {
            if (!formatFound || dataFound || size == 0 || size % 2 != 0)
                invalid();
            dataFound = true;
        }
        position += paddedSize;
        input.seekg(static_cast<std::streamoff>(position));
        if (!input)
            invalid();
    }
    if (!formatFound || !dataFound)
        invalid();
}

inline void ValidateWavFile(const std::string& path)
{
    std::ifstream input(std::filesystem::u8path(path), std::ios::binary);
    if (!input)
        throw std::runtime_error("Cannot open input WAV: " + path);
    ValidateWav(input);
}

inline void WriteNewFile(const std::filesystem::path& path, const std::vector<uint8_t>& bytes)
{
    FILE* file = nullptr;
#ifdef _WIN32
    _wfopen_s(&file, path.c_str(), L"wbx");
#else
    file = std::fopen(path.c_str(), "wbx");
#endif
    if (file == nullptr)
        throw std::runtime_error("Cannot create new output (existing files are never overwritten): " + path.u8string());
    const auto written = std::fwrite(bytes.data(), 1, bytes.size(), file);
    const auto closeResult = std::fclose(file);
    if (written != bytes.size() || closeResult != 0)
        throw std::runtime_error("Output write/close failed; a partial file may remain: " + path.u8string());
}

// A process watchdog also bounds SDK calls whose futures/destructors can block.
// On timeout, exit without unwinding objects still in use by native threads.
class CompletionDeadline
{
public:
    explicit CompletionDeadline(std::chrono::milliseconds timeout = std::chrono::minutes(5))
        : worker([this, timeout] {
            std::unique_lock<std::mutex> lock(mutex);
            if (!changed.wait_for(lock, timeout, [this] { return finished; }))
            {
                std::fputs("Live Interpreter completion timeout; terminating without SDK cleanup.\n", stderr);
                std::fflush(stderr);
                std::_Exit(EXIT_FAILURE);
            }
        })
    {
    }

    ~CompletionDeadline()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            finished = true;
        }
        changed.notify_all();
        worker.join();
    }

    CompletionDeadline(const CompletionDeadline&) = delete;
    CompletionDeadline& operator=(const CompletionDeadline&) = delete;

private:
    std::mutex mutex;
    std::condition_variable changed;
    bool finished = false;
    std::thread worker;
};
}
