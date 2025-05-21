# Scenarios: Create captions with speech to text
In this quickstart, you run a console app to create captions with speech to text.

## Option 1: Run the Sample Easily in Visual Studio Code (Recommended)
This is the easiest way to get started. The Azure AI Speech Toolkit extension automates setup, environment configuration, build, and run.

- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- In extension's panel, click `View Samples` and download this sample from sample gallery.
- From the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), run:
  - `Azure AI Speech Toolkit: Configure Azure Speech Resources` to select a speech resource (one-time setup).
  - `Azure AI Speech Toolkit: Configure and Setup the Sample App` to prepare the project (one-time setup).
  - `Azure AI Speech Toolkit: Build the Sample App` to compile the code.
  - `Azure AI Speech Toolkit: Run the Sample App` to run the sample.
    
    This command will run the sample as a task in terminal. Interactively input local audio file if you would like to use file instead of default microphone as audio input. (You can download this audio file as an example: [Call1_separated_16k_health_insurance.wav](https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/master/scenarios/call-center/sampledata/Call1_separated_16k_health_insurance.wav))

## Option 2: Manual Setup and Run (Advanced)
Follow these steps if you prefer not to use VS Code.

In other IDEs, please refer to [captioning-quickstart](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/captioning-quickstart?tabs=windows%2Cterminal&pivots=programming-language-python) for a detailed guide.

### Prerequisites
- Python 3.0
- Microsoft Azure Cognitive Services Speech SDK
- (Optional) GStreamer for handling compressed audio inputs. Instructions are [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams).

### Usage and arguments

Connection:

* `--key`: Your Speech resource key. Overrides the SPEECH_KEY environment variable. VS Code will set SPEECH_KEY environment variable or use the `--key` option.
* `--region REGION`: Your Speech resource region. Overrides the SPEECH_REGION environment variable. VS Code will set SPEECH_REGION environment variable or use the `--region` option. Examples: `westus`, `northeurope`

Input:

* `--input FILE`: Input audio from file. The default input is the microphone. 
* `--format FORMAT`: Use compressed audio format. Valid only with `--file`. Valid values are `alaw`, `any`, `flac`, `mp3`, `mulaw`, and `ogg_opus`. The default value is `any`. To use a `wav` file, don't specify the format. This option is not available with the JavaScript captioning sample. For compressed audio files such as MP4, install GStreamer and see [How to use compressed input audio](~/articles/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams.md). 

Language:

* `--language LANG`: Specify a language using one of the corresponding [supported locales](~/articles/cognitive-services/speech-service/language-support.md?tabs=stt-tts). This is used when breaking captions into lines. Default value is `en-US`.

Recognition:

* `--offline`: Output offline results. Overrides `--realTime`. Default output mode is offline.
* `--realTime`: Output real-time results. 

Real-time output includes `Recognizing` event results. The default offline output is `Recognized` event results only. These are always written to the console, never to an output file. The `--quiet` option overrides this. For more information, see [Get speech recognition results](~/articles/cognitive-services/speech-service/get-speech-recognition-results.md).

Accuracy options:

* `--phrases PHRASE1;PHRASE2`: You can specify a list of phrases to be recognized, such as `Contoso;Jessie;Rehaan`. For more information, see [Improve recognition with phrase list](~/articles/cognitive-services/speech-service/improve-accuracy-phrase-list.md).

Output:

* `--help`: Show this help and stop
* `--output FILE`: Output captions to the specified `file`. This flag is required.
* `--srt`: Output captions in SRT (SubRip Text) format. The default format is WebVTT (Web Video Text Tracks). For more information about SRT and WebVTT caption file formats, see [Caption output format](~/articles/cognitive-services/speech-service/captioning-concepts.md#caption-output-format).
* `--maxLineLength LENGTH`: Set the maximum number of characters per line for a caption to LENGTH. Minimum is 20. Default is 37 (30 for Chinese).
* `--lines LINES`: Set the number of lines for a caption to LINES. Minimum is 1. Default is 2.
* `--delay MILLISECONDS`: How many MILLISECONDS to delay the display of each caption, to mimic a real-time experience. This option is only applicable when you use the `realTime` flag. Minimum is 0.0. Default is 1000.
* `--remainTime MILLISECONDS`: How many MILLISECONDS a caption should remain on screen if it is not replaced by another. Minimum is 0.0. Default is 1000.
* `--quiet`: Suppress console output, except errors.
* `--profanity OPTION`: Valid values: raw, remove, mask. For more information, see [Profanity filter](~/articles/cognitive-services/speech-service/display-text-format.md#profanity-filter) concepts.
* `--threshold NUMBER`: Set stable partial result threshold. The default value is `3`. This option is only applicable when you use the `realTime` flag. For more information, see [Get partial results](~/articles/cognitive-services/speech-service/captioning-concepts.md#get-partial-results) concepts.