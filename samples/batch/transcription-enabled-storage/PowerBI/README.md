# Power BI Reports

<!-- 
Guidelines on README format: https://review.docs.microsoft.com/help/onboard/admin/samples/concepts/readme-template?branch=master

Guidance on onboarding samples to docs.microsoft.com/samples: https://review.docs.microsoft.com/help/onboard/admin/samples/process/onboarding?branch=master

Taxonomies for products and languages: https://review.docs.microsoft.com/new-hope/information-architecture/metadata/taxonomies?branch=master
-->

This project contains Power BI templates that use the SQL outputs from the Transcription Enabled Storage Solution Accelerator.

See the Contribution guide for how to contribute to this repo.

## Contents

There are template reports provided for two use cases that help to realize value of the speech solution accelerator. Below are the file names and a brief description of each template.

| File       | Description                                |
|-------------------|--------------------------------------------|
| `SpeechInsights.pbit`       | A collection of reports that use data from Transcription Enabled Storage to quantify your customer care agents' interactions and improve empathetic conversations.                       |
| `SentimentInsights.pbit`      | A collection of reports that use data from Transcription Enabled Storage to understand the sentiment of your customers during an interaction and improve satisfaction.      |


## Prerequisites

* An existing [Azure Account](https://azure.microsoft.com/en-us/free/)
* An existing [Speech Services Subscription](https://ms.portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices)
* An existing [Text Analytics Subscription](https://ms.portal.azure.com/#create/Microsoft.CognitiveServicesTextAnalytics) (required for SentimentInsights.pbit only)
* Successfully deployed [Transcription Enabled Storage Solution Accelerator](https://github.com/Azure/SpeechAccelerators/pull/TranscriptionEnabledStorage/Setup/guide.md)

## Getting started

Follow the instructions [here](guide.md).

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
