# Instructions to run Microsoft Azure TTS Talking Avatar sample code

This sample demonstrates the basic usage of Azure text-to-speech avatar real-time API.

* Step 1: Follow [Text to speech quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/get-started-text-to-speech?pivots=programming-language-python#set-up-the-environment) to set up the environment for running Speech SDK in python.

* Step 2: Open a console and navigate to the folder containing this README.md document.
    * Run `pip install -r requirements.txt` to install the required packages.
    * Run `python -m flask run -h 0.0.0.0 -p 5000` to start this sample.

* Step 3: Open a browser and navigate to `http://localhost:5000` to view the web UI of this sample.

* Step 4: Fill or select below information:
    * TTS Configuration
        * TTS Voice - the voice of the TTS. Here is the [available TTS voices list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts#supported-languages)
        * Custom Voice Deployment ID (Endpoint ID) - the deployment ID (also called endpoint ID) of your custom voice. If you are not using a custom voice, please leave it empty.
        * Personal Voice Speaker Profile ID - the personal voice speaker profile ID of your personal voice. Please follow [here](https://learn.microsoft.com/azure/ai-services/speech-service/personal-voice-overview) to view and create personal voice.
        * Enable Private Endpoint - check this if you want to apply private endpoint. If you check this, you need to fill Private Endpoint section below. Please refer to [speech-services-private-link](https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link) to learn more about private endpoint.
        * Private EndPoint - the private endpoint of your Azure speech resource. The format should be like: `https://{your custom name}.cognitiveservices.azure.com`.
    * Avatar Configuration
        * Avatar Character - The character of the avatar. By default it's `lisa`, and you can update this value to use a different avatar.
        * Avatar Style - The style of the avatar. You can update this value to use a different avatar style. This parameter is optional for custom avatar.
        * Background Color - The color of the avatar background.
        * Custom Avatar - Check this if you are using a custom avatar.
        * Transparent Background - Check this if you want to use transparent background for the avatar. When this is checked, the background color of the video stream from server side is automatically set to green(#00FF00FF), and the js code on client side (check the `makeBackgroundTransparent` function in main.js) will do the real-time matting by replacing the green color with transparent color.
        * Video Crop - By checking this, you can crop the video stream from server side to a smaller size. This is useful when you want to put the avatar video into a customized rectangle area.

* Step 5: Click `Start Session` button to setup video connection with Azure TTS Talking Avatar service. If everything goes well, you should see a live video with an avatar being shown on the web page.

* Step 6: Type some text in the `Spoken Text` text box and click `Speak` button to send the text to Azure TTS Talking Avatar service. The service will synthesize the text to talking avatar video, and send the video stream back to the browser. The browser will play the video stream. You should see the avatar speaking the text you typed with mouth movement, and hear the voice which is synchronized with the mouth movement.

* Step 7: You can either continue to type text in the `Spoken Text` text box and let the avatar speak that text by clicking `Speak` button, or click `Stop Session` button to stop the video connection with Azure TTS Talking Avatar service. If you click `Stop Session` button, you can click `Start Session` button to start a new video connection with Azure TTS Talking Avatar service.
