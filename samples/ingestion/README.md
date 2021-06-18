# Examples to use the Speech Services Transcription at scale

Azure Speechis designed to handle a large number of audio fragments in storage, such as Azure Blobs. The functionality is exposed through a REST API and is easy to access from many programming languages. Some samples here do **NOT** require the installation of the Cognitive Service Speech SDK, but use the REST API directly or through additional helper libraries.

For a detailed explanation see the [batch transcription documentation](https://docs.microsoft.com/azure/cognitive-services/speech-service/batch-transcription) and the `README.md` in the language specific subdirectories.

Available samples:

| Language | Directory | Description |
| ---------- | -------- | ----------- |
| C# | [csharp](csharp) | C# calling batch transcription REST API through System.Net.Http |
| C# | [Ingestion Client](ingestion-client) | Project to automatically transcribe all audio files which are added to an Azure Storage Container. Setup via ARM template. |
| Python | [python](python) | Python client calling batch transcription REST API |
| Node.js | [js/node](js/node) | Node.js client calling batch transcription REST API |
