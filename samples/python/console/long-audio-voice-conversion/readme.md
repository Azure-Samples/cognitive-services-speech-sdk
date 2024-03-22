# Voice conversion(VC) for long audio using Speech SDK

This demo shows that how to convert long audio into another voice using Speech SDK.

## Prerequisites

Refer to [this readme](../README.md) for prerequisites of speech SDK installation.

### Python packages

This sample needs these python packages:

- `pydub`: split the input audio.
- `librosa`: load and resample the input audio.
- `soundfile`: save the output resampled audio.
- `tqdm`: show a smart progress meter when convert multi audios.
- `azure-storage-blob`: to access azure blob.

Install them using the following command:

``` sh
python3 -m pip install pydub librosa soundfile tqdm azure-storage-blob
```

### Azure Blob Storage

When calling the voice conversion(VC) service, the location of the source audio is specified by using `<mstts:voiceconversion url='XXX'/>` in SSML. The source `url` should be an https URL ([Azure Blob Storage](https://learn.microsoft.com/en-us/azure/storage/blobs/storage-blobs-introduction) can host your audio files on an https URL).

This sample uses `Azure Blob Storage` to temporarily store the source audio, please refer to [Create a container](https://learn.microsoft.com/en-us/azure/storage/blobs/storage-quickstart-blobs-portal) to create a new container.

**Note**: you need to configure anonymous public read access for the **container**, otherwise the speech service cannot read the source audio in the Azure container. See [Configure anonymous public read access for containers and blobs](https://learn.microsoft.com/en-us/azure/storage/blobs/anonymous-read-access-configure?view=form-recog-3.0.0&tabs=portal#set-the-public-access-level-for-a-container).

## Steps to convert long audio into another voice

The basic steps to convert long audio into another voice shown in this sample are

1. `Split audio`: first convert input long audio to 16 kHz, 16 bit, single channel WAV, then use ASR service to get the segment info and finally split the long audio into multiple continuous short audios(voiced segments and silent segments).
2. `Call VC`: call VC service for each voiced segment in parallel. For each audio, first upload to azure blob, then call VC service for conversion, and finally delete the audio in azure blob.
3. `Merge audio`: merge silent segments and converted voiced segments back into a single audio file. Before splicing audio, adjust the length of the converted voiced segments according to the length of the corresponding source voiced segment(truncate or splice silence behind)

## Run the sample

To run the app, navigate to the `samples/python/console/long-audio-voice-conversion` directory in your local copy of the samples repository.

Update the following strings before running the sample code(`long_audio_voice_changer.py`):

- `YourSubscriptionKey`: replace with your subscription key.
- `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with.
- `YourContainerURLWithSAS`: replace with your container URL with SAS token, refer to [Create SAS tokens for storage containers](https://learn.microsoft.com/en-us/azure/applied-ai-services/form-recognizer/create-sas-tokens). **Note**: the SAS token must have at least *read*, *write* and *delete* permissions.

You may want to update other configuration such as `language`(e.g, "zh-CN") and `name`(e.g, "zh-CN-XiaoxiaoNeural").

Start the app with the following command:

``` sh
python3 long_audio_voice_changer.py
```

Depending on your platform, the Python 3 executable might also just be called `python`.

## Note

The voice conversion service is available for *zh-CN-XiaoxiaoNeural* in all region, *zh-CN-YunxiNeural* on southeastasia region and *en-US-GuyNeural* on westus2 region currently.
