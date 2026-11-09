// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md in the project root.

#include "sample_helpers.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace
{
void Check(bool condition, const char* message)
{
    if (!condition)
        throw std::runtime_error(message);
}

template <typename Action>
void ExpectFailure(Action action)
{
    bool failed = false;
    try { action(); }
    catch (const std::runtime_error&) { failed = true; }
    Check(failed, "Expected an explicit failure.");
}

std::string Wav()
{
    const unsigned char bytes[] = {
        'R','I','F','F', 38,0,0,0, 'W','A','V','E',
        'f','m','t',' ', 16,0,0,0, 1,0, 1,0,
        0x80,0x3e,0,0, 0,0x7d,0,0, 2,0, 16,0,
        'd','a','t','a', 2,0,0,0, 0,0
    };
    return std::string(reinterpret_cast<const char*>(bytes), sizeof(bytes));
}

void Validate(const std::string& bytes)
{
    std::istringstream input(bytes, std::ios::binary);
    live_interpreter::ValidateWav(input);
}
}

int main(int argc, char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--watchdog")
    {
        live_interpreter::CompletionDeadline deadline(std::chrono::milliseconds(30));
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return 0;
    }
    try
    {
        {
            live_interpreter::CompletionDeadline deadline(std::chrono::seconds(1));
        }
        auto state = std::make_shared<live_interpreter::SessionState>();
        live_interpreter::OnEvent(state, [] { throw std::runtime_error("test callback failure"); });
        Check(state->failed, "Callback failure was not recorded.");
        Check(!state->done, "A callback failure must not signal session completion.");
        live_interpreter::OnEvent(state, [&] { state->done = true; });
        Check(state->done && state->failed, "Terminal completion lost the callback failure.");
        Validate(Wav());
        ExpectFailure([] { Validate(""); });
        auto bad = Wav();
        bad[0] = 'X';
        ExpectFailure([&] { Validate(bad); });
        bad = Wav();
        bad[22] = 2; // Stereo is outside this sample's input contract.
        ExpectFailure([&] { Validate(bad); });
        bad = Wav();
        bad[24] = 0;
        ExpectFailure([&] { Validate(bad); });
        bad = Wav();
        bad[34] = 8;
        ExpectFailure([&] { Validate(bad); });
        bad = Wav();
        bad[40] = 100;
        ExpectFailure([&] { Validate(bad); });
        bad = Wav();
        bad[4] = 37;
        ExpectFailure([&] { Validate(bad); });
        bad = Wav();
        bad.replace(36, 4, "JUNK");
        ExpectFailure([&] { Validate(bad); });
        bad = Wav();
        bad[20] = 3; // IEEE float is not PCM.
        ExpectFailure([&] { Validate(bad); });
        bad = Wav();
        bad[32] = 4;
        ExpectFailure([&] { Validate(bad); });
        ExpectFailure([] { Validate(Wav().substr(0, 45)); });
        bad = Wav();
        bad[4] = 36;
        bad[40] = 0;
        bad.resize(44);
        ExpectFailure([&] { Validate(bad); });
        bad = Wav();
        bad[4] = 48;
        bad.insert(12, "JUNK\1\0\0\0x\0", 10);
        Validate(bad); // Unknown chunks and odd-byte padding are valid.
        bad = Wav();
        bad.append(bad.substr(36));
        bad[4] = 48;
        ExpectFailure([&] { Validate(bad); });

        const auto directory = std::filesystem::current_path() / "helper-test-output";
        Check(std::filesystem::create_directory(directory), "Test directory already exists.");
        const auto output = directory / "translation-1.wav";
        const std::vector<uint8_t> bytes{1, 2, 3};
        live_interpreter::WriteNewFile(output, bytes);
        ExpectFailure([&] { live_interpreter::WriteNewFile(output, {4}); });
        std::ifstream saved(output, std::ios::binary);
        const std::vector<uint8_t> actual{
            std::istreambuf_iterator<char>(saved), std::istreambuf_iterator<char>()};
        Check(actual == bytes, "Existing output was overwritten.");
        saved.close();
        ExpectFailure([&] { live_interpreter::WriteNewFile(directory / "missing" / "out.wav", bytes); });
        std::filesystem::remove(output);
        std::filesystem::remove(directory);
        std::cout << "WAV validation and exclusive-output tests passed.\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
