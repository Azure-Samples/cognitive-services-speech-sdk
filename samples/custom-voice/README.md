
# Examples to use Custom Voice

The Custom Voice API (Preview) is designed to create professional voice and personal voice. The functionality is exposed through a REST API and is easy to access from many programming languages.

For a detailed explanation see the [custom neural voice documentation](https://learn.microsoft.com/azure/ai-services/speech-service/custom-neural-voice) and the `README.md` in the language specific subdirectories.

REST API doc: [custom voice REST API](https://learn.microsoft.com/rest/api/aiservices/speechapi/operation-groups?view=rest-aiservices-speechapi-2024-02-01-preview).

Available samples:

| Language | Directory | Description |
| ---------- | -------- | ----------- |
| C# | [csharp](csharp) | C# client calling custom voice REST API through System.Net.Http |
| Python | [python](python) | Python client calling custom voice REST API |

## Note

1. You need a Cognitive Services subscription key to run sample code here.
    - You can get the subscription key from the "Keys and Endpoint" tab on your Cognitive Services or Speech resource in the Azure Portal.
    - Custom Voice is only available for paid subscriptions, free subscriptions are not supported.
2. Both professional voice and personal voice access are [limited](https://learn.microsoft.com/legal/cognitive-services/speech-service/custom-neural-voice/limited-access-custom-neural-voice?context=%2fazure%2fcognitive-services%2fspeech-service%2fcontext%2fcontext) based on eligibility and usage criteria. Please [request access](https://aka.ms/customneural) before using sample code here.
3. Personal voice is available in these regions: West Europe, East US, West US 2, Southeast Asia and East Asia.
