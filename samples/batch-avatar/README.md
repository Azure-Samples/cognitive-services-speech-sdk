# Examples to use Batch Avatar Synthesis

The Batch avatar synthesis API (Preview) provides asynchronous synthesis of talking avatar to generate avatar video content with the text input.
The functionality is exposed through a REST API and is easy to access from many programming languages. The samples here do **NOT** require the installation of the Cognitive Service Speech SDK, but use the REST API directly instead.

For a detailed explanation see the [batch avatar synthesis documentation](https://learn.microsoft.com/azure/ai-services/speech-service/text-to-speech-avatar/batch-synthesis-avatar) and the `README.md` in the language specific subdirectories.

Available samples:

| Language | Directory | Description |
| ---------- | -------- | ----------- |
| Python | [python](python) | Python client calling batch avatar synthesis REST API |
| C# | [csharp](csharp) | C# client calling batch avatar REST API. |

## Note

Refer to [this](../js/browser/avatar/README.md) for real time avatar synthesis.


## Resources

1. [Batch avatar synthesis request properties](https://learn.microsoft.com/azure/ai-services/speech-service/text-to-speech-avatar/batch-synthesis-avatar-properties)
2. [The OPENAPI specification for the Batch avatar synthesis API](https://github.com/Azure/azure-rest-api-specs/blob/main/specification/cognitiveservices/data-plane/Speech/BatchAvatar/stable/2024-08-01/batchavatar.json)
