# Examples to use Custom Voice

The Custom Voice API (Preview) is designed to create professional voice and personal voice. The functionality is exposed through a REST API and is easy to access from many programming languages.

For a detailed explanation see the [custom neural voice documentation](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/custom-neural-voice) and the `README.md` in the language specific subdirectories.

You can copy paste [custom voice REST API swagger doc](https://github.com/Azure/azure-rest-api-specs/blob/main/specification/cognitiveservices/data-plane/Speech/TextToSpeech/preview/2023-12-01-preview/texttospeech.json) to [Swagger Editor](https://editor.swagger.io/), then you can go through API in swagger UI.
You can find sample request/response pair for each API [here](https://github.com/Azure/azure-rest-api-specs/tree/main/specification/cognitiveservices/data-plane/Speech/TextToSpeech/preview/2023-12-01-preview/examples).

Available samples:

| Language | Directory | Description |
| ---------- | -------- | ----------- |
| C# | [csharp](csharp) | C# client calling custom voice REST API through System.Net.Http |
| Python | [python](python) | Python client calling custom voice REST API |

## Note

1. You need a Cognitive Services subscription key to run sample code here.
    - You can get the subscription key from the "Keys and Endpoint" tab on your Cognitive Services or Speech resource in the Azure Portal.
    - Custom Voice is only available for paid subscriptions, free subscriptions are not supported.
2. Both professional voice and personal voice access are [limited](https://learn.microsoft.com/en-us/legal/cognitive-services/speech-service/custom-neural-voice/limited-access-custom-neural-voice?context=%2fazure%2fcognitive-services%2fspeech-service%2fcontext%2fcontext) based on eligibility and usage criteria. Please [request access](https://aka.ms/customneural) before using sample code here.
3. Personal voice is available in these regions: West Europe, East US, and South East Asia.
