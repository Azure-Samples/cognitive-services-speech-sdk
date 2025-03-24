# Chatting with Azure Pronunciation Assessment and Azure OAI using Speech SDK

This demo shows that how to do chatting with Azure Pronunciation Assessment and Azure OAI using Speech SDK.

## Prerequisites

Refer to [this readme](../README.md) for prerequisites of speech SDK installation.

This sample uses `requests`, `numpy` and `soundfile` packages. Install them using the following command:

```sh
pip install requests numpy soundfile
```

## Run the sample

Update the following strings before running the sample:

  * `YourSubscriptionKey`: replace with your subscription key.
  * `https://YourServiceRegion.api.cognitive.microsoft.com`: replace with the endpoint for your Speech resource. You can find this endpoint in the Azure Portal under your Speech resource's "Keys and Endpoint" section.
* `YourOaiResourceName`: replace with the name of your Azure OAI Resource.
* `YourOaiDeploymentName`: replace with the name of your model deployment. You're required to first deploy a model before you can make calls.
* `YourOaiApiVersion`: replace with the [API version](https://learn.microsoft.com/azure/ai-services/openai/reference#chat-completions) to use for this operation. For example, `2023-05-15`.
* `YourOaiApiKey`: replace with the Azure OAI resource key.

To run the app, navigate to the `samples/python/console/chatting` directory in your local copy of the samples repository.
Start the app with the command

```sh
python3 chatting_sample.py
```

Depending on your platform, the Python 3 executable might also just be called `python`.