# Video Translation Assistant

A Semantic Kernel agent that helps users translate videos from one language to another using Azure AI services.

## Overview

This project provides an AI-powered assistant that can help users:
- Translate videos from one language to another
- Upload local videos to Azure Blob Storage for processing
- Process videos from remote URLs
- View, manage and track video translation requests
- Create iterations with WebVTT files for improved subtitles
- Get detailed status of translation operations

The agent leverages Azure OpenAI, Azure Speech Services, and Azure Cognitive Services for Video Translation.

## Getting Started

### Prerequisites

- [Python > 3.11.10](https://www.python.org/downloads/)
- [Git](https://git-scm.com/downloads)
- Azure resources:
  - **Azure subscription**: [Create a free account](https://azure.microsoft.com/free/) if you don't already have one
  - **Azure AI Speech Services**: 
    - Create a [Speech resource](https://portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices) in a [supported region for video translation](https://learn.microsoft.com/azure/ai-services/speech-service/regions#speech-service)
  - **Azure Blob Storage**:
    - [Create a storage account](https://learn.microsoft.com/azure/storage/common/storage-account-create) with a container configured for video uploads
    - To provide video access either
      - Generate a [SAS token](https://learn.microsoft.com/azure/storage/common/storage-sas-overview) to provide as a video URL directly to the agent
      - Tell the agent where your video is located locally (via file path) and the agent will handle uploading the video to the storage account and generating the url
  - **Azure CLI**: [Install](https://learn.microsoft.com/cli/azure/install-azure-cli) and authenticate using `az login`


- Video requirements:
  - .mp4 format video file
  - Less than 5 GB in size
  - Shorter than 4 hours in duration
  - Sample video for testing: [es-ES-TryOutOriginal.mp4](https://speechstudioprodpublicsa.blob.core.windows.net/ttsvoice/VideoTranslation/PublicDoc/SampleData/es-ES-TryOutOriginal.mp4)
  - Verify your [source and target languages are supported](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=video-translation)

### Setting up Azure AI Agent Service

Follow the [official Azure AI Agent Service documentation](https://learn.microsoft.com/azure/ai-services/agents/quickstart?pivots=ai-foundry-portal) quickstart. This guides you through creating a hub and project in the Azure AI Foundry Portal and deploying your model. Once completed, you'll have the necessary credentials to configure your environment variables in the next steps.

### Quickstart

1. Clone the repository:

```bash
git clone https://github.com/Azure-Samples/cognitive-services-speech-sdk.git
cd cognitive-services-speech-sdk/samples/video-translation/python/microsoft_video_translation_client
```

2. Create and activate a virtual environment:

```bash
python -m venv .venv
source .venv/bin/activate  # On Windows, use: .venv\Scripts\activate
```

3. Install the required dependencies:

```bash
pip install -r requirements.txt
```

4. Create a `.env` file based on the `.env.sample` provided:

```bash
cp .env.sample .env
```

5. Update the `.env` file.

## Running the Video Translation Assistant

Start the assistant with:

```bash
python agent.py
```

The assistant will guide you through the process of translating videos, providing helpful prompts for required information. To exit the assistant at any time, simply type `exit` or `quit` in the console.

## Features

### Video Translation

Translate videos from one language to another with the following options:
- Source and target languages
- Voice type (Platform Voice or Personal Voice)
- Speaker count customization
- Subtitle character limit adjustment
- Embedded subtitle options

### File Management

The assistant can work with:
- Local video files (automatically uploads to Azure Blob Storage)
- Remote video URLs (direct processing)
- WebVTT subtitle files for custom iterations

### Translation Management

Manage your translations with commands to:
- List all translations
- Get details about specific translations
- Create new iterations with customized subtitles
- Delete translations

## Usage Examples

### Translating a Video

```
You: I'd like to translate a video from English to Japanese
Assistant: I can help you translate your video from English to Japanese. First, I need to know if you have a local video file on your computer or a URL to a video stored online.

You: I have a local file at C:\Videos\my_presentation.mp4
Assistant: Great! I'll need to upload that file to Azure Blob Storage first so it can be processed.
[Assistant uploads file]
Video uploaded successfully!
Access URL: https://yourstorageaccount.blob.core.windows.net/videos/my_presentation.mp4
Secure access URL (valid for 24 hours): https://yourstorageaccount.blob.core.windows.net/videos/my_presentation.mp4?sv=...

Now I need a few more details:
1. Would you like to use "PlatformVoice" or "PersonalVoice" for the translation? PlatformVoice is the standard option.
[...]
```

### Creating an Iteration with Custom Subtitles

```
You: I want to improve the subtitles for my translation
Assistant: I can help you create a new iteration with custom subtitles. Do you have the Translation ID for the video?

You: Yes, it's 06202024123045_en-US_ja-JP_PlatformVoice
Assistant: Great! Now I need the URL to your WebVTT subtitle file and the type of WebVTT file you're providing.
[...]
```

## Architecture

The Video Translation Assistant uses:
- **Semantic Kernel**: For creating the AI assistant and managing plugins
- **Azure AI Agents**: For conversation management and orchestration
- **Azure Video Translation API**: For the core translation functionality
- **Azure Blob Storage**: For secure file storage and retrieval

## Troubleshooting

Common issues and solutions:

- **Authentication Errors**: Ensure you're logged in with `az login` and have the proper permissions.
- **File Upload Issues**: Check your storage account name and permissions.
- **Translation Failures**: Verify your video format is supported and your Cognitive Services endpoint is correct.

### Logging Configuration

By default, logging is set to the ERROR level. If you need more detailed output for debugging or monitoring, you can adjust the logging level:

```bash
# Options include: DEBUG, INFO, WARNING, ERROR, CRITICAL
export LOG_LEVEL=DEBUG  # On Windows, use: set LOG_LEVEL=DEBUG
```

You can also modify the logging configuration directly in the code by updating the `logging.basicConfig()` call in `agent.py`:

```python
# Example: Change to INFO level logging with more detailed format
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(name)s: %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)
```

## Resources
- [Semantic Kernel Official Documentation](https://learn.microsoft.com/semantic-kernel/overview/)
- [Azure AI Agent Service Documentation](https://learn.microsoft.com/azure/ai-services/agents/)
- [Azure AI Speech Service: Video Translation Documentation](https://learn.microsoft.com/azure/ai-services/speech-service/video-translation-overview)
- [Azure AI Foundry Documentation](https://learn.microsoft.com/azure/ai-foundry/)
- [Azure AI Speech Service Documentation](https://learn.microsoft.com/azure/ai-services/speech-service/)
- [Azure Blob Storage Documentation](https://learn.microsoft.com/azure/storage/blobs/)
- [Azure AI Services Overview](https://learn.microsoft.com/azure/ai-services/)