// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
// Please install the required packages from https://github.com/microsoft/cognitive-services-speech-sdk-js
import * as sdk from 'microsoft-cognitiveservices-speech-sdk';
import fs from 'fs';
import _ from 'lodash';

// content assessment with audio file
export const main = (settings) => {
    var audioConfig = sdk.AudioConfig.fromWavFileInput(fs.readFileSync(settings.filename));
    var speechConfig = sdk.SpeechConfig.fromSubscription(settings.subscriptionKey, settings.serviceRegion);

    var url = `https://${settings.aoaiResourceName}.openai.azure.com/openai/deployments/${settings.aoaiDeploymentName}/` +
        `chat/completions?api-version=${settings.aoaiApiVersion}`;
    var headers = {"Content-Type": "application/json", "api-key": settings.aoaiApiKey};

    // setting the recognition language to English.
    speechConfig.speechRecognitionLanguage = "en-US";

    const pronunciationAssessmentConfig = new sdk.PronunciationAssessmentConfig(
        "",
        sdk.PronunciationAssessmentGradingSystem.HundredMark,
        sdk.PronunciationAssessmentGranularity.Phoneme,
        false
    );
    pronunciationAssessmentConfig.enableProsodyAssessment = true;

    // create the speech recognizer.
    var reco = new sdk.SpeechRecognizer(speechConfig, audioConfig);
    // (Optional) get the session ID
    reco.sessionStarted = (_s, e) => {
        console.log(`SESSION ID: ${e.sessionId}`);
    };
    pronunciationAssessmentConfig.applyTo(reco);

    const recognizedTexts = [];

    reco.recognized = function (s, e) {
        var jo = JSON.parse(e.result.properties.getProperty(sdk.PropertyId.SpeechServiceResponse_JsonResult));
        console.log(`Recognizing: ${jo.DisplayText}`);
        recognizedTexts.push(jo.DisplayText);
    }

    async function getContentScores(recognizedText, topic) {
        const sampleSentence1 = "OK the movie i like to talk about is the cove it is very say phenomenal sensational documentary about adopting hunting practices in japan i think the director is called well i think the name escapes me anyway but well let's talk about the movie basically it's about dolphin hunting practices in japan there's a small village where where villagers fisherman Q almost twenty thousand dolphins on a yearly basis which is brutal and just explain massacre this book has influenced me a lot i still remember the first time i saw this movie i think it was in middle school one of my teachers showed it to all the class or the class and i remember we were going through some really boring topics like animal protection at that time it was really boring to me but right before everyone was going to just sleep in the class the teacher decided to put the textbook down and show us a clear from this document documentary we were shocked speechless to see the has of the dolphins chopped off and left on the beach and the C turning bloody red with their blood which is i felt sick i couldn't need fish for a whole week and it was lasting impression if not scarring impression and i think this movie is still very meaningful and it despite me a lot especially on wildlife protection dolphins or search beautiful intelligent animals of the sea and why do villagers and fishermen in japan killed it i assume there was a great benefit to its skin or some scientific research but the ironic thing is that they only kill them for the meat because the meat taste great that sickens me for awhile and i think the book inspired me to do a lot of different to do a lot of things about well i protection i follow news like";
        const sampleSentence2 = "yes i can speak how to this movie is it is worth young wolf young man this is this movie from korea it's a crime movies the movies on the movies speaker speaker or words of young man love hello a cow are you saying they end so i have to go to the go to the america or ha ha ha lots of years a go on the woman the woman is very old he talk to korea he carpool i want to go to the this way this whole home house this house is a is hey so what's your man and at the end the girl cause so there's a woman open open hum finally finds other wolf so what's your young man so the young man don't so yeah man the young man remember he said here's a woman also so am i it's very it's very very sad she is she is a crack credit thank you ";
        const sampleSentence3 = "yes i want i want to talk about the TV series are enjoying watching a discount name is a friends and it's uh accommodate in the third decades decades an it come out the third decades and its main characters about a six friends live in the NYC but i watched it a long time ago i can't remember the name of them and the story is about what they are happening in their in their life and there are many things treating them and how the friendship are hard friendship and how the french how the strong strongly friendship they obtain them and they always have some funny things happen they only have happened something funny things and is a comedy so that was uh so many and i like this be cause of first adult cause it has a funding it has a farming serious and it can improve my english english words and on the other hand it can i can know about a lot of cultures about the united states and i i first hear about death TV series it's come out of a website and i took into and i watch it after my after my finish my studies and when i was a bad mood when i when i'm in a bad mood or i ";
        const data = {
            "messages": [
                {
                    "role": "system",
                    "content": `You are an English teacher and please help to grade a student's essay from vocabulary and grammar and topic relevance on how well the essay aligns with the title, and output format as: {"vocabularyScore": *.*(0-100), "grammarScore": *.*(0-100), "topicScore": *.*(0-100)}.`
                },
                {
                    "role": "user",
                    "content": `Example1: this essay: '${sampleSentence1}' has vocabulary and grammar scores of 80 and 80, respectively. ` +
                        `Example2: this essay: '${sampleSentence2}' has vocabulary and grammar scores of 40 and 43, respectively. ` +
                        `Example3: this essay: '${sampleSentence3}' has vocabulary and grammar scores of 50 and 50, respectively. ` +
                        `The essay for you to score is '${recognizedText}', and the title is '${topic}'. ` +
                        `The script is from speech recognition so that please first add punctuations when needed, remove duplicates and unnecessary un uh from oral speech, then find all the misuse of words and grammar errors in this essay, find advanced words and grammar usages, and finally give scores based on this information. Please only response as this format {"vocabularyScore": *.*(0-100), "grammarScore": *.*(0-100), "topicScore": *.*(0-100)}`
                }
            ],
            "temperature": 0,
            "top_p": 1
        }

        const response = await fetch(url, {
            method: "POST",
            headers: headers,
            body: JSON.stringify(data)
        });
        
        const result = await response.json();
        const content = JSON.parse(result.choices[0].message.content);

        return content;
    }

    function onRecognizedResult() {
        const recognizedText = recognizedTexts.join(" ");
        console.log(`Recognized text: ${recognizedText}`);
        getContentScores(recognizedText, settings.topic).then((contentResult) => {
            console.log("Content assessment result: \n",
                "\tvocabulary score: ", Number(contentResult.vocabularyScore.toFixed(1)), '\n',
                "\tgrammar score: ", Number(contentResult.grammarScore.toFixed(1)), '\n',
                "\ttopic score: ", Number(contentResult.topicScore.toFixed(1))
            );
        });
    }

    reco.canceled = function (s, e) {
        if (e.reason === sdk.CancellationReason.Error) {
            var str = "(cancel) Reason: " + sdk.CancellationReason[e.reason] + ": " + e.errorDetails;
            console.log(str);
        }
        reco.stopContinuousRecognitionAsync();
    };

    reco.sessionStopped = function (s, e) {
        reco.stopContinuousRecognitionAsync();
        reco.close();
        onRecognizedResult();
    };

    reco.startContinuousRecognitionAsync();
}
