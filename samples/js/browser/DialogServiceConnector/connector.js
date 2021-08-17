// subscription key and region for speech services.
var subscriptionKey, serviceRegion;
var SpeechSDK;
var connector;
var audioPlayer;

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

document.addEventListener("DOMContentLoaded", function() {
  listenButton = document.getElementById("listenButton");
  subscriptionKey = document.getElementById("subscriptionKey");
  serviceRegion = document.getElementById("serviceRegion");
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

  updateTokenButton.addEventListener("click", function() {
    connector.authorizationToken = newTokenDiv.value;
  })

  connectButton.addEventListener("click", function() {
    if (connector) {
      connector.close();
      connector = undefined;
    }

    var botConfig = SpeechSDK.BotFrameworkConfig.fromSubscription(
      subscriptionKey.value,
      serviceRegion.value
    );

    var audioConfig = SpeechSDK.AudioConfig.fromDefaultMicrophoneInput();
    connector = new SpeechSDK.DialogServiceConnector(botConfig, audioConfig);

    connector.connect();

    connectButton.innerText = "Reconnect";
    listenButton.disabled = false;
    sendActivityButton.disabled = false;
    
    var audioFormat = SpeechSDK.AudioStreamFormat.getDefaultInputFormat();
    audioPlayer = new SpeechSDK.BaseAudioPlayer(audioFormat);
    
    connector.activityReceived = (sender, eventArgs) => {
      var jsonActivity = eventArgs.activity;
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

  sendActivityButton.addEventListener("click", function() {
    var jsonStr = customActivityDiv.value;
    connector.sendActivityAsync(jsonStr);
  });

  listenButton.addEventListener("click", function() {
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
