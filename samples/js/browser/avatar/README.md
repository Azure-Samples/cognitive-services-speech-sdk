# Instructions to run Microsoft Azure TTS Talking Avatar sample code

## Basic Sample

This sample demonstrates the basic usage of Azure text-to-speech avatar real-time API.

* Step 1: Run the sample code by opening basic.html in a browser.

* Step 2: Fill or select below information:
    * Azure Speech Resource
        * Region - the region of your Azure speech resource.
        * Subscription Key - the subscription key of your Azure speech resource.
    * ICE Server
        * URL - the ICE server URL for WebRTC. e.g. `turn:relay.communication.microsoft.com:3478`. You can get the ICE server from ACS ([Azure Communication Services](https://learn.microsoft.com/azure/communication-services/overview)): you need follow [Create communication resource](https://learn.microsoft.com/azure/communication-services/quickstarts/create-communication-resource?tabs=windows&pivots=platform-azp) to create ACS resource, and then follow [Getting the relay configuration](https://learn.microsoft.com/azure/communication-services/quickstarts/relay-token?pivots=programming-language-python#getting-the-relay-configuration) to get ICE server url, ICE server username, and ICE server credential. For ICE server url, please make sure to use prefix `turn:`, instead of `stun:`.
        * IceServerUsername - the username of the ICE server, which is provided together with the ICE server URL (see above).
        * IceServerCredential - the credential (password) of the ICE server, which is provided together with the ICE server URL (see above).
    * TTS Configuration
        * TTS Voice - the voice of the TTS. Here is the [available TTS voices list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts#supported-languages)
        * Custom Voice Deployment ID (Endpoint ID) - the deployment ID (also called endpoint ID) of your custom voice. If you are not using a custom voice, please leave it empty.
    * Avatar Configuration
        * Avatar Character - The character of the avatar. By default it's 'lisa', and you can update this value to use a different avatar.
        * Avatar Style - The style of the avatar. You can update this value to use a different avatar style. This parameter is optional for custom avatar.
        * Background Color - The color of the avatar background.
        * Custom Avatar - Check this if you are using a custom avatar.
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
        * Subscription Key - the subscription key of your Azure speech resource.
    * Azure OpenAI Resource
        * Endpoint - the endpoint of your Azure OpenAI resource, e.g. https://your-openai-resource-name.openai.azure.com/, which can be found in the `Keys and Endpoint` section of your Azure OpenAI resource in Azure portal.
        * API Key - the API key of your Azure OpenAI resource, which can be found in the `Keys and Endpoint` section of your Azure OpenAI resource in Azure portal.
        * Deployment Name - the name of your Azure OpenAI model deployment, which can be found in the `Model deployments` section of your Azure OpenAI resource in Azure portal.
        * Enable BYOD (Bring Your Own Data) - check this if you want to use your own data to constrain the chat. If you check this, you need to fill `Azure Cognitive Search Resource` section below.
    * Azure Cognitive Search Resource - if you want to constrain the chat within your own data, please follow [Quickstart: Chat with Azure OpenAI models using your own data](https://learn.microsoft.com/azure/cognitive-services/openai/use-your-data-quickstart?pivots=programming-language-studio) to create your data source, and then fill below information:
        * Endpoint - the endpoint of your Azure Cognitive Search resource, e.g. https://your-cogsearch-resource-name.search.windows.net/, which can be found in the `Overview` section of your Azure Cognitive Search resource in Azure portal, appearing at `Essentials -> Url` field.
        * API Key - the API key of your Azure Cognitive Search resource, which can be found in the `Keys` section of your Azure Cognitive Search resource in Azure portal. Please make sure to use the `Admin Key` instead of `Query Key`.
        * Index Name - the name of your Azure Cognitive Search index, which can be found in the `Indexes` section of your Azure Cognitive Search resource in Azure portal.
    * ICE Server
        * URL - the ICE server URL for WebRTC. e.g. `turn:relay.communication.microsoft.com:3478`. You can get the ICE server from ACS ([Azure Communication Services](https://learn.microsoft.com/azure/communication-services/overview)): you need follow [Create communication resource](https://learn.microsoft.com/azure/communication-services/quickstarts/create-communication-resource?tabs=windows&pivots=platform-azp) to create ACS resource, and then follow [Getting the relay configuration](https://learn.microsoft.com/azure/communication-services/quickstarts/relay-token?pivots=programming-language-python#getting-the-relay-configuration) to get ICE server url, ICE server username, and ICE server credential. For ICE server url, please make sure to use prefix `turn:`, instead of `stun:`.
        * IceServerUsername - the username of the ICE server, which is provided together with the ICE server URL (see above).
        * IceServerCredential - the credential (password) of the ICE server, which is provided together with the ICE server URL (see above).
    * STT / TTS Configuration
        * STT Locale - the locale of the STT. Here is the [available STT languages list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=stt#supported-languages)
        * TTS Voice - the voice of the TTS. Here is the [available TTS voices list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts#supported-languages)
        * Custom Voice Deployment ID (Endpoint ID) - the deployment ID (also called endpoint ID) of your custom voice. If you are not using a custom voice, please leave it empty.
    * Avatar Configuration
        * Avatar Character - The character of the avatar. By default it's 'lisa', and you can update this value to use a different avatar.
        * Avatar Style - The style of the avatar. You can update this value to use a different avatar style. This parameter is optional for custom avatar.
        * Custom Avatar - Check this if you are using a custom avatar.

* Step 3: Click `Open Video Connection` button to setup video connection with Azure TTS Talking Avatar service. If everything goes well, you should see a live video with an avatar being shown on the web page.

* Step 4: Click `Start Microphone` button to start microphone (make sure to allow the microphone access tip box popping up in the browser), and then you can start chatting with the avatar with speech. The chat history (the text of what you said, and the response text by the Azure OpenAI chat API) will be shown beside the avatar. The avatar will then speak out the response of the chat API.

* Step 5: If you want to clear the chat history and start a new round of chat, you can click `Clear Chat History` button. And if you want to stop the avatar service, please click `Close Video Connection` button to close the connection with avatar service. 

# Additional Tip(s)

* For the chat sample, you can edit the text in `System Prompt` text box to preset the context for the chat API. The chat API will then generate the response based on this context.
