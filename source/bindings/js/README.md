# TODO - this is an old ReadMe

Kept for reference. Will be edited / removed in the future.

[![npm version](https://badge.fury.io/js/microsoft-speech-browser-sdk.svg)](https://www.npmjs.com/package/microsoft-speech-browser-sdk)

## Prerequisites

### Subscribe to the Speech Recognition API, and get a free trial subscription key

The Speech API is part of Cognitive Services. You can get free trial subscription keys from the [Cognitive Services subscription](https://azure.microsoft.com/try/cognitive-services/) page. After you select the Speech API, select **Get API Key** to get the key. It returns a primary and secondary key. Both keys are tied to the same quota, so you can use either key.

**Note:** Before you can use Speech client libraries, you must have a [subscription key](https://azure.microsoft.com/try/cognitive-services/).

## Get started

In this section we will walk you through the necessary steps to load a sample HTML page. The sample is located in our [github repository](https://github.com/Azure-Samples/SpeechToText-WebSockets-Javascript). You can **open the sample directly** from the repository, or **open the sample from a local copy** of the repository. 

**Note:** Some browsers block microphone access on un-secure origin. So, it is recommended to host the 'sample'/'your app' on https to get it working on all supported browsers. 

### Open the sample directly

Acquire a subscription key as described above. Then open the [link to the sample](https://htmlpreview.github.io/?https://github.com/Azure-Samples/SpeechToText-WebSockets-Javascript/blob/preview/samples/browser/Sample.html). This will load the page into your default browser (Rendered using [htmlPreview](https://github.com/htmlpreview/htmlpreview.github.com)).

### Open the sample from a local copy

To try the sample locally, clone this repository:

```
git clone https://github.com/Azure-Samples/SpeechToText-WebSockets-Javascript
```

compile the TypeScript sources and bundle/browserfy them into a single JavaScript file ([npm](https://www.npmjs.com/) needs to be installed on your machine). Change into the root of the cloned repository and run the commands:

```
cd SpeechToText-WebSockets-Javascript && npm run bundle
```

Open `samples\browser\Sample.html` in your favorite browser.

## Next steps

### Installation of npm package

An npm package of the Microsoft Speech Javascript Websocket SDK is available. To install the [npm package](https://www.npmjs.com/package/microsoft-speech-browser-sdk) run
```
npm install microsoft-speech-browser-sdk
```

### As a Node module

If you're building a node app and want to use the Speech SDK, all you need to do is add the following import statement:

```javascript
import * as SDK from 'microsoft-speech-browser-sdk';
```

<a name="reco_setup"></a>and setup the recognizer:

```javascript
function RecognizerSetup(SDK, recognitionMode, language, format, subscriptionKey) {
    let recognizerConfig = new SDK.RecognizerConfig(
        new SDK.SpeechConfig(
            new SDK.Context(
                new SDK.OS(navigator.userAgent, "Browser", null),
                new SDK.Device("SpeechSample", "SpeechSample", "1.0.00000"))),
        recognitionMode, // SDK.RecognitionMode.Interactive  (Options - Interactive/Conversation/Dictation)
        language, // Supported languages are specific to each recognition mode Refer to docs.
        format); // SDK.SpeechResultFormat.Simple (Options - Simple/Detailed)

    // Alternatively use SDK.CognitiveTokenAuthentication(fetchCallback, fetchOnExpiryCallback) for token auth
    let authentication = new SDK.CognitiveSubscriptionKeyAuthentication(subscriptionKey);

    return SDK.Recognizer.Create(recognizerConfig, authentication);
}

function RecognizerStart(SDK, recognizer) {
    recognizer.Recognize((event) => {
        /*
            Alternative syntax for typescript devs.
            if (event instanceof SDK.RecognitionTriggeredEvent)
        */
        switch (event.Name) {
            case "RecognitionTriggeredEvent" :
                UpdateStatus("Initializing");
                break;
            case "ListeningStartedEvent" :
                UpdateStatus("Listening");
                break;
            case "RecognitionStartedEvent" :
                UpdateStatus("Listening_Recognizing");
                break;
            case "SpeechStartDetectedEvent" :
                UpdateStatus("Listening_DetectedSpeech_Recognizing");
                console.log(JSON.stringify(event.Result)); // check console for other information in result
                break;
            case "SpeechHypothesisEvent" :
                UpdateRecognizedHypothesis(event.Result.Text);
                console.log(JSON.stringify(event.Result)); // check console for other information in result
                break;
            case "SpeechFragmentEvent" :
                UpdateRecognizedHypothesis(event.Result.Text);
                console.log(JSON.stringify(event.Result)); // check console for other information in result
                break;
            case "SpeechEndDetectedEvent" :
                OnSpeechEndDetected();
                UpdateStatus("Processing_Adding_Final_Touches");
                console.log(JSON.stringify(event.Result)); // check console for other information in result
                break;
            case "SpeechSimplePhraseEvent" :
                UpdateRecognizedPhrase(JSON.stringify(event.Result, null, 3));
                break;
            case "SpeechDetailedPhraseEvent" :
                UpdateRecognizedPhrase(JSON.stringify(event.Result, null, 3));
                break;
            case "RecognitionEndedEvent" :
                OnComplete();
                UpdateStatus("Idle");
                console.log(JSON.stringify(event)); // Debug information
                break;
        }
    })
    .On(() => {
        // The request succeeded. Nothing to do here.
    },
    (error) => {
        console.error(error);
    });
}

function RecognizerStop(SDK, recognizer) {
    // recognizer.AudioSource.Detach(audioNodeId) can be also used here. (audioNodeId is part of ListeningStartedEvent)
    recognizer.AudioSource.TurnOff();
}
```

### In a Browser, using Webpack

Currently, the TypeScript code in this SDK is compiled using the default module system (CommonJS), which means that the compilation produces a number of distinct JS source files. To make the SDK usable in a browser, it first needs to be "browserified" (all the javascript sources need to be glued together). Towards this end, this is what you need to do:

1. Add `require` statement to you web app source file, for instance (take a look at [sample_app.js](samples/browser/sample_app.js)):

    ```javascript
        var SDK = require('<path_to_speech_SDK>/Speech.Browser.Sdk.js');
    ```

2. Setup the recognizer, same as [above](#reco_setup).

3. Run your web-app through the webpack (see "bundle" task in [gulpfile.js](gulpfile.js), to execute it, run `npm run bundle`).

4. Add the generated bundle to your html page:

    ```
    <script src="../../distrib/speech.sdk.bundle.js"></script>
    ```

### In a Browser, as a native ES6 module

...in progress, will be available soon

### Token-based authentication

To use token-based authentication, please launch a local node server, as described [here](https://github.com/Azure-Samples/SpeechToText-WebSockets-Javascript/blob/master/samples/browser/README.md)

## Docs
The SDK is a reference implementation for the speech websocket protocol. Check the [API reference](https://docs.microsoft.com/en-us/azure/cognitive-services/speech/API-reference-rest/bingvoicerecognition#websocket) and [Websocket protocol reference](https://docs.microsoft.com/en-us/azure/cognitive-services/speech/API-reference-rest/websocketprotocol) for more details.

## Browser support
The SDK depends on WebRTC APIs to get access to the microphone and read the audio stream. Most of todays browsers(Edge/Chrome/Firefox) support this. For more details about supported browsers refer to [navigator.getUserMedia#BrowserCompatibility](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/getUserMedia#Browser_compatibility)

**Note:** The SDK currently depends on [navigator.getUserMedia](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/getUserMedia#Browser_compatibility) API. However this API is in process of being dropped as browsers are moving towards newer [MediaDevices.getUserMedia](https://developer.mozilla.org/en-US/docs/Web/API/MediaDevices/getUserMedia) instead. The SDK will add support to the newer API soon.

## Contributing
This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
