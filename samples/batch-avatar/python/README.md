# How to use the Speech Services Batch Avatar Synthesis API from Python

## Install dependencies

The sample uses the `requests` library. You can install it with the command

```sh
pip install requests
```

## Run the sample code

The sample code itself is [synthesis.py](synthesis.py) and can be run using Python 3.8 or higher.
You will need to adapt the following information to run the sample:

1. Your Cognitive Services subscription key and region.

    Some notes:

    - You can get the subscription key from the "Keys and Endpoint" tab on your Cognitive Services or Speech resource in the Azure Portal.
    - Batch avatar synthesis is only available for paid subscriptions, free subscriptions are not supported.
    - Batch avatar synthesis is only available in these service regions: `West US 2`, `West Europe` and `South East Asia`

1. (Optional:) The relationship between custom voice names and deployment ID, if you want to use custom voices.
2. (Optional:) The URI of a writable Azure blob container, if you want to store the audio files in your own Azure storage.

You can use a development environment like PyCharm or VS Code to edit, debug, and execute the sample.
