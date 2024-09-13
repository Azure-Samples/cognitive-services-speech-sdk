# Instructions to run Microsoft Azure TTS Talking Avatar sample code

## Pre-requisites

* Follow [Text to speech quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/get-started-text-to-speech?pivots=programming-language-csharp#set-up-the-environment) to set up the environment for running Speech SDK in csharp.

## Basic Sample

This sample demonstrates the basic usage of Azure text-to-speech avatar real-time API.

* Step 1: Open a console and navigate to the folder containing this README.md document.
    * Set below varibles in appsettings.json:
        * `SpeechRegion` - the region of your Azure speech resource, e.g. westus2.
        * `SpeechKey` - the API key of your Azure speech resource.
        * `SpeechPrivateEndpoint` - the private endpoint of your Azure speech resource. e.g. https://my-speech-service.cognitiveservices.azure.com. This is optional, and only needed when you want to use private endpoint to access Azure speech service. This is optional, which is only needed when you are using custom endpoint.
    * Set below varibles if you want to use customized ICE server:
        * `IceServerUrl` - the URL of your customized ICE server.
        * `IceServerUrlRemote` - the URL of your customized ICE server for remote side. This is only required when the ICE address for remote side is different from local side.
        * `IceServerUsername` - the username of your customized ICE server.
        * `IceServerPassword` - the password of your customized ICE server.
    * Run `dotnet restore` to restore the dependencies and tools specified in the project file.
    * Run `dotnet run --urls http://localhost:5000` to start this sample.

* Step 2: Open a browser and navigate to `http://localhost:5000/basic` to view the web UI of this sample.

* Step 3: Fill or select below information:
    * TTS Configuration
        * TTS Voice - the voice of the TTS. Here is the [available TTS voices list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts#supported-languages)
        * Custom Voice Deployment ID (Endpoint ID) - the deployment ID (also called endpoint ID) of your custom voice. If you are not using a custom voice, please leave it empty.
        * Personal Voice Speaker Profile ID - the personal voice speaker profile ID of your personal voice. Please follow [here](https://learn.microsoft.com/azure/ai-services/speech-service/personal-voice-overview) to view and create personal voice.
    * Avatar Configuration
        * Avatar Character - The character of the avatar. By default it's `lisa`, and you can update this value to use a different avatar.
        * Avatar Style - The style of the avatar. You can update this value to use a different avatar style. This parameter is optional for custom avatar.
        * Background Color - The color of the avatar background.
        * Background Image (URL) - The URL of the background image. If you want to have a background image for the avatar, please fill this field. You need first upload your image to a publicly accessbile place, with a public URL. e.g. https://samples-files.com/samples/Images/jpg/1920-1080-sample.jpg
        * Custom Avatar - Check this if you are using a custom avatar.
        * Transparent Background - Check this if you want to use transparent background for the avatar. When this is checked, the background color of the video stream from server side is automatically set to green(#00FF00FF), and the js code on client side (check the `makeBackgroundTransparent` function in main.js) will do the real-time matting by replacing the green color with transparent color.
        * Video Crop - By checking this, you can crop the video stream from server side to a smaller size. This is useful when you want to put the avatar video into a customized rectangle area.

* Step 4: Click `Start Session` button to setup video connection with Azure TTS Talking Avatar service. If everything goes well, you should see a live video with an avatar being shown on the web page.

* Step 5: Type some text in the `Spoken Text` text box and click `Speak` button to send the text to Azure TTS Talking Avatar service. The service will synthesize the text to talking avatar video, and send the video stream back to the browser. The browser will play the video stream. You should see the avatar speaking the text you typed with mouth movement, and hear the voice which is synchronized with the mouth movement.

* Step 6: You can either continue to type text in the `Spoken Text` text box and let the avatar speak that text by clicking `Speak` button, or click `Stop Session` button to stop the video connection with Azure TTS Talking Avatar service. If you click `Stop Session` button, you can click `Start Session` button to start a new video connection with Azure TTS Talking Avatar service.

## Chat Sample

This sample demonstrates the chat scenario, with integration of Azure speech-to-text, Azure OpenAI, and Azure text-to-speech avatar real-time API.

* Step 1: Open a console and navigate to the folder containing this README.md document.
    * Set below varibles in appsettings.json:
        * `SpeechRegion` - the region of your Azure speech resource, e.g. westus2.
        * `SpeechKey` - the API key of your Azure speech resource.
        * `SpeechPrivateEndpoint` - the private endpoint of your Azure speech resource. e.g. https://my-speech-service.cognitiveservices.azure.com. This is optional, and only needed when you want to use private endpoint to access Azure speech service. This is optional, which is only needed when you are using custom endpoint. For more information about private endpoint, please refer to [Enable private endpoint](https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link).
        * `SpeechResourceUrl` - the URL of your Azure speech resource, e.g. /subscriptions/6e83d8b7-00dd-4b0a-9e98-dab9f060418b/resourceGroups/my-resource-group/providers/Microsoft.CognitiveServices/accounts/my-speech-resource. To fetch the speech resource URL, go to your speech resource overview page on Azure portal, click `JSON View` link, and then copy the `Resource ID` value on the popped up page. This is optional, which is only needed when you want to use private endpoint to access Azure speech service.
        * `UserAssignedManagedIdentityClientId` - the client ID of your user-assigned managed identity. This is optional, which is only needed when you want to use private endpoint with user-assigned managed identity to access Azure speech service. For more information about user-assigned managed identity, please refer to [Use a user-assigned managed identity](https://learn.microsoft.com/azure/active-directory/managed-identities-azure-resources/how-to-use-vm-token?tabs=azure-cli).
        * `AzureOpenAIEndpoint` - the endpoint of your Azure OpenAI resource, e.g. https://my-aoai.openai.azure.com/, which can be found in the `Keys and Endpoint` section of your Azure OpenAI resource in Azure portal.
        * `AzureOpenAIAPIKey` - the API key of your Azure OpenAI resource, which can be found in the `Keys and Endpoint` section of your Azure OpenAI resource in Azure portal.
        * `AzureOpenAIDeploymentName` - the name of your Azure OpenAI model deployment, which can be found in the `Model deployments` section of your Azure OpenAI resource in Azure portal.
    * Set below varibles if you want to use your own data to constrain the chat:
        * `CognitiveSearchEndpoint` - the endpoint of your Azure Cognitive Search resource, e.g. https://my-cognitive-search.search.windows.net/, which can be found in the `Overview` section of your Azure Cognitive Search resource in Azure portal, appearing at `Essentials -> Url` field.
        * `CognitiveSearchAPIKey` - the API key of your Azure Cognitive Search resource, which can be found in the `Keys` section of your Azure Cognitive Search resource in Azure portal. Please make sure to use the `Admin Key` instead of `Query Key`.
        * `CognitiveSearchIndexName` - the name of your Azure Cognitive Search index, which can be found in the `Indexes` section of your Azure Cognitive Search resource in Azure portal.
    * Set below varibles if you want to use customized ICE server:
        * `IceServerUrl` - the URL of your customized ICE server.
        * `IceServerUrlRemote` - the URL of your customized ICE server for remote side. This is only required when the ICE address for remote side is different from local side.
        * `IceServerUsername` - the username of your customized ICE server.
        * `IceServerPassword` - the password of your customized ICE server.
    * Run `dotnet restore` to restore the dependencies and tools specified in the project file.
    * Run `dotnet run --urls http://localhost:5000` to start this sample.

* Step 2: Open a browser and navigate to `http://localhost:5000/chat` to view the web UI of this sample.

* Step 3: Fill or select below information:
    * Chat Configuration
        * Azure OpenAI Deployment Name - the name of your Azure OpenAI model deployment, which can be found in the `Model deployments` section of your Azure OpenAI resource in Azure portal.
        * System Prompt - you can edit this text to preset the context for the chat API. The chat API will then generate the response based on this context.
        * Enable On Your Data - check this if you want to use your own data to constrain the chat. If you check this, you need to fill `Azure Cognitive Search Index Name` field below.
        * Azure Cognitive Search Index Name - the name of your Azure Cognitive Search index, which can be found in the `Indexes` section of your Azure Cognitive Search resource in Azure portal.
    * Speech Configuration
        * STT Locale(s) - the locale(s) of the STT. Here is the [available STT languages list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=stt#supported-languages). If multiple locales are specified, the STT will enable multi-language recognition, which means the STT will recognize the speech in any of the specified locales.
        * TTS Voice - the voice of the TTS. Here is the [available TTS voices list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts#supported-languages)
        * Custom Voice Deployment ID (Endpoint ID) - the deployment ID (also called endpoint ID) of your custom voice. If you are not using a custom voice, please leave it empty.
        * Personal Voice Speaker Profile ID - the personal voice speaker profile ID of your personal voice. Please follow [here](https://learn.microsoft.com/azure/ai-services/speech-service/personal-voice-overview) to view and create personal voice.
        * Continuous Conversation - check this if you want to enable continuous conversation. If this is checked, the STT will keep listening to your speech, with microphone always on until you click `Stop Microphone` button. If this is not checked, the microphone will automatically stop once an utterance is recognized, and you need click `Start Microphone` every time before you give a speech. The `Continuous Conversation` mode is suitable for quiet environment, while the `Non-Continuous Conversation` mode is suitable for noisy environment, which can avoid the noise being recorded while you are not speaking.
    * Avatar Configuration
        * Avatar Character - The character of the avatar. By default it's `lisa`, and you can update this value to use a different avatar.
        * Avatar Style - The style of the avatar. You can update this value to use a different avatar style. This parameter is optional for custom avatar.
        * Custom Avatar - Check this if you are using a custom avatar.
        * Auto Reconnect - Check this if you want to enable auto reconnect. If this is checked, the avatar video stream is automatically reconnected once the connection is lost.
        * Use Local Video for Idle - Check this if you want to use local video for idle part. If this is checked, the avatar video stream is replaced by local video when the avatar is idle. To use this feature, you need to prepare a local video file. Usually, you can record a video of the avatar doing idle action. [Here](https://ttspublic.blob.core.windows.net/sampledata/video/avatar/lisa-casual-sitting-idle.mp4) is a sample video for lisa-casual-sitting avatar idle status. You can download it and put it to `video/lisa-casual-sitting-idle.mp4` under the same folder of `chat.html`.

* Step 4: Click `Open Avatar Session` button to setup video connection with Azure TTS Talking Avatar service. If everything goes well, you should see a live video with an avatar being shown on the web page.

* Step 5: Click `Start Microphone` button to start microphone (make sure to allow the microphone access tip box popping up in the browser), and then you can start chatting with the avatar with speech. The chat history (the text of what you said, and the response text by the Azure OpenAI chat API) will be shown beside the avatar. The avatar will then speak out the response of the chat API.

# Additional Tip(s)

* If you want to enforce the avatar to stop speaking before the avatar finishes the utterance, you can click `Stop Speaking` button. This is useful when you want to interrupt the avatar speaking.

* If you want to clear the chat history and start a new round of chat, you can click `Clear Chat History` button. And if you want to stop the avatar service, please click `Close Avatar Session` button to close the connection with avatar service.

* If you want to type your query message instead of speaking, you can check the `Type Message` checkbox, and then type your query message in the text box showing up below the checkbox.
