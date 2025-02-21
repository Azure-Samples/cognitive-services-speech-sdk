# Examples to use personal voice

Personal voice API is designed to create AI voice replicas for your users. This feature is available through a set of REST APIs and is easy to access from many programming languages.

For a detailed introduction, see the [personal voice documentation](https://learn.microsoft.com/azure/ai-services/speech-service/personal-voice-overview).

REST API doc: Personal voice REST API is a part of [custom voice REST API](https://learn.microsoft.com/rest/api/aiservices/speechapi/operation-groups?view=rest-aiservices-speechapi-2024-02-01-preview).

## Available samples

| Language | Sample code | Description |
|----------|-----------|-------------|
| C#       | [csharp](custom-voice/csharp/CustomVoiceSample/PersonalVoiceSample.cs) | C# client calling personal voice REST API through System.Net.Http |
| Python   | [python](custom-voice/python/personal_voice_sample.py) | Python client calling personal voice REST API |

> [!NOTE]
> You need an Azure AI services [subscription key](https://learn.microsoft.com/azure/ai-services/speech-service/get-started-text-to-speech?tabs=windows%2Cterminal&pivots=programming-language-rest#prerequisites) resource to run the sample code provided here. Personal voice is only available for paid resources (S0); free resources (F0) aren't supported.

> Personal voice access is [limited](https://learn.microsoft.com/legal/cognitive-services/speech-service/custom-neural-voice/limited-access-custom-neural-voice?context=%2fazure%2fcognitive-services%2fspeech-service%2fcontext%2fcontext) based on eligibility and usage criteria. [Request access](https://aka.ms/customneural) before using the sample code here.

> Personal voice is available in these regions: West Europe, East US, West US 2, Southeast Asia and East Asia.

> For supported locales, see [personal voice language support](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts#personal-voice).
