# C# Example to use the Speech Services Batch Transcription API

Speech Services Batch Transcription is exposed through a REST API. The samples here do **NOT** require the installation of the Cognitive Service Speech SDK, but use the REST API directly.

For detailed explanation see the [batch transcription documentation](https://docs.microsoft.com/azure/cognitive-services/speech-service/batch-transcription).

To successfully run this example you require

- a Speech Service subscription key,
- the region information matching your subscription key,
- the shared access signature (SAS) URI of the audio file you want to transcribe,

and optionally

- the references of any custom models your want to apply.
- a deployment of an Azure function to handle webhook notifications, as demonstrated in the [webhookreceiver](webhookreceiver/webhookreceiver.cs) sample.

With this information, change the definitions of the appropriate constants in `program.cs`.

