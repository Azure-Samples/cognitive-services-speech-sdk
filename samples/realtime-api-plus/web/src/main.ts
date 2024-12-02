// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

import { Player } from "./player.ts";
import { Recorder } from "./recorder.ts";
import "./style.css";
import { LowLevelRTClient, SessionUpdateMessage, Voice } from "rt-client";

let realtimeStreaming: LowLevelRTClient;
let audioRecorder: Recorder;
let audioPlayer: Player;
let peerConnection: RTCPeerConnection;

async function start_realtime(endpoint: string, apiKey: string, deploymentOrModel: string) {
  realtimeStreaming = new LowLevelRTClient(new URL(endpoint), { key: apiKey }, { deployment: deploymentOrModel });

  try {
    console.log("sending session config");
    await realtimeStreaming.send(createConfigMessage());
  } catch (error) {
    console.log(error);
    makeNewTextBlock("[Connection error]: Unable to send initial config message. Please check your endpoint and authentication details.");
    setFormInputState(InputState.ReadyToStart);
    return;
  }
  console.log("sent");
  await Promise.all([resetAudio(true), handleRealtimeMessages()]);
}

function createConfigMessage() : SessionUpdateMessage {

  let configMessage : SessionUpdateMessage = {
    type: "session.update",
    session: {
      turn_detection: {
        type: "server_vad",
      },
      input_audio_transcription: {
        model: "whisper-1"
      }
    }
  };

  const systemMessage = getSystemMessage();
  const temperature = getTemperature();
  const voice = getVoice();

  if (systemMessage) {
    configMessage.session.instructions = systemMessage;
  }
  if (!isNaN(temperature)) {
    configMessage.session.temperature = temperature;
  }
  if (voice) {
    configMessage.session.voice = voice as Voice;
  }

  return configMessage;
}

async function handleRealtimeMessages() {
  for await (const message of realtimeStreaming.messages()) {
    let consoleLog = "" + message.type;

    switch (message.type) {
      case "session.created":
        setFormInputState(InputState.ReadyToStop);
        makeNewTextBlock("<< Session Started >>");
        makeNewTextBlock();
        break;
      case "session.updated":
        if (message.session?.ice_servers) {
          console.log("Received ICE servers" + JSON.stringify(message.session.ice_servers));
          consoleLog += " " + JSON.stringify(message.session.ice_servers);
          peerConnection = new RTCPeerConnection({ iceServers: message.session.ice_servers });

          setupPeerConnection();
          peerConnection.onicegatheringstatechange = (): void => {
            if (peerConnection.iceGatheringState === "complete") {
            }
          };

          const sdp = await peerConnection.createOffer();
          await peerConnection.setLocalDescription(sdp);
          // sleep 2 seconds to wait for ICE candidates to be gathered
          await new Promise((resolve) => setTimeout(resolve, 2000));
          console.log("getting local description" + JSON.stringify(peerConnection.localDescription));
          await realtimeStreaming.send({
            type: "extension.avatar.connect",
            client_description: btoa(JSON.stringify(peerConnection.localDescription)),
          });
        }
        break;
      case "response.audio_transcript.delta":
        appendToTextBlock(message.delta);
        break;
      case "response.audio.delta":
        const binary = atob(message.delta);
        const bytes = Uint8Array.from(binary, (c) => c.charCodeAt(0));
        const pcmData = new Int16Array(bytes.buffer);
        audioPlayer.play(pcmData);
        break;

      case "input_audio_buffer.speech_started":
        makeNewTextBlock("<< Speech Started >>");
        let textElements = formReceivedTextContainer.children;
        latestInputSpeechBlock = textElements[textElements.length - 1];
        makeNewTextBlock();
        audioPlayer.clear();
        break;
      case "conversation.item.input_audio_transcription.completed":
        latestInputSpeechBlock.textContent += " User: " + message.transcript;
        break;
      case "response.done":
        formReceivedTextContainer.appendChild(document.createElement("hr"));
        break;
      case "extension.avatar.connecting":
        const sdpAnswer: RTCSessionDescription = new RTCSessionDescription(
          JSON.parse(atob(message.server_description)) as RTCSessionDescriptionInit,
        );
        await peerConnection.setRemoteDescription(sdpAnswer);
        break;
      default:
        consoleLog = JSON.stringify(message, null, 2);
        break
    }
    if (consoleLog) {
      console.log(consoleLog);
    }
  }
  resetAudio(false);
}

/**
 * Basic audio handling
 */

let recordingActive: boolean = false;
let buffer: Uint8Array = new Uint8Array();

function combineArray(newData: Uint8Array) {
  const newBuffer = new Uint8Array(buffer.length + newData.length);
  newBuffer.set(buffer);
  newBuffer.set(newData, buffer.length);
  buffer = newBuffer;
}

function processAudioRecordingBuffer(data: Buffer) {
  const uint8Array = new Uint8Array(data);
  combineArray(uint8Array);
  if (buffer.length >= 4800) {
    const toSend = new Uint8Array(buffer.slice(0, 4800));
    buffer = new Uint8Array(buffer.slice(4800));
    const regularArray = String.fromCharCode(...toSend);
    const base64 = btoa(regularArray);
    if (recordingActive) {
      realtimeStreaming.send({
        type: "input_audio_buffer.append",
        audio: base64,
      });
    }
  }

}

async function resetAudio(startRecording: boolean) {
  recordingActive = false;
  if (audioRecorder) {
    audioRecorder.stop();
  }
  if (audioPlayer) {
    audioPlayer.clear();
  }
  audioRecorder = new Recorder(processAudioRecordingBuffer);
  audioPlayer = new Player();
  audioPlayer.init(24000);
  if (startRecording) {
    const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
    audioRecorder.start(stream);
    recordingActive = true;
  }
}

/**
 * UI and controls
 */

const formReceivedTextContainer = document.querySelector<HTMLDivElement>(
  "#received-text-container",
)!;
const formStartButton =
  document.querySelector<HTMLButtonElement>("#start-recording")!;
const formStopButton =
  document.querySelector<HTMLButtonElement>("#stop-recording")!;
const formDeploymentOrModelField = document.querySelector<HTMLInputElement>("#deployment-or-model")!;
const formSessionInstructionsField =
  document.querySelector<HTMLTextAreaElement>("#session-instructions")!;
const formTemperatureField = document.querySelector<HTMLInputElement>("#temperature")!;
const formVoiceSelection = document.querySelector<HTMLInputElement>("#voice")!;
const formAvatarVideo = document.getElementById('remoteVideo')!;

let latestInputSpeechBlock: Element;

enum InputState {
  Working,
  ReadyToStart,
  ReadyToStop,
}

function setFormInputState(state: InputState) {
  formDeploymentOrModelField.disabled = state != InputState.ReadyToStart;
  formStartButton.disabled = state != InputState.ReadyToStart;
  formStopButton.disabled = state != InputState.ReadyToStop;
  formSessionInstructionsField.disabled = state != InputState.ReadyToStart;
  formVoiceSelection.disabled = state != InputState.ReadyToStart;
}

function getSystemMessage(): string {
  return formSessionInstructionsField.value || "";
}

function getTemperature(): number {
  return parseFloat(formTemperatureField.value);
}

function getVoice(): string {
  return formVoiceSelection.value;
}

function makeNewTextBlock(text: string = "") {
  let newElement = document.createElement("p");
  newElement.textContent = text;
  formReceivedTextContainer.appendChild(newElement);
}

function appendToTextBlock(text: string) {
  let textElements = formReceivedTextContainer.children;
  if (textElements.length == 0) {
    makeNewTextBlock();
  }
  textElements[textElements.length - 1].textContent += text;
}

formStartButton.addEventListener("click", async () => {
  setFormInputState(InputState.Working);
  const endpoint = import.meta.env.VITE_ENDPOINT;
  const key = "placeholder";
  const deploymentOrModel = formDeploymentOrModelField.value.trim();

  if (!key) {
    alert("API Key is required");
    return;
  }

  try {
    start_realtime(endpoint, key, deploymentOrModel);
  } catch (error) {
    console.log(error);
    setFormInputState(InputState.ReadyToStart);
  }
});

formStopButton.addEventListener("click", async () => {
  setFormInputState(InputState.Working);
  resetAudio(false);
  realtimeStreaming.close();
  setFormInputState(InputState.ReadyToStart);
});

formDeploymentOrModelField.addEventListener("change", async () => {
  if (formDeploymentOrModelField.value === "gpt4o-realtime") {
    // remove all options of voice selection
    formVoiceSelection.innerHTML = "";
    for (var voice of ["", "alloy", "echo", "shimmer"]) {
      var option = document.createElement("option");
      option.value = voice;
      option.text = voice;
      formVoiceSelection.appendChild(option);
    }
    formVoiceSelection.value = "";
  } else {
    const cnvVoice = import.meta.env.VITE_CNV_VOICE;
    const voiceOptions = [
      { id: 'en-us-ava:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Ava (HD)'},
      { id: 'en-us-steffan:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Steffan (HD)'},
      { id: 'en-us-andrew:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Andrew (HD)'},
      { id: 'zh-cn-xiaochen:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Xiaochen (HD)'},
      { id: 'en-us-emma:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Emma (HD)'},
      { id: 'en-us-emma2:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Emma (HD 2)'},
      { id: 'en-us-andrew2:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Andrew (HD 2)'},
      { id: 'de-DE-Seraphina:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Seraphina (HD)'},
      { id: 'en-us-aria:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Aria (HD)'},
      { id: 'en-us-davis:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Davis (HD)'},
      { id: 'en-us-jenny:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Jenny (HD)'},
      { id: 'ja-jp-masaru:DragonHDLatestNeural', name: 'DragonHDLatestNeural, Masaru (HD)'},
      { id: 'en-US-AvaNeural', name: 'Ava' },
      { id: 'en-US-AvaMultilingualNeural', name: 'Ava Multilingual' },
      { id: 'en-US-AlloyTurboMultilingualNeural', name: 'Alloy Turbo Multilingual'},
      { id: 'en-US-AndrewNeural', name: 'Andrew' },
      { id: 'en-US-AndrewMultilingualNeural', name: 'Andrew Multilingual' },
      { id: 'en-US-BrianMultilingualNeural', name: 'Brian Multilingual' },
      { id: 'en-US-EmmaMultilingualNeural', name: 'Emma Multilingual' },
      { id: 'en-US-NovaTurboMultilingualNeural', name: 'Nova Turbo Multilingual'},
      { id: 'zh-CN-XiaoxiaoMultilingualNeural', name: 'Xiaoxiao Multilingual' },
      { id: 'en-US-JennyNeural', name: 'Jenny' },
      { id: 'CNV:' + cnvVoice, name: cnvVoice + ' (Custom Neural Voice)' },
    ];

    function generateOptions() {
      return voiceOptions.map(option => {
        return `<option value="${option.id}"}>${option.name}</option>`;
      }).join('\n');
    }

// Example usage: Insert options into a select element
    formVoiceSelection.innerHTML = generateOptions();
    formVoiceSelection.value = "en-US-AvaNeural";
  }
});


function setupPeerConnection() {
  // Clean up existing video element if there is any
  for (var i = 0; i < formAvatarVideo.childNodes.length; i++) {
    formAvatarVideo.removeChild(formAvatarVideo.childNodes[i])
  }

  peerConnection.ontrack = function (event) {
    const mediaPlayer = document.createElement(event.track.kind) as HTMLMediaElement
    mediaPlayer.id = event.track.kind
    mediaPlayer.srcObject = event.streams[0]
    mediaPlayer.autoplay = true
    formAvatarVideo.appendChild(mediaPlayer)
    // document.getElementById('videoLabel').hidden = true
    // document.getElementById('overlayArea').hidden = false
  }

  peerConnection.addTransceiver('video', { direction: 'sendrecv' });
  peerConnection.addTransceiver('audio', { direction: 'sendrecv' });

  peerConnection.addEventListener("datachannel", event => {
    const dataChannel = event.channel
    dataChannel.onmessage = e => {
        console.log("[" + (new Date()).toISOString() + "] WebRTC event received: " + e.data)
    }
    dataChannel.onclose = () => {
      console.log("Data channel closed");
    };
  });
  peerConnection.createDataChannel("eventChannel")
}
// guessIfIsAzureOpenAI();