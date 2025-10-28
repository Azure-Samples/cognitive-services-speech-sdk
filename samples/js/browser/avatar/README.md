# Instructions to run Microsoft Azure TTS Talking Avatar sample code

## Basic Sample

This sample demonstrates the basic usage of Azure text-to-speech avatar real-time API.

* Step 1: Run the sample code by opening basic.html in a browser.

* Step 2: Fill or select below information:
    * Azure Speech Resource
        * Region - the region of your Azure speech resource.
        * API Key - the API key of your Azure speech resource.
        * Enable Private Endpoint - check this if you want to apply private endpoint. If you check this, you need to fill Private Endpoint section below. Please refer to [speech-services-private-link](https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link) to learn more about private endpoint.
        * Private EndPoint - the private endpoint of your Azure speech resource. The format should be like: `https://{your custom name}.cognitiveservices.azure.com`.
    * TTS Configuration
        * TTS Voice - the voice of the TTS. Here is the [available TTS voices list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts#supported-languages)
        * Custom Voice Deployment ID (Endpoint ID) - the deployment ID (also called endpoint ID) of your custom voice. If you are not using a custom voice, please leave it empty.
    * Avatar Configuration
        * Avatar Character - The character of the avatar. By default it's `lisa`, and you can update this value to use a different avatar.
        * Avatar Style - The style of the avatar. You can update this value to use a different avatar style. This parameter is optional for custom avatar.
        * Background Color - The color of the avatar background.
        * Photo Avatar - Check this if you want to use photo avatar.
        * Custom Avatar - Check this if you are using a custom avatar.
        * Use Built-In Voice - Check this if you want to use built-in voice, the voice that was built together with your custom avatar.
        * Transparent Background - Check this if you want to use transparent background for the avatar. When this is checked, the background color of the video stream from server side is automatically set to green(#00FF00FF), and the js code on client side (check the `makeBackgroundTransparent` function in main.js) will do the real-time matting by replacing the green color with transparent color.
        * Video Crop - By checking this, you can crop the video stream from server side to a smaller size. This is useful when you want to put the avatar video into a customized rectangle area.

* Step 3: Click `Start Session` button to setup video connection with Azure TTS Talking Avatar service. If everything goes well, you should see a live video with an avatar being shown on the web page.

* Step 4: Type some text in the `Spoken Text` text box and click `Speak` button to send the text to Azure TTS Talking Avatar service. The service will synthesize the text to talking avatar video, and send the video stream back to the browser. The browser will play the video stream. You should see the avatar speaking the text you typed with mouth movement, and hear the voice which is synchronized with the mouth movement.

* Step 5: You can either continue to type text in the `Spoken Text` text box and let the avatar speak that text by clicking `Speak` button, or click `Stop Session` button to stop the video connection with Azure TTS Talking Avatar service. If you click `Stop Session` button, you can click `Start Session` button to start a new video connection with Azure TTS Talking Avatar service.

## Chat Sample

This sample demonstrates the chat scenario, with integration of Azure speech-to-text, Azure OpenAI, and Azure text-to-speech avatar real-time API.

* Step 1: Run the sample code by opening chat.html in a browser.

* Step 2: Fill or select below information:
    * Azure Speech Resource
        * Region - the region of your Azure speech resource.
        * API Key - the API key of your Azure speech resource.
        *  Enable Private Endpoint - check this if you want to apply private endpoint. If you check this, you need to fill Private Endpoint section below. Please refer to [speech-services-private-link](https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link) to learn more about private endpoint.
        * Private EndPoint - the private endpoint of your Azure speech resource. The format should be like: `https://{your custom name}.cognitiveservices.azure.com`.
    * Azure OpenAI Resource
        * Endpoint - the endpoint of your Azure OpenAI resource, e.g. https://your-openai-resource-name.openai.azure.com/, which can be found in the `Keys and Endpoint` section of your Azure OpenAI resource in Azure portal.
        * API Key - the API key of your Azure OpenAI resource, which can be found in the `Keys and Endpoint` section of your Azure OpenAI resource in Azure portal.
        * Deployment Name - the name of your Azure OpenAI model deployment, which can be found in the `Model deployments` section of your Azure OpenAI resource in Azure portal.
        * System Prompt - you can edit this text to preset the context for the chat API. The chat API will then generate the response based on this context.
        * Enable On Your Data - check this if you want to use your own data to constrain the chat. If you check this, you need to fill `Azure Cognitive Search Resource` section below.
    * Azure Cognitive Search Resource - if you want to constrain the chat within your own data, please follow [Quickstart: Chat with Azure OpenAI models using your own data](https://learn.microsoft.com/azure/cognitive-services/openai/use-your-data-quickstart?pivots=programming-language-studio) to create your data source, and then fill below information:
        * Endpoint - the endpoint of your Azure Cognitive Search resource, e.g. https://your-cogsearch-resource-name.search.windows.net/, which can be found in the `Overview` section of your Azure Cognitive Search resource in Azure portal, appearing at `Essentials -> Url` field.
        * API Key - the API key of your Azure Cognitive Search resource, which can be found in the `Keys` section of your Azure Cognitive Search resource in Azure portal. Please make sure to use the `Admin Key` instead of `Query Key`.
        * Index Name - the name of your Azure Cognitive Search index, which can be found in the `Indexes` section of your Azure Cognitive Search resource in Azure portal.
    * STT / TTS Configuration
        * STT Locale(s) - the locale(s) of the STT. Here is the [available STT languages list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=stt#supported-languages). If multiple locales are specified, the STT will enable multi-language recognition, which means the STT will recognize the speech in any of the specified locales.
        * TTS Voice - the voice of the TTS. Here is the [available TTS voices list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts#supported-languages)
        * Custom Voice Deployment ID (Endpoint ID) - the deployment ID (also called endpoint ID) of your custom voice. If you are not using a custom voice, please leave it empty.
        * Continuous Conversation - check this if you want to enable continuous conversation. If this is checked, the STT will keep listening to your speech, with microphone always on until you click `Stop Microphone` button. If this is not checked, the microphone will automatically stop once an utterance is recognized, and you need click `Start Microphone` every time before you give a speech. The `Continuous Conversation` mode is suitable for quiet environment, while the `Non-Continuous Conversation` mode is suitable for noisy environment, which can avoid the noise being recorded while you are not speaking.
    * Avatar Configuration
        * Avatar Character - The character of the avatar. By default it's `lisa`, and you can update this value to use a different avatar.
        * Avatar Style - The style of the avatar. You can update this value to use a different avatar style. This parameter is optional for custom avatar.
        * Photo Avatar - Check this if you want to use photo avatar.
        * Custom Avatar - Check this if you are using a custom avatar.
        * Use Built-In Voice - Check this if you want to use built-in voice, the voice that was built together with your custom avatar.
        * Auto Reconnect - Check this if you want to enable auto reconnect. If this is checked, the avatar video stream is automatically reconnected once the connection is lost.
        * Use Local Video for Idle - Check this if you want to use local video for idle part. If this is checked, the avatar video stream is replaced by local video when the avatar is idle. To use this feature, you need to prepare a local video file. Usually, you can record a video of the avatar doing idle action. [Here](https://ttspublic.blob.core.windows.net/sampledata/video/avatar/lisa-casual-sitting-idle.mp4) is a sample video for lisa-casual-sitting avatar idle status. You can download it and put it to `video/lisa-casual-sitting-idle.mp4` under the same folder of `chat.html`.

* Step 3: Click `Open Avatar Session` button to setup video connection with Azure TTS Talking Avatar service. If everything goes well, you should see a live video with an avatar being shown on the web page.

* Step 4: Click `Start Microphone` button to start microphone (make sure to allow the microphone access tip box popping up in the browser), and then you can start chatting with the avatar with speech. The chat history (the text of what you said, and the response text by the Azure OpenAI chat API) will be shown beside the avatar. The avatar will then speak out the response of the chat API.

# Additional Tip(s)

* If you want to enforce the avatar to stop speaking before the avatar finishes the utterance, you can click `Stop Speaking` button. This is useful when you want to interrupt the avatar speaking.

* If you want to clear the chat history and start a new round of chat, you can click `Clear Chat History` button. And if you want to stop the avatar service, please click `Close Avatar Session` button to close the connection with avatar service.

* If you want to type your query message instead of speaking, you can check the `Type Message` checkbox, and then type your query message in the text box showing up below the checkbox.
