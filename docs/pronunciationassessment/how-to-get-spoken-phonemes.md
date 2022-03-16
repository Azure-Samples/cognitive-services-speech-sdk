# How to get spoken phonemes

1. Follow [How to use speech SDK for pronunciation assessment](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-pronunciation-assessment) to learn the basic usage of Pronunciation Assessment API.

2. Update the Pronunciation Assessment configuration to trigger the IPA format. Based on the code from step 1, modify the code of creating "pronunciationAssessmentConfig" to below:

    C#

    ```C#
    var pronunciationAssessmentConfig = PronunciationAssessmentConfig.FromJson("{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\",\"nBestPhonemeCount\":5}");
    ```

    C++

    ```C++
    auto pronunciationAssessmentConfig = PronunciationAssessmentConfig::CreateFromJson("{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\",\"nBestPhonemeCount\":5}");
    ```

    Java

    ```Java
    PronunciationAssessmentConfig pronunciationAssessmentConfig = PronunciationAssessmentConfig.fromJson("{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\",\"nBestPhonemeCount\":5}");
    ```

    Python

    ```Python
    pronunciation_assessment_config = speechsdk.PronunciationAssessmentConfig(json_string="{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\",\"nBestPhonemeCount\":5}")
    ```

    JavaScript

    ```JavaScript
    var pronunciationAssessmentConfig = SpeechSDK.PronunciationAssessmentConfig.fromJSON("{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\",\"nBestPhonemeCount\":5}");
    ```

    Objective-C
    
    ```ObjectiveC
    SPXPronunciationAssessmentConfiguration* pronunciationAssessmentConfig = [[SPXPronunciationAssessmentConfiguration alloc]initWithJson:[@"{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\",\"nBestPhonemeCount\":5}"]];
    ```

3. Receive the Pronunciation Assessment result in JSON. Based on the code from step1, insert a code line to receive the Pronunciation Assessment result in JSON, as below:

    C#

    ```C#
    var pronunciationAssessmentResultJson = speechRecognitionResult.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
    ```

    C++

    ```C++
    auto pronunciationAssessmentResultJson = speechRecognitionResult ->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
    ```

    Java

    ```Java
    String pronunciationAssessmentResultJson = result.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult);
    ```

    Python

    ```Python
    pronunciation_assessment_result_json = result.properties.get(speechsdk.PropertyId.SpeechServiceResponse_JsonResult)
    ```

    JavaScript

    ```JavaScript
    var pronunciationAssessmentResultJson = result.properties.getProperty(SpeechSDK.PropertyId.SpeechServiceResponse_JsonResult);
    ```

    Objective-C
    
    ```ObjectiveC
    NSString* pronunciationAssessmentResultJson = [result.properties getPropertyByName:@"RESULT-Json"];
    ```

4. By running the code with above updates, the content of `pronunciationAssessmentResultJson` will be like below. It is in JSON format, and you can parse it with any JSON parsing library. Under each element of `Phonemes` array, there is also a `PronunciationAssessment` section. This section has a `Phoneme` field and a `NBestPhonemes` section. The `Phoneme` field indicates the expected phoneme, and the `NBestPhonemes` section indicates the spoken phonemes. There are multiple spoken phonemes for each expected phoneme, ranking with the probability.
Taking the first phoneme of word `good` for example, the expected phoneme is / ɡ /, and it has `NBestPhonemes`: / ɡ /, / k /, ... (There should be 5 `NBestPhonemes` if you specified "nBestPhonemeCount":5 in step2 above.)
The phoneme / ɡ / was put at the first position in `NBestPhonemes` section, and it has highest score. This means that the speaker's pronunciation is mostly closed to / ɡ /.
The phoneme / k / was put at the second position in `NBestPhonemes` section, and it has second high score. This means that the speaker's pronunciation is secondly closed to / k /.
In this example the speaker did a good job because his/her top spoken phoneme / ɡ / was consistent with expected phoneme / ɡ /.
There could also be case with `NBestPhonemes`: / k /, / ɡ /, …, which means the speaker's pronunciation was mostly closed to / k /, and inconsistent with the expected phoneme / ɡ /. It indicates that the speaker pronounced the word `good` more like `kood`. In this case you can tell the speaker: "sounds like you said / kʊd /".


    ```json
    {
        "Format": "Detailed",
        "RecognitionStatus": "Success",
        "DisplayText": "Good morning.",
        "Offset": 400000,
        "Duration": 11000000,
        "SNR": 37.3953,
        "NBest": [
        {
            "Confidence": "0.87",
            "Lexical": "good morning",
            "ITN" : "good morning",
            "MaskedITN" : "good morning",
            "Display" : "Good morning.",
            "PronunciationAssessment" : {
                "PronScore" : 84.4,
                "AccuracyScore" : 100.0,
                "FluencyScore" : 74.0,
                "CompletenessScore" : 100.0,
            },
            "Words": [
            {
                "Word" : "good",
                "Offset" : 500000,
                "Duration" : 2700000,
                "PronunciationAssessment": {
                    "AccuracyScore" : 100.0,
                    "ErrorType" : "None"
                },
                "Syllables" : [
                {
                    "Syllable" : "ɡʊd",
                    "Offset" : 500000,
                    "Duration" : 2700000,
                    "PronunciationAssessment" : {
                        "AccuracyScore": 100.0
                    }
                }],
                "Phonemes": [
                {
                    "Phoneme" : "ɡ",
                    "Offset" : 500000,
                    "Duration": 1200000,
                    "PronunciationAssessment": {
                        "AccuracyScore": 100.0,
                        "NBestPhonemes": [
                        {
                            "Phoneme": "ɡ",
                            "Score": 100.0
                        },
                        {
                            "Phoneme": "k",
                            "Score": 5.0
                        },
                        ... // omitted n best phonemes
                        ]
                    }
                },
                {
                    "Phoneme" : "ʊ",
                    "Offset" : 1800000,
                    "Duration": 500000,
                    "PronunciationAssessment": {
                        "AccuracyScore": 100.0
                        "NBestPhonemes": [
                            ... // omitted n best phonemes
                        ]
                    }
                },
                {
                    "Phoneme" : "d",
                    "Offset" : 2400000,
                    "Duration": 800000,
                    "PronunciationAssessment": {
                        "AccuracyScore": 100.0
                        "NBestPhonemes": [
                            ... // omitted n best phonemes
                        ]
                    }
                }]
            },
            {
                "Word" : "morning",
                "Offset" : 3300000,
                "Duration" : 5500000,
                "PronunciationAssessment": {
                    "AccuracyScore" : 100.0,
                    "ErrorType" : "None"
                },
                "Syllables": [
                {
                    "Syllable" : "mɔr",
                    "Offset" : 3300000,
                    "Duration": 2300000,
                    "PronunciationAssessment": {
                        "AccuracyScore": 100.0
                    }
                },
                {
                    "Syllable" : "nɪŋ",
                    "Offset" : 5700000,
                    "Duration": 3100000,
                    "PronunciationAssessment": {
                        "AccuracyScore": 100.0
                    }
                }],
                "Phonemes": [
                    ... // omitted phonemes
                ]
            }]
        }]
    }
    ```
