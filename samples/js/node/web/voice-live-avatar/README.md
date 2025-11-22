# Instructions to run Microsoft Azure Voice Live with Avatar sample code
This sample demonstrates the usage of Azure Voice Live API with avatar.

### Prerequisites
- Docker installed on your machine. You can download and install Docker from [here](https://www.docker.com/get-started).
- An active Azure account. If you don't have an Azure account, you can create a account [here](https://azure.microsoft.com/free/ai-services).
- A Microsoft Foundry resource created in one of the supported regions. For more information about region availability, see the [voice live overview documentation](https://learn.microsoft.com/azure/ai-services/speech-service/voice-live).

### Avatar available locations
The avatar feature is currently available in the following service regions: Southeast Asia, North Europe, West Europe, Sweden Central, South Central US, East US 2, and West US 2.

### Build the sample
To run the sample, you need to build it into a Docker image: navigate to the folder containing this README.md document and run the following command:
  ```bash
  docker build -t voice-live-avatar .
  ```

This command will create a Docker image named `voice-live-avatar`.

### Start the sample
To start the sample, use the following command:
  ```bash
  docker run --rm -p 3000:3000 voice-live-avatar
  ```

Then open your web browser and navigate to `http://localhost:3000` to access the sample.

### Configure and play the sample

* Step 1: Under the `Connection Settings` section, fill `Azure AI Services Endpoint` and `Subscription Key`, which can be obtained from the `Keys and Endpoint` tab in your Azure AI Services resource. The endpoint can be the regional endpoint (e.g., `https://<region>.api.cognitive.microsoft.com/`) or a custom domain endpoint (e.g., `https://<custom-domain>.cognitiveservices.azure.com/`).

* Step 2: Under `Conversation Settings` section, toggle the `Avatar` switch to enable avatar feature. Then select an avatar from the `Avatar` dropdown list. Toggle `Use Photo Avatar` switch if you want to use photo avatar. Then choose a prebuilt avatar character from the dropdown list below. If you want to use a custom avatar, toggle the `Use Custom Avatar` switch and fill the character name in the `Character` field below.

* Step 3: Click `Connect` button to start the conversation. Once connected, you should see the avatar appearing on the page, and you can click `Turn on microphone` and start talking with the avatar with speech.

* Step 4: On top of the page, you can toggle the `Developer mode` switch to enable developer mode, which will show chat history in text and additional logs useful for debugging.

### Deployment

This sample can be deployed to cloud for global access. The recommended hosting platform is [Azure Container Apps](https://learn.microsoft.com/azure/container-apps/overview). Here are the steps to deploy this sample to `Azure Container Apps`:

* Step 1: Push the Docker image to a container registry, such as [Azure Container Registry](https://learn.microsoft.com/azure/container-registry/). You can use the following command to push the image to Azure Container Registry:
  ```bash
  docker tag voice-live-avatar <your-registry-name>.azurecr.io/voice-live-avatar:latest
  docker push <your-registry-name>.azurecr.io/voice-live-avatar:latest
  ```

* Step 2: Create an `Azure Container App` and deploy the Docker image built from above steps, following [Deploy from an existing container image](https://learn.microsoft.com/azure/container-apps/quickstart-portal). Make sure to set the environment variables as described in the previous sections, such as `SPEECH_REGION`, `SPEECH_KEY`, etc.

* Step 3: Once the `Azure Container App` is created, you can access the sample by navigating to the URL of the `Azure Container App` in your browser.
