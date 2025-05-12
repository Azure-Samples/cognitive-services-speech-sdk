# Synthesize long-form text using Speech SDK

This demo shows that how to synthesize long-form text using Speech SDK.

## Prerequisites

Refer to [this readme](../README.md) for prerequisites of speech SDK installation.

This sample uses `nltk` package to tokenize the input text. Install it using the following command:

```sh
pip install nltk
```

## Steps to synthesize long-form text

The basic steps to synthesize long-form text shown in this sample are

- Split the long-form text to paragraphs or sentences as the speech synthesis service has a limitation on the length of input text and output audio. This sample uses `nltk` package to tokenize the input text; or split SSML based on `voice` tag.
- Synthesize each paragraph or sentence using Speech SDK, better in parallel to save time. The sample uses `multiprocessing` module to parallelize the synthesis and use a `SynthesizerPool` to reuse the synthesizer instances instead of creating new ones each time.
- Merge the synthesized audio files into a single audio file, as well as the word and sentence boundaries. The audio offset of each sentence is started from 0, then we need to accumulate the offset of each sentence to get the final offset.

## Run the sample

Update the following strings before running the sample:

  * `YourSubscriptionKey`: replace with your subscription key.
  * `https://YourServiceRegion.api.cognitive.microsoft.com`: replace with the endpoint for your Speech resource. You can find this endpoint in the Azure Portal under your Speech resource's "Keys and Endpoint" section.

You may want to update other configuration such as `VoiceName` and `OutputFormat`.

To run the app, navigate to the `samples/python/console/long-form-text-synthesis` directory in your local copy of the samples repository.
Start the app with the command

```sh
python3 long_form_text_synthesis.py.py
```

Depending on your platform, the Python 3 executable might also just be called `python`.

## Note

The sample text [*The Great Gatsby* Chapter 1](./Gatsby-chapter1.txt) is an open-domain book and downloaded from [gutenberg](https://www.gutenberg.org/ebooks/64317)


