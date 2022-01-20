// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

let SpeechSDK;
let connector;
let audioPlayer;

function audioReadLoop(audioStream) {
  const audioBuffer = new ArrayBuffer(320);
  audioStream.read(audioBuffer).then((bytesRead) => {
    if (bytesRead === 0 ) {
      audioPlayer.playAudioSample(audioBuffer);
      }
    if (bytesRead > 0) {
      audioPlayer.playAudioSample(audioBuffer);
      audioReadLoop(audioStream);
    }
  });
};

document.addEventListener("DOMContentLoaded", () => {
  listenButton = document.getElementById("listenButton");
  let subscriptionKey = document.getElementById("subscriptionKey");
  let serviceRegion = document.getElementById("serviceRegion");
  resultDiv = document.getElementById("resultDiv");
  activityDiv = document.getElementById("activityDiv");
  customActivityDiv = document.getElementById("customActivityDiv");
  newTokenDiv = document.getElementById("newTokenDiv");

  if (!!window.SpeechSDK) {
    SpeechSDK = window.SpeechSDK;
    listenButton.disabled = true;
    sendActivityButton.disabled = true;
    document.getElementById("content").style.display = "block";
    document.getElementById("warning").style.display = "none";
  }

  updateTokenButton.addEventListener("click", () => {
    connector.authorizationToken = newTokenDiv.value;
  })

  connectButton.addEventListener("click", () => {
    if (connector) {
      connector.close();
      connector = undefined;
    }

    let botConfig = SpeechSDK.BotFrameworkConfig.fromSubscription(
      subscriptionKey.value,
      serviceRegion.value
    );

    let audioConfig = SpeechSDK.AudioConfig.fromDefaultMicrophoneInput();
    connector = new SpeechSDK.DialogServiceConnector(botConfig, audioConfig);

    connector.connect();

    connectButton.innerText = "Reconnect";
    listenButton.disabled = false;
    sendActivityButton.disabled = false;
    
    let audioFormat = SpeechSDK.AudioStreamFormat.getDefaultInputFormat();
    audioPlayer = new SpeechSDK.BaseAudioPlayer(audioFormat);
    
    connector.activityReceived = (sender, eventArgs) => {
      let jsonActivity = eventArgs.activity;
      activityDiv.innerHTML += JSON.stringify(jsonActivity);

      if (
        eventArgs.activity.speak !== null &&
        eventArgs.activity.speak !== undefined
      ) {
        console.log(eventArgs.audioStream.privFormat);
        audioReadLoop(eventArgs.audioStream);
      }
    };
  });

  sendActivityButton.addEventListener("click", () => {
    let jsonStr = customActivityDiv.value;
    connector.sendActivityAsync(jsonStr);
  });

  listenButton.addEventListener("click", () => {
    listenButton.disabled = true;
    resultDiv.innerHTML = "";
    activityDiv.innerHTML = "";

    connector.recognizing = (sender, eventArgs) => {
      resultDiv.innerHTML += "Hyphothesis: " + eventArgs.result.text + "\n";
    };

    connector.recognized = (sender, eventArgs) => {
      listenButton.disabled = false;
    };

    connector.canceled = (sender, eventArgs) => {
      resultDiv.innerHTML += "Canceled event: " + eventArgs.reason + "\n";
    }

    connector.sessionStarted = (sender, eventArgs) => {
      resultDiv.innerHTML += "SessionStarted: " + eventArgs.sessionId + "\n";
    }

    connector.listenOnceAsync(
      result => {
        listenButton.disabled = false;
        resultDiv.innerHTML += "\r\nFinal result: " + result.text + "\r\n";
      },
      error => {
        listenButton.disabled = false;
        resultDiv.innerHTML += error;
      }
    );
  });
});