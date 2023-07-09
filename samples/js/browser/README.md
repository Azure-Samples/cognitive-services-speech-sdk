# JavaScript Speech Recognition, Synthesis, and Translation Sample for the Web Browser

This sample demonstrates how to recognize, synthesize, and translate speech with the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome.
See [this article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-browser) for introductory information on the Speech SDK for JavaScript.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* For intent recognition: an *endpoint* subscription key for the [Language Understanding Intelligent Service (LUIS)](https://www.luis.ai/home), and an application ID.
* A PC or Mac, with a working speaker.
* A text editor.
* Ensure you have [Node.js](https://nodejs.org/en/download/) installed.

## Build the sample

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://docs.microsoft.com/azure/cognitive-services/speech-service/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)

If you want to host the sample on a web server, the web server must be able to run Node. Follow these steps:

* Edit the `.env` source:
  * Replace the string `paste-your-speech-region-here` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
  * Replace the string `paste-your-speech-key-here` with your own subscription key.
* Edit the `index.html` source:
  * Replace the value for the variable `authorizationEndpoint` with the full URL where you can access the Node server. By default, this is http://localhost:3001/api/get-speech-token
* For synthesis, edit the `synthesis.html` source:
  * Replace the value for the variable `authorizationEndpoint` with the full URL where you can access the Node server. By default, this is http://localhost:3001/api/get-speech-token
* Deploy all files to your web server.

## How to run the sample

* Clone this repo, then change directory to the project root and run `npm install` to install dependencies.
* Add your Azure Speech key and region to the `.env` file, replacing the placeholder text.
* To run the Express server, run `npm run server`.

* In case you are running the sample from your local computer, open `public/index.html` or `public/synthesis.html` from the location where you have downloaded this sample with a JavaScript capable browser.
  Use the input fields to set your subscription key and service region.
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.


## Token exchange process

This sample application shows an example design pattern for retrieving and managing tokens, a common task when using the Speech JavaScript SDK in a browser environment. A simple Express back-end is implemented in the same project under `server/index.js`, which abstracts the token retrieval process. 

The reason for this design is to prevent your speech key from being exposed on the front-end, since it can be used to make calls directly to your subscription. By using an ephemeral token, you are able to protect your speech key from being used directly. To get a token, you use the Speech REST API and make a call using your speech key and region. In the Express part of the app, this is implemented in `index.js` behind the endpoint `/api/get-speech-token`, which the front-end uses to get tokens. 

```javascript
app.get('/api/get-speech-token', async (req, res, next) => {
    res.setHeader('Content-Type', 'application/json');
    const speechKey = process.env.SPEECH_KEY;
    const speechRegion = process.env.SPEECH_REGION;

    if (speechKey === 'paste-your-speech-key-here' || speechRegion === 'paste-your-speech-region-here') {
        res.status(400).send('You forgot to add your speech key or region to the .env file.');
    } else {
        const headers = { 
            headers: {
                'Ocp-Apim-Subscription-Key': speechKey,
                'Content-Type': 'application/x-www-form-urlencoded'
            }
        };

        try {
            const tokenResponse = await axios.post(`https://${speechRegion}.api.cognitive.microsoft.com/sts/v1.0/issueToken`, null, headers);
            res.send({ token: tokenResponse.data, region: speechRegion });
        } catch (err) {
            res.status(401).send('There was an error authorizing your speech key.');
        }
    }
});
```

In the request, you create a `Ocp-Apim-Subscription-Key` header, and pass your speech key as the value. Then you make a request to the **issueToken** endpoint for your region, and an authorization token is returned. In a production application, this endpoint returning the token should be *restricted by additional user authentication* whenever possible. 

On the front-end, both `public/index.html` and `public/synthesis.html` contain code (using the axios library) to retrieve the token from the server. 

```javascript
let authorizationEndpoint = "http://localhost:3001/api/get-speech-token";

async function RequestAuthorizationToken() {
  if (authorizationEndpoint) {
    try {
      const res = await axios.get(authorizationEndpoint);
      const token = res.data.token;
      const region = res.data.region;
      regionOptions.value = region;
      authorizationToken = token;

      console.log('Token fetched from back-end: ' + token);
    } catch (err) {
        console.log(err);
    }
  }
}
```

This code makes the call to `/api/get-speech-token` to fetch an new authorization. 

```javascript
let speechConfig;
if (authorizationToken) {
    speechConfig = sdkConfigType.fromAuthorizationToken(authorizationToken, regionOptions.value);
} 
```

In many other Speech service samples, you will see the function `SpeechConfig.fromSubscription` used instead of `SpeechConfig.fromAuthorizationToken`, but by **avoiding the usage** of `fromSubscription` on the front-end, you prevent your speech subscription key from becoming exposed, and instead utilize the token authentication process. `fromSubscription` is safe to use in a Node.js environment, or in other Speech SDK programming languages when the call is made on a back-end, but it is best to avoid using in a browser-based JavaScript environment.

## References

* [JavaScript quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-browser)
* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
