# How to get syllables

1. Follow [How to use speech SDK for pronunciation assessment](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-pronunciation-assessment) to learn the basic usage of Pronunciation Assessment API.

2. Update the Pronunciation Assessment configuration to trigger the syllables. Based on the code from step 1, modify the code of creating `pronunciationAssessmentConfig` to below:

    C#

    ```C#
    var pronunciationAssessmentConfig = PronunciationAssessmentConfig.FromJson("{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\"}");
    ```

    C++

    ```C++
    auto pronunciationAssessmentConfig = PronunciationAssessmentConfig::CreateFromJson("{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\"}");
    ```

    Java

    ```Java
    PronunciationAssessmentConfig pronunciationAssessmentConfig = PronunciationAssessmentConfig.fromJson("{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\"}");
    ```

    Python

    ```Python
    pronunciation_assessment_config = speechsdk.PronunciationAssessmentConfig(json_string="{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\"}")
    ```

    JavaScript

    ```JavaScript
    var pronunciationAssessmentConfig = SpeechSDK.PronunciationAssessmentConfig.fromJSON("{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\"}");
    ```

    Objective-C
    
    ```ObjectiveC
    SPXPronunciationAssessmentConfiguration* pronunciationAssessmentConfig = [[SPXPronunciationAssessmentConfiguration alloc]initWithJson:[@"{\"referenceText\":\"good morning\",\"gradingSystem\":\"HundredMark\",\"granularity\":\"Phoneme\",\"phonemeAlphabet\":\"IPA\"}"]];
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

4. By running the code with above updates, the content of `pronunciationAssessmentResultJson` will be like below. It is in JSON format, and you can parse it with any JSON parsing library. Under each element of `Words` array, there is a `Syllables` section, which contains the syllables for this word. For word `good`, there is only one syllable / ɡʊd /, and for word `morning`, you can see two syllables / mɔr / & / nɪŋ /. Each syllable also has a `AccuracyScore` field under the `PronunciationAssessment` section, meaning the syllable level score, which can tell how accurately the speaker pronounces this syllable.

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
                        "AccuracyScore": 100.0
                    }
                },
                {
                    "Phoneme" : "ʊ",
                    "Offset" : 1800000,
                    "Duration": 500000,
                    "PronunciationAssessment": {
                        "AccuracyScore": 100.0
                    }
                },
                {
                    "Phoneme" : "d",
                    "Offset" : 2400000,
                    "Duration": 800000,
                    "PronunciationAssessment": {
                        "AccuracyScore": 100.0
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
