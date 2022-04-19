# C# .NET Framework console sample tool for Windows to enumerate all input and output audio devices

Speech SDK has an API to select which microphone to use for Speech Recognition. The default microphone is often used, but if you want your application to select another microphone (on a PC that has multiple microphones) you will need the Device ID of that microphone. This sample tool shows how to enumerates the Device ID of all connected microphones.

Similarly, this sample tool also lists the Device ID of all connected loudspeakers. Speech SDK has an API to select which loudspeaker to play Text-to-Speech audio, if you want to play to the non-default loudspeaker.

This sample tool uses the two Open Source NuGet packages [NAudio.Core](https://www.nuget.org/packages/NAudio.Core/) and [NAudio.Wasapi](https://www.nuget.org/packages/NAudio.Wasapi/) by Mark Heath. See [NAudio on GitHub](https://github.com/naudio/NAudio).

## Prerequisites

* A Windows PC with one or more microphone and/or loudspeakers connected.
* [Microsoft Visual Studio 2019](https://www.visualstudio.com/), Community Edition or higher.
* The **.NET desktop development** workload in Visual Studio. You can enable it in **Tools** \> **Get Tools and Features**.

## Build the sample

* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio 2019 and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
  * On a 64-bit Windows installation, choose `x64` as active solution platform.
  * On a 32-bit Windows installation, choose `x86` as active solution platform.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

## Example output

On a PC with two microphones and two loudspeakers:

```
Capture endpoint: Echo Cancelling Speakerphone (Jabra SPEAK 410 USB) ({0.0.1.00000000}.{28ffa723-afb7-4d91-ba19-0ac1829a3ade})
Capture endpoint: Desktop Microphone (Microsoftr LifeCam Cinema(TM)) ({0.0.1.00000000}.{66bbc8db-c7ee-4ed2-bc49-10a69ad00c00})
Render endpoint: Echo Cancelling Speakerphone (Jabra SPEAK 410 USB) ({0.0.0.00000000}.{9c7bb03b-de51-48a3-8256-948998270128})
Render endpoint: LG ULTRAWIDE (NVIDIA High Definition Audio) ({0.0.0.00000000}.{a897dbb2-0a33-444c-b6c4-c819fa031e09})
```
The Device ID that Speech SDK requires is the string inside the last parenthesis (for example `{0.0.1.00000000}.{28ffa723-afb7-4d91-ba19-0ac1829a3ade}`)

## References

* [Select an audio input device with the Speech SDK](https://aka.ms/csspeech/select-audio-device)
* [NAudio on GitHub](https://github.com/naudio/NAudio)
