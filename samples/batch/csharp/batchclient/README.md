**C# Example to use the Speech Services Batch Transcription API**

Speech Service Batch Transcription is exposed through a REST API. The samples here does **NOT** require the installation of the Cognitive Service Speech SDK, but uses the REST API directly.

For detailed explanation see the [batch transcription documentation](https://docs.microsoft.com/azure/cognitive-services/speech-service/batch-transcription).

To successfully run this example you require

- a Speech Service subscription key,
- the region information matching your subscription key,
- the shared access signature (SAS) URI of the audio file you want to transcribe,

and optionally

- the IDs of any acoustic or language models your want to apply.

With this information, change the definitions of the appropriate constants in `program.cs`.