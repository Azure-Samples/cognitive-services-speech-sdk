<!DOCTYPE html>
<html>
<head>
  <title>Microsoft Cognitive Services Speech SDK JavaScript Transcription Quickstart</title>
  <meta charset="utf-8" />
</head>
<body style="font-family:'Helvetica Neue',Helvetica,Arial,sans-serif; font-size:13px;">
  <div id="warning">
    <h1 style="font-weight:500;">Speech Recognition Speech SDK not found (microsoft.cognitiveservices.speech.sdk.bundle.js missing).</h1>
  </div>
  
  <div id="content" style="display:none">
    <table width="100%">
      <tr>
        <td></td>
        <td><h1 style="font-weight:500;">Microsoft Cognitive Services Speech SDK JavaScript Quickstart</h1></td>
      </tr>
      <tr>
        <td align="right"><a href="https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started" target="_blank">Subscription</a>:</td>
        <td><input id="subscriptionKey" type="text" size="40" value="KEY"></td>
      </tr>
      <tr>
        <td align="right">Region</td>
        <td><input id="serviceRegion" type="text" size="40" value=""></td>
      </tr>
      <tr>
        <td align="right">Language:</td>
        <td align="left">
            <!-- For the full list of supported languages see: https://docs.microsoft.com/azure/cognitive-services/speech-service/supported-languages -->
            <select id="languageOptions">
                <option value="ar-EG">Arabic - EG</option>
                <option value="ca-ES">Catalan - ES</option>
                <option value="da-DK">Danish - DK</option>
                <option value="da-DK">Danish - DK</option>
                <option value="de-DE">German - DE</option>
                <option value="en-AU">English - AU</option>
                <option value="en-CA">English - CA</option>
                <option value="en-GB">English - GB</option>
                <option value="en-IN">English - IN</option>
                <option value="en-NZ">English - NZ</option>
                <option selected="selected" value="en-US">English - US</option>
                <option value="es-ES">Spanish - ES</option>
                <option value="es-MX">Spanish - MX</option>
                <option value="fi-FI">Finnish - FI</option>
                <option value="fr-CA">French - CA</option>
                <option value="fr-FR">French - FR</option>
                <option value="hi-IN">Hindi - IN</option>
                <option value="it-IT">Italian - IT</option>
                <option value="ja-JP">Japanese - JP</option>
                <option value="ko-KR">Korean - KR</option>
                <option value="nb-NO">Norwegian - NO</option>
                <option value="nl-NL">Dutch - NL</option>
                <option value="pl-PL">Polish - PL</option>
                <option value="pt-BR">Portuguese - BR</option>
                <option value="pt-PT">Portuguese - PT</option>
                <option value="ru-RU">Russian - RU</option>
                <option value="sv-SE">Swedish - SE</option>
                <option value="zh-CN">Chinese - CN</option>
                <option value="zh-HK">Chinese - HK</option>
                <option value="zh-TW">Chinese - TW</option>
            </select>
        </td>
      </tr>
      <tr>

        <td></td>
        <td><button id="startTranscriberButton">Start transcribing conversation</button></td>
      </tr>
      <tr>
        <td></td>
        <td><button id="stopTranscriberButton">Stop transcribing conversation, close transcriber</button></td>
      </tr>
      <tr>
        <td align="right" valign="top">Input Text</td>
        <td><textarea id="phraseDiv" style="display: inline-block;width:500px;height:200px"></textarea></td>
      </tr>
      <tr>
        <td align="right" valign="top">Result</td>
        <td><textarea id="resultDiv" style="display: inline-block;width:500px;height:100px"></textarea></td>
      </tr>
    </table>
  </div>

  <!-- Speech SDK reference sdk. -->
  <script src="https://aka.ms/csspeech/jsbrowserpackageraw"></script>

  <!-- Speech SDK Authorization token -->
  <script>
  // Note: Replace the URL with a valid endpoint to retrieve
  //       authorization tokens for your subscription.
  var authorizationEndpoint = "token.php";

  function RequestAuthorizationToken() {
    if (authorizationEndpoint) {
      var a = new XMLHttpRequest();
      a.open("GET", authorizationEndpoint);
      a.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
      a.send("");
      a.onload = function() {
        var token = JSON.parse(atob(this.responseText.split(".")[1]));
        serviceRegion.value = token.region;
        authorizationToken = this.responseText;
        subscriptionKey.disabled = true;
        subscriptionKey.value = "using authorization token (hit F5 to refresh)";
        console.log("Got an authorization token: " + token);
      }
    }
  }
  </script>

  <!-- Speech SDK USAGE -->
  <script>
    // status fields and start button in UI
    var startTranscriberButton, stopTranscriberButton, filePicker;

    // subscription key and region for speech services.
    var subscriptionKey, serviceRegion;
    var authorizationToken;
    var SpeechSDK;
    var transcriber;
       

    document.addEventListener("DOMContentLoaded", function () {
      startTranscriberButton = document.getElementById("startTranscriberButton");
      stopTranscriberButton = document.getElementById("stopTranscriberButton");
      subscriptionKey = document.getElementById("subscriptionKey");
      serviceRegion = document.getElementById("serviceRegion");
      const phraseDiv = document.getElementById("phraseDiv");
      const resultDiv = document.getElementById("resultDiv");
      
      const languageOptions = document.getElementById("languageOptions");
           
      var lastRecognized = ""

      startTranscriberButton.addEventListener("click", function () {
        phraseDiv.innerHTML = "";

        // if we got an authorization token, use the token. Otherwise use the provided subscription key
        let speechConfig;
        if (authorizationToken) {
          speechConfig = SpeechSDK.SpeechConfig.fromAuthorizationToken(authorizationToken, serviceRegion.value);
        } else {
          if (subscriptionKey.value === "" || subscriptionKey.value === "subscription") {
            alert("Please enter your Microsoft Cognitive Services Speech subscription key!");
            startTranscriberButton.disabled = false;
            return;
          }
          speechConfig = SpeechSDK.SpeechConfig.fromSubscription(subscriptionKey.value, serviceRegion.value);
        }
        speechConfig.speechRecognitionLanguage = languageOptions.value;
        
        const audioConfig = SpeechSDK.AudioConfig.fromDefaultMicrophoneInput();

        transcriber = new SpeechSDK.ConversationTranscriber(speechConfig, audioConfig);
        transcriber.sessionStarted = function (s, e) {
          window.console.log(e);
        }
        transcriber.sessionStopped = function (s, e) {
          window.console.log(e);
          startTranscriberButton.disabled = false;
        }
        transcriber.canceled = function (s, e) {
          window.console.log(e);
        }
        transcriber.transcribing = function (s, e) {
          // Uncomment line below to see intermediate results in console
          // window.console.log(e);
        };

        // The event recognized signals that a final recognition result is received.
        // This is the final event that a phrase has been recognized.
        // For transcription, you will get one recognized event for each phrase recognized.
        transcriber.transcribed = function (s, e) {
          window.console.log(e);

        // Indicates that recognizable speech was not detected, and that recognition is done.
        if (e.result.reason === SpeechSDK.ResultReason.NoMatch) {
          var noMatchDetail = SpeechSDK.NoMatchDetails.fromResult(e.result);
          resultDiv.innerHTML += "(transcribed)  Reason: " + SpeechSDK.ResultReason[e.result.reason] + " NoMatchReason: " + SpeechSDK.NoMatchReason[noMatchDetail.reason] + "\r\n";
        } else {
          resultDiv.innerHTML += "(transcribed)  Reason: " + SpeechSDK.ResultReason[e.result.reason] + " Text: " + e.result.text + "\r\n";
          resultDiv.innerHTML += "(transcribed)  SpeakerId: " + e.result.speakerId + "\r\n";
        }
        lastRecognized += e.result.text + "\r\n";
        phraseDiv.innerHTML = lastRecognized;
      };

      transcriber.startTranscribingAsync(
        () => {
          window.console.log("startTranscribing complete");
        });
        startTranscriberButton.disabled = true;
        stopTranscriberButton.disabled = false;
      });
      stopTranscriberButton.addEventListener("click", function () {
        transcriber.stopTranscribingAsync(
          () => {
            transcriber.close();
            transcriber = undefined;
          });
          startTranscriberButton.disabled = false;
          stopTranscriberButton.disabled = true;
      });
      if (!!window.SpeechSDK) {
        SpeechSDK = window.SpeechSDK;
        startTranscriberButton.disabled = false;
        stopTranscriberButton.disabled = true;

        document.getElementById('content').style.display = 'block';
        document.getElementById('warning').style.display = 'none';

        // in case we have a function for getting an authorization token, call it.
        if (typeof RequestAuthorizationToken === "function") {
          RequestAuthorizationToken();
        }
      }
    });
  
  
  </script>
</body>
</html>
