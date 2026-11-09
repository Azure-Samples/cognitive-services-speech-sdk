---
ai-usage: ai-assisted
---

# Live Interpreter samples

These standalone samples translate a WAV file into French text and speech
with Live Interpreter. They use open-range language identification and
`personal-voice`; you don't specify an input language or candidate languages.
The existing standard speech translation samples are unchanged.

## Prerequisites

- An Azure Speech resource in a
  [region that supports Live Interpreter](https://learn.microsoft.com/azure/ai-services/speech-service/regions?tabs=speech-translation).
- Approved [personal voice access](https://aka.ms/customneural) for the
  resource used by these samples.
- A mono, 16-bit PCM WAV file at 16 kHz. You can use
  [`whatstheweatherlike.wav`](../python/console/whatstheweatherlike.wav).
- The language-specific tools described below and the
  [Speech SDK platform prerequisites](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform).
- A writable output directory. Each sample creates `translation-N.wav`
  files there. Run from a fresh directory or move previous output files
  before running again; the samples fail instead of overwriting them.

For service configuration and language support, see
[How to translate speech](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-translate-speech).
Running against the service can incur charges.

## Configure the sample

Set the following environment variables in the terminal used to run a sample.
Don't put resource keys in source code or commit them to Git.

| Variable | Value |
| --- | --- |
| `SPEECH_KEY` | Your Speech resource key. |
| `SPEECH_ENDPOINT` | Your resource's universal v2 endpoint, such as `wss://<resource-name>.cognitiveservices.azure.com/stt/speech/universal/v2`. |
| `LIVE_INTERPRETER_AUDIO_FILE` | An absolute path to the input WAV file. |

PowerShell:

```powershell
$env:SPEECH_KEY = "<your-speech-resource-key>"
$env:SPEECH_ENDPOINT = "wss://<resource-name>.cognitiveservices.azure.com/stt/speech/universal/v2"
$env:LIVE_INTERPRETER_AUDIO_FILE = "C:\audio\input.wav"
```

Bash or zsh:

```bash
export SPEECH_KEY="<your-speech-resource-key>"
export SPEECH_ENDPOINT="wss://<resource-name>.cognitiveservices.azure.com/stt/speech/universal/v2"
export LIVE_INTERPRETER_AUDIO_FILE="/absolute/path/to/input.wav"
```

### Select the LLM-based Live Interpreter

To select the large language model (LLM)-based Live Interpreter, append
`?setfeature=llm` to `SPEECH_ENDPOINT`. If the endpoint already contains query
parameters, append `&setfeature=llm` instead.

```text
wss://<resource-name>.cognitiveservices.azure.com/stt/speech/universal/v2?setfeature=llm
```

Every sample passes this endpoint to the SDK without removing its query
parameters. Keep the target language, personal voice, and open-range
configuration unchanged. Omit the query parameter to use the base endpoint.

## Choose a language

| Language | Sample | Tools |
| --- | --- | --- |
| Python | [Python](python) | Python 3.9 or later and pip. |
| C# | [C#](csharp) | .NET 8 SDK or later. |
| JavaScript | [Node.js](javascript) | Node.js 22 or later and npm. |
| Java | [Java](java) | JDK 11 or later and Maven. |
| C++ | [C++](cpp) | A C++17 compiler, CMake 3.20 or later, and the native Speech SDK. |
| Go | [Go](go) | Linux, Go, a C compiler, and the native Speech SDK. |
| Objective-C | [Objective-C](objective-c) | macOS, Xcode command-line tools, and the Speech SDK framework. |
| Swift | [Swift](swift) | macOS, Xcode command-line tools, and the Speech SDK framework. |

The package manifests pin Speech SDK 1.53.0. Use the same version for native
SDK headers and libraries. For Go and native SDK installation, follow the
language-specific guidance.

### Python

From `python`, create a virtual environment and install the dependency:

```text
python -m venv .venv
```

Activate it with `.venv\Scripts\Activate.ps1` in PowerShell, or
`source .venv/bin/activate` in Bash or zsh, then run:

```text
python -m pip install -r requirements.txt
python live_interpreter.py
```

Run the offline tests with `python -m unittest -v`.

### C#

From `csharp`, run:

```text
dotnet run
```

The project requires no audio or JSON packages beyond the Speech SDK.

### JavaScript

From `javascript`, run:

```text
npm install
npm start
```

Run the offline tests with `npm test`.

### Java

Set `JAVA_HOME` to your JDK installation. From `java`, run:

```text
mvn compile exec:java
```

### C++

Use an x64 or ARM64 toolchain and matching native Speech SDK libraries.
Set `SPEECHSDK_ROOT` to an extracted native SDK or
`Microsoft.CognitiveServices.Speech` NuGet package directory.
The build doesn't download the SDK. If you already built the C# sample,
CMake can find NuGet 1.53.0 in the default package cache.

From `cpp`, configure and build:

```text
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

For a Visual Studio generator, run `build\Release\live_interpreter.exe`
in PowerShell. For a single-configuration generator such as Ninja, run
`build\live_interpreter.exe` on Windows or `./build/live_interpreter`
on Linux. Select the matching architecture with
`-DSPEECHSDK_ARCH=x64` or `-DSPEECHSDK_ARCH=arm64` when needed.

### Go on Linux

Follow the [Go SDK prerequisites](https://github.com/microsoft/cognitive-services-speech-sdk-go#prerequisites)
to install Go, a C compiler, and the native Speech SDK 1.53.0.
Enable cgo and set the native header, linker, and runtime library paths.
For an extracted Linux x64 SDK, for example:

```bash
export SPEECHSDK_ROOT="/absolute/path/to/extracted/speechsdk"
export CGO_ENABLED=1
export CGO_CFLAGS="-I${SPEECHSDK_ROOT}/include/c_api"
export CGO_LDFLAGS="-L${SPEECHSDK_ROOT}/lib/x64"
export LD_LIBRARY_PATH="${SPEECHSDK_ROOT}/lib/x64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
```

Use the corresponding native library directory for your architecture.
From `go`, run:

```bash
go mod download
go test -race -count=1 -timeout 60s .
go run .
```

The released Go binding doesn't expose the native handles needed for complete
cleanup of synthesis events and results. These examples are short-lived
console programs; don't reuse the Go synthesis callback unchanged in a
long-running application without addressing that binding limitation.

### Objective-C and Swift on macOS

Install the Xcode command-line tools and download the
[Speech SDK for macOS](https://aka.ms/csspeech/macosbinary).
Use the macOS slice of the XCFramework, not an iOS slice.

Set `SPEECHSDK_ROOT` to the directory containing
`MicrosoftCognitiveServicesSpeech.framework`. For example:

```bash
export SPEECHSDK_ROOT="/path/to/MicrosoftCognitiveServicesSpeech.xcframework/macos-arm64_x86_64"
```

From `objective-c` or `swift`, build and run:

```bash
make
./live-interpreter
```

Alternatively, pass `SPEECHSDK_FRAMEWORK="/full/path/to/MicrosoftCognitiveServicesSpeech.framework"`
to `make`. The Makefiles set the framework search path and runtime search path
for the supplied framework. Swift builds in Swift 5 language mode.

## Expected results

Each sample prints recognized text, the detected source language, and French
translations. Synthesized audio is written to numbered `translation-N.wav`
files in the current directory; the samples don't play audio through speakers.
Change the `"fr"` target-language value in the sample to use another supported
target language.

The samples wait up to five minutes for the file-translation session.
Use a short input file, or adjust the timeout for longer recordings.
An empty synthesis event doesn't end the session: the samples wait for
session completion or cancellation and stop recognition outside SDK callbacks.
Configuration, cancellation errors, output-file errors, and timeouts cause
a nonzero exit.

The native C++ and Go samples validate the input WAV format before starting
recognition. Their watchdogs also bound blocked native calls. On timeout,
they terminate the process without normal SDK cleanup rather than attempting
to dispose resources while a native operation is still running.

## Troubleshooting

- Missing environment variable: set the variable named in the error message
  in the same terminal that runs the sample.
- Input file error: check the absolute path and WAV format.
- Output file already exists: use a fresh output directory or move the
  previous output files.
- Service cancellation: check the reported error details, resource key,
  endpoint, regional availability, and personal voice access.
- Translation timeout: verify network access and try a shorter audio file.

The offline tests verify configuration and file handling without using a
Speech resource. They don't verify service access, translation quality, or
the LLM backend. Those require an appropriately enabled resource.
