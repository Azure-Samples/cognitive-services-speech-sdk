# How to use the Speech Services Batch Synthesis API from Python

## Run the Sample within VS Code
1. Install "Azure AI Speech Toolkit" extension in VS Code.
2. Download this sample from sample gallery to local machine.
3. Trigger "Azure AI Speech Toolkit: Configure Azure Speech Resources" command from command palette to select speech resource.
4. Trigger "Azure AI Speech Toolkit: Configure and Setup the Sample App" command from command palette to configure and setup the sample. This command only needs to be run once.
5. Trigger "Azure AI Speech Toolkit: Build the Sample App" command from command palette to build the sample.
6. Trigger "Azure AI Speech Toolkit: Run the Sample App" command from command palette to run the sample.


## Prerequisites
- Python 3.8 or higher
- (Optional:) The relationship between custom voice names and deployment ID, if you want to use custom voices.
- (Optional:) The URI of a writable Azure blob container, if you want to store the audio files in your own Azure storage.

## Some notes:
- We recommend using a passwordless authentication provided by the `azure-identity` library. Your Microsoft Entra user account is need to be assigned with `Cognitive Services User` or `Cognitive Services Speech User` role.
- Alternatively, you can get the subscription key from the "Keys and Endpoint" tab on your Azure AI Speech resource in the Azure Portal.
- Batch synthesis is only available for paid subscriptions, free subscriptions are not supported.
- Please refer to [this page](https://learn.microsoft.com/azure/ai-services/speech-service/regions#rest-apis) for a complete list of region identifiers in the expected format.

## Run the Sample without VS Code
1. **Azure Subscription**: You need an Azure account with an active subscription.
2. **Azure Speech Service**: Create an Azure Speech resource in the Azure portal.
3. **Python Environment**: Ensure you have Python 3.x installed.
4. **Required Dependencies**: Install the necessary Python packages using:

   ```sh
   pip install azure-identity requests
   ```

5. **Authentication Setup**:
   - Set up environment variables for authentication:
     ```sh
     export SPEECH_ENDPOINT="your_speech_endpoint"
     export SPEECH_REGION="your_speech_region"
     export SPEECH_KEY="your_speech_key"
     ```

### 1. Running the Script

Execute the script using:
```sh
python synthesis.py
```