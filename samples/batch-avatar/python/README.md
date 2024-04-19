# How to use the Speech Services Batch Avatar Synthesis API from Python

## Install dependencies

The sample uses the `requests` library. You can install it with the command

```sh
pip install requests
```

We recommend using a passwordless authentication provided by the `azure-identity` library.
You can install it with the command

```sh
pip install azure-identity
```

## Run the sample code

The sample code itself is [synthesis.py](synthesis.py) and can be run using Python 3.8 or higher.
You will need to adapt the following information to run the sample:

1. Your Azure AI Speech Services.

    Some notes:

    - We recommend using a passwordless authentication provided by the `azure-identity` library. Your Microsoft Entra user account is need to be assigned with `Cognitive Services User` or `Cognitive Services Speech User` role.
      - Alternatively, you can get the subscription key from the "Keys and Endpoint" tab on your Azure AI Speech resource in the Azure Portal.
    - Batch avatar synthesis is only available for paid subscriptions, free subscriptions are not supported.
    - Batch avatar synthesis is only available in these service regions: `West US 2`, `West Europe` and `South East Asia`

2. (Optional:) The relationship between custom voice names and deployment ID, if you want to use custom voices.
3. (Optional:) The URI of a writable Azure blob container, if you want to store the synthesized files in your own Azure storage.

You can use a development environment like PyCharm or VS Code to edit, debug, and execute the sample.
