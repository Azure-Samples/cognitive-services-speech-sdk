# C# Example to use the Speech Services Batch Synthesis API

To successfully run this example, you must use one of the following authentication methods:

Option 1: Token-based authentication (Recommended)

- Set the Speech endpoint (set `SPEECH_ENDPOINT`)
  - Ensure your Microsoft Entra account is assigned the "Cognitive Services Speech Contributor" or "Cognitive Services User" role.

Option 2: Key-based authentication

- Set the Speech Service subscription key (set `SPEECH_KEY`)
- Set the region matching your subscription key (set `SPEECH_REGION`)

Optionally:

- The relationship between custom voice names and deployment ID, if you want to use custom voices
- The URI of a writable Azure blob container, if you want to store the audio files in your own Azure storage
