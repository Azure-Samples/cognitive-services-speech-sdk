# Ingestion Client

<!-- 
Guidelines on README format: https://review.docs.microsoft.com/help/onboard/admin/samples/concepts/readme-template?branch=master

Guidance on onboarding samples to docs.microsoft.com/samples: https://review.docs.microsoft.com/help/onboard/admin/samples/process/onboarding?branch=master

Taxonomies for products and languages: https://review.docs.microsoft.com/new-hope/information-architecture/metadata/taxonomies?branch=master
-->

Ingestion Client is a solution which accelerates transcription capability by automatically transcribing audio files uploaded to an Azure Storage Container.

See the Contribution guide for how to contribute to this repo.

## Contents

Outline the file contents of the repository. It helps users navigate the codebase, build configuration and any related assets.

| Folder       | Description                                |
|-------------------|--------------------------------------------|
| `Connector`       | Shared code between Azure Functions.                       |
| `FetchTranscription`    | Function to check the state of the transcription job and write results to storage.             |
| `StartTranscriptionByServiceBus`      | Function to add a new transcription job to the speech service as one-job-per-file.      |
| `StartTranscriptionByTimer`      | Function to add a new transcription job to the speech service for all files added in a certain period of time.      |
| `RealTimeTranscription`      | Function to stream an audio file to the speech service and compose the transcript results in a single json file.      |
| `PowerBI`    | Template files and deployment guide for visualizing insights from the accelerator.             |
| `Setup` | ARM template to setup the project on Azure. |
| `Tests`       | Unit tests for both functions.                          |

## Prerequisites

* An existing [Azure Account](https://azure.microsoft.com/free/)
* An existing [Speech Services Subscription](https://ms.portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices)

## Getting started

Follow the instructions for setting up Ingestion Client [here](Setup/guide.md).

To set up Power BI for further insights of the transcription, follow the guide [here](PowerBI/README.md).

## Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
