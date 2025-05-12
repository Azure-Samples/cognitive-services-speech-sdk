//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.talkingavatar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.content.Context;
import android.graphics.Color;
import android.media.AudioManager;
import android.os.Bundle;
import android.text.Layout;
import android.text.Spannable;
import android.text.method.ScrollingMovementMethod;
import android.text.style.ForegroundColorSpan;
import android.util.Base64;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.microsoft.cognitiveservices.speech.Connection;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisCancellationDetails;
import com.microsoft.cognitiveservices.speech.SpeechSynthesizer;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.json.JSONException;
import org.json.JSONObject;
import org.webrtc.AudioSource;
import org.webrtc.AudioTrack;
import org.webrtc.DataChannel;
import org.webrtc.DefaultVideoDecoderFactory;
import org.webrtc.DefaultVideoEncoderFactory;
import org.webrtc.EglBase;
import org.webrtc.EglRenderer;
import org.webrtc.IceCandidate;
import org.webrtc.MediaConstraints;
import org.webrtc.MediaStream;
import org.webrtc.PeerConnection;
import org.webrtc.PeerConnectionFactory;
import org.webrtc.RendererCommon;
import org.webrtc.RtpReceiver;
import org.webrtc.SdpObserver;
import org.webrtc.SessionDescription;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoDecoderFactory;
import org.webrtc.VideoEncoderFactory;
import org.webrtc.VideoSource;
import org.webrtc.VideoTrack;
import org.webrtc.audio.AudioDeviceModule;
import org.webrtc.audio.JavaAudioDeviceModule;

import static android.Manifest.permission.ACCESS_NETWORK_STATE;
import static android.Manifest.permission.INTERNET;

public class MainActivity extends AppCompatActivity {
    // Replace below with your own subscription key
    private static final String speechSubscriptionKey = "YourSubscriptionKey";
    // Replace below with your own endpoint URL (e.g., "https://westus2.api.cognitive.microsoft.com/").
    private static final String endpointUrl = "YourEndpointUrl";

    // Update below values if you want to use a different avatar
    private static final String avatarCharacter = "lisa";
    private static final String avatarStyle = "casual-sitting";
    // Set below parameter to true if you want to use custom avatar
    private static final boolean customAvatar = false;

    // Update below value if you want to use a different TTS voice
    private static final String ttsVoice = "en-US-AvaMultilingualNeural";
    // Fill below value if you want to use custom TTS voice
    private static final String ttsEndpointID = "";

    private static final String VIDEO_TRACK_ID = "ARDAMSv0";
    private static final String AUDIO_TRACK_ID = "ARDAMSa0";
    private static final String AUDIO_CODEC_ISAC = "ISAC";
    private static final String VIDEO_CODEC_H264 = "H264";

    private String iceUrl;
    private String iceUsername;
    private String icePassword;

    private SpeechConfig speechConfig;
    private SpeechSynthesizer synthesizer;
    private Connection connection;

    private PeerConnectionFactory peerConnectionFactory;
    private PeerConnection peerConnection;
    private AudioTrack audioTrack;
    private VideoTrack videoTrack;
    private SdpObserver sdpObserver;

    private Button startSessionButton;
    private Button stopSessionButton;
    private Button speakButton;
    private Button stopSpeakingButton;
    private SurfaceViewRenderer videoRenderer;
    private EglRenderer.FrameListener frameListener;
    private TextView outputMessage;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Note: we need to request the permissions
        int requestCode = 5; // Unique code for the permission request
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{ACCESS_NETWORK_STATE, INTERNET}, requestCode);

        startSessionButton = this.findViewById(R.id.startSessionButton);
        stopSessionButton = this.findViewById(R.id.stopSessionButton);
        speakButton = this.findViewById(R.id.speakButton);
        stopSpeakingButton = this.findViewById(R.id.stopSpeakingButton);
        setButtonAvailability(stopSessionButton, false);
        setButtonAvailability(speakButton, false);
        setButtonAvailability(stopSpeakingButton, false);

        outputMessage = this.findViewById(R.id.outputMessage);
        outputMessage.setMovementMethod(new ScrollingMovementMethod());

        // Switch audio output device from earphone to speaker, for louder volume.
        AudioManager audioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        audioManager.setSpeakerphoneOn(true);

        initializePeerConnectionFactory();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        // Release speech synthesizer and its dependencies
        if (synthesizer != null) {
            synthesizer.close();
            connection.close();
        }

        if (speechConfig != null) {
            speechConfig.close();
        }

        if (peerConnection != null) {
            peerConnection.close();
        }
    }

    public void onStartSessionButtonClicked(View v) throws URISyntaxException {
        setButtonAvailability(startSessionButton, false);

        if (synthesizer != null) {
            speechConfig.close();
            synthesizer.close();
            connection.close();
        }

        if (peerConnection != null) {
            peerConnection.close();
        }

        videoRenderer.addFrameListener(frameListener, 1.0f);
        fetchIceToken();
    }

    public void onSpeakButtonClicked(View v) throws ExecutionException, InterruptedException {
        if (synthesizer == null) {
            updateOutputMessage("Please start the avatar session first", true, true);
            return;
        }

        EditText inputText = this.findViewById(R.id.inputText);
        String spokenText = inputText.getText().toString();
        synthesizer.SpeakTextAsync(spokenText);
    }

    public void onStopSpeakingButtonClicked(View v) {
        setButtonAvailability(stopSpeakingButton, false);
        connection.sendMessageAsync("synthesis.control", "{\"action\":\"stop\"}");
    }

    public void onStopSessionButtonClicked(View v) {
        if (synthesizer == null) {
            updateOutputMessage("Please start the avatar session first", true, true);
            return;
        }

        synthesizer.close();
    }

    private void initializePeerConnectionFactory() {
        // Prepare peer connection factory
        PeerConnectionFactory.InitializationOptions initializationOptions =
                PeerConnectionFactory.InitializationOptions
                        .builder(getApplicationContext())
                        .setEnableInternalTracer(true)
                        .createInitializationOptions();
        PeerConnectionFactory.initialize(initializationOptions);

        PeerConnectionFactory.Options options = new PeerConnectionFactory.Options();
        EglBase eglBase = EglBase.create();
        VideoDecoderFactory videoDecoderFactory =
                new DefaultVideoDecoderFactory(eglBase.getEglBaseContext());
        VideoEncoderFactory videoEncoderFactory =
                new DefaultVideoEncoderFactory(eglBase.getEglBaseContext(), true, true);
        AudioDeviceModule audioDeviceModule =
                JavaAudioDeviceModule.builder(this).createAudioDeviceModule();
        peerConnectionFactory = PeerConnectionFactory.builder()
                .setOptions(options)
                .setVideoDecoderFactory(videoDecoderFactory)
                .setVideoEncoderFactory(videoEncoderFactory)
                .setAudioDeviceModule(audioDeviceModule)
                .createPeerConnectionFactory();

        // Initialize video renderer
        videoRenderer = this.findViewById(R.id.webrtcVideoRenderer);
        videoRenderer.init(eglBase.getEglBaseContext(), null);
        videoRenderer.setScalingType(RendererCommon.ScalingType.SCALE_ASPECT_FILL);
        setVideoRendererVisibility(false);
        frameListener = bitmap -> {
            setVideoRendererVisibility(true);
            setButtonAvailability(stopSessionButton, true);
            setButtonAvailability(speakButton, true);
            synthesizer.SynthesisStarted.addEventListener((o, e) -> {
                setButtonAvailability(stopSessionButton, false);
                setButtonAvailability(speakButton, false);
                setButtonAvailability(stopSpeakingButton, true);
                e.close();
            });
            synthesizer.SynthesisCompleted.addEventListener((o, e) -> {
                setButtonAvailability(stopSessionButton, true);
                setButtonAvailability(speakButton, true);
                setButtonAvailability(stopSpeakingButton, false);
                e.close();
            });
        };

        // Create audio track
        MediaConstraints audioConstraints = new MediaConstraints();
        audioConstraints.mandatory.add(new MediaConstraints.KeyValuePair("googEchoCancellation", "true"));
        audioConstraints.mandatory.add(new MediaConstraints.KeyValuePair("googAutoGainControl", "true"));
        audioConstraints.mandatory.add(new MediaConstraints.KeyValuePair("googHighpassFilter", "true"));
        audioConstraints.mandatory.add(new MediaConstraints.KeyValuePair("googNoiseSuppression", "true"));
        AudioSource audioSource = peerConnectionFactory.createAudioSource(audioConstraints);
        audioTrack = peerConnectionFactory.createAudioTrack(AUDIO_TRACK_ID, audioSource);

        // Create video track
        VideoSource videoSource = peerConnectionFactory.createVideoSource(false);
        videoTrack = peerConnectionFactory.createVideoTrack(VIDEO_TRACK_ID, videoSource);
    }

    private void fetchIceToken() {
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                if  (iceUrl == null) {
                    updateOutputMessage("Fetching ICE token ...", false, true);
                    try {
                        URL url = new URL(endpointUrl + "/cognitiveservices/avatar/relay/token/v1");
                        HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
                        urlConnection.setRequestMethod("GET");
                        urlConnection.setRequestProperty("Ocp-Apim-Subscription-Key", speechSubscriptionKey);
                        urlConnection.connect();
                        InputStream responseStream = urlConnection.getInputStream();
                        InputStreamReader streamReader = new InputStreamReader(responseStream);
                        BufferedReader bufferedReader = new BufferedReader(streamReader);
                        StringBuilder responseStringBuilder = new StringBuilder();
                        String responseLine;
                        while ((responseLine = bufferedReader.readLine()) != null) {
                            responseStringBuilder.append(responseLine);
                        }

                        String responseString = responseStringBuilder.toString();
                        JSONObject iceTokenJsonObj = new JSONObject(responseString);
                        iceUrl = iceTokenJsonObj.getJSONArray("Urls").getString(0);
                        iceUsername = iceTokenJsonObj.getString("Username");
                        icePassword = iceTokenJsonObj.getString("Password");
                        updateOutputMessage("ICE token successfully fetched", false, true);
                        setupWebRTC();
                    } catch (IOException e) {
                        Log.e("[ICE Token]", e.toString());
                    } catch (JSONException e) {
                        Log.e("[ICE Token]", e.toString());
                    }
                } else {
                    setupWebRTC();
                }
            }
        };

        Thread thread = new Thread(runnable);
        thread.start();
    }

    private void setupWebRTC() {
        // Create peer connection
        List<PeerConnection.IceServer> iceServers = new ArrayList<>();
        PeerConnection.IceServer iceServer = PeerConnection.IceServer
                .builder(iceUrl)
                .setUsername(iceUsername)
                .setPassword(icePassword)
                .createIceServer();
        iceServers.add(iceServer);

        PeerConnection.RTCConfiguration rtcConfig = new PeerConnection.RTCConfiguration(iceServers);
        rtcConfig.continualGatheringPolicy = PeerConnection.ContinualGatheringPolicy.GATHER_CONTINUALLY;
        rtcConfig.iceTransportsType = PeerConnection.IceTransportsType.RELAY;
        rtcConfig.audioJitterBufferMaxPackets = 0;

        // Encode SDP and send to service
        PeerConnection.Observer peerConnectionObserver = new PeerConnection.Observer() {
            private int iceCandidateCount = 0;

            @Override
            public void onSignalingChange(PeerConnection.SignalingState signalingState) {
            }

            @Override
            public void onIceConnectionChange(PeerConnection.IceConnectionState iceConnectionState) {
                Log.i("[WebRTC][PeerConnectionObserver]", "WebRTC connection state: " + iceConnectionState.name());
                updateOutputMessage("WebRTC connection state: " + iceConnectionState.name(), false, true);
                if (iceConnectionState.equals(PeerConnection.IceConnectionState.DISCONNECTED)) {
                    setVideoRendererVisibility(false);
                    setButtonAvailability(startSessionButton, true);
                    setButtonAvailability(stopSessionButton, false);
                    setButtonAvailability(speakButton, false);
                    setButtonAvailability(stopSpeakingButton, false);
                }
            }

            @Override
            public void onIceConnectionReceivingChange(boolean b) {
            }

            @Override
            public void onIceGatheringChange(PeerConnection.IceGatheringState iceGatheringState) {
                Log.i("[WebRTC][PeerConnectionObserver]", "ICE gathering state: " + iceGatheringState.name());
                updateOutputMessage("ICE gathering state: " + iceGatheringState.name(), false, true);
            }

            @Override
            public void onIceCandidate(IceCandidate iceCandidate) {
                iceCandidateCount++;
                Log.i("[WebRTC][PeerConnectionObserver]", "ICE candidate: " + iceCandidate.toString());
                if (iceCandidateCount >= 2) {
                    // Encode SDP and send to service
                    JSONObject sdpJsonObj = new JSONObject();
                    try {
                        sdpJsonObj.put("type", "offer");
                        sdpJsonObj.put("sdp", peerConnection.getLocalDescription().description);
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }

                    String sdpJsonStr = sdpJsonObj.toString();
                    String sdpBase64Str = Base64.encodeToString(sdpJsonStr.getBytes(), Base64.NO_WRAP);
                    connectAvatar(sdpBase64Str);
                }
            }

            @Override
            public void onIceCandidatesRemoved(IceCandidate[] iceCandidates) {
            }

            @Override
            public void onAddStream(MediaStream mediaStream) {
            }

            @Override
            public void onRemoveStream(MediaStream mediaStream) {
            }

            @Override
            public void onDataChannel(DataChannel dataChannel) {
            }

            @Override
            public void onRenegotiationNeeded() {
            }

            @Override
            public void onAddTrack(RtpReceiver rtpReceiver, MediaStream[] mediaStreams) {
                if (mediaStreams.length > 0) {
                    if (!mediaStreams[0].videoTracks.isEmpty()) {
                        mediaStreams[0].videoTracks.get(0).addSink(videoRenderer);
                    }

                    if (!mediaStreams[0].audioTracks.isEmpty()) {
                        mediaStreams[0].audioTracks.get(0).setEnabled(true);

                        // To boost the audio volume, set below value to be greater than 1.0
                        mediaStreams[0].audioTracks.get(0).setVolume(1.0);
                    }
                }
            }
        };

        // Create peer connection
        peerConnection = peerConnectionFactory.createPeerConnection(rtcConfig, peerConnectionObserver);
        peerConnection.addTrack(audioTrack);
        peerConnection.addTrack(videoTrack);

        // Add offer constrains
        MediaConstraints constraints = new MediaConstraints();
        constraints.mandatory.add(new MediaConstraints.KeyValuePair("OfferToReceiveAudio", "true"));
        constraints.mandatory.add(new MediaConstraints.KeyValuePair("OfferToReceiveVideo", "true"));

        // Create offer
        sdpObserver = new SdpObserver() {
            @Override
            public void onCreateSuccess(SessionDescription sessionDescription) {
                String sdp = sessionDescription.description;
                sdp = preferCodec(sdp, AUDIO_CODEC_ISAC, true);
                sdp = preferCodec(sdp, VIDEO_CODEC_H264, false);
                peerConnection.setLocalDescription(sdpObserver, new SessionDescription(sessionDescription.type, sdp));
                Log.i("[WebRTC][SdpObserver]", "Offer creation succeeded: " + sessionDescription.description);
            }

            @Override
            public void onSetSuccess() {
            }

            @Override
            public void onCreateFailure(String s) {
            }

            @Override
            public void onSetFailure(String s) {
            }
        };
        peerConnection.createOffer(sdpObserver, constraints);
    }

    private void connectAvatar(String localSDP) {
        URI uri;
        try {
            uri = new URI(endpointUrl + "/cognitiveservices/websocket/v1?enableTalkingAvatar=true");
            speechConfig = SpeechConfig.fromEndpoint(uri, speechSubscriptionKey);
            speechConfig.setSpeechSynthesisVoiceName(ttsVoice);
            if (ttsEndpointID != null && !ttsEndpointID.isEmpty()) {
                speechConfig.setEndpointId(ttsEndpointID);
            }

            synthesizer = new SpeechSynthesizer(speechConfig, null);
            connection = Connection.fromSpeechSynthesizer(synthesizer);
            synthesizer.SynthesisCanceled.addEventListener((o, e) -> {
                String cancellationDetails =
                        SpeechSynthesisCancellationDetails.fromResult(e.getResult()).toString();
                updateOutputMessage(cancellationDetails, true, true);
                Log.e("[Synthesizer]", cancellationDetails);
                e.close();
            });

            String avatarConfig = buildAvatarConfig(localSDP);
            try {
                JSONObject avatarConfigJsonObj = new JSONObject(avatarConfig);
            } catch (org.json.JSONException e) {
                Log.e("[WebRTC]", e.toString());
            }

            connection.setMessageProperty("speech.config", "context", avatarConfig);
            synthesizer.SpeakText("");
            String turnStartMessage = synthesizer.getProperties().getProperty("SpeechSDKInternal-ExtraTurnStartMessage");
            try {
                JSONObject turnStartMessageJsonObj = new JSONObject(turnStartMessage);
                String remoteSdpBase64 = turnStartMessageJsonObj.getJSONObject("webrtc").getString("connectionString");
                String remoteSdpJsonStr = new String(Base64.decode(remoteSdpBase64, Base64.DEFAULT), StandardCharsets.UTF_8);
                JSONObject remoteSdpJsonObj = new JSONObject(remoteSdpJsonStr);
                String remoteSdp = remoteSdpJsonObj.getString("sdp");
                Log.i("[WebRTC][Remote SDP]", "Remote SDP: " + remoteSdp);
                peerConnection.setRemoteDescription(sdpObserver, new SessionDescription(SessionDescription.Type.ANSWER, remoteSdp));
            } catch (JSONException e) {
                Log.e("[WebRTC][Remote SDP]", e.toString());
            }
        } catch (URISyntaxException e) {
            Log.e("[WebRTC]", e.toString());
        }
    }

    private synchronized void setVideoRendererVisibility(boolean visible) {
        this.runOnUiThread(() -> {
            if (visible) {
                videoRenderer.setBackgroundColor(0x00000000);
            } else {
                videoRenderer.setBackgroundColor(0xFFFFFFFF);
            }
        });
    }

    private synchronized void setButtonAvailability(Button button, boolean enabled) {
        this.runOnUiThread(() -> {
            button.setEnabled(enabled);
        });
    }

    private synchronized void updateOutputMessage(String text, boolean error, boolean append) {
        this.runOnUiThread(() -> {
            if (append) {
                if (outputMessage.length() > 0) {
                    outputMessage.append("\n");
                }

                outputMessage.append(text);
            } else {
                outputMessage.setText(text);
            }

            if (error) {
                Spannable spannableText = (Spannable) outputMessage.getText();
                spannableText.setSpan(new ForegroundColorSpan(Color.RED),
                    spannableText.length() - text.length(),
                    spannableText.length(),
                    0);
            }

            // Scroll to bottom in order to show latest message
            final Layout layout = outputMessage.getLayout();
            if (layout != null) {
                int scrollAmount = layout.getLineTop(outputMessage.getLineCount()) - outputMessage.getHeight();
                if (scrollAmount > 0) {
                    outputMessage.scrollTo(0, scrollAmount);
                } else {
                    outputMessage.scrollTo(0, 0);
                }
            }
        });
    }

    private String buildAvatarConfig(String localSDP) {
        return "{\n" +
                "    \"synthesis\": {\n" +
                "        \"video\": {\n" +
                "            \"protocol\": {\n" +
                "                \"name\": \"WebRTC\",\n" +
                "                \"webrtcConfig\": {\n" +
                "                    \"clientDescription\": \"" + localSDP + "\",\n" +
                "                    \"iceServers\": [{\n" +
                "                        \"urls\": [ \"" + iceUrl + "\" ],\n" +
                "                        \"username\": \"" + iceUsername + "\",\n" +
                "                        \"credential\": \"" + icePassword + "\"\n" +
                "                    }]\n" +
                "                }\n" +
                "            },\n" +
                "            \"format\":{\n" +
                "                \"crop\":{\n" +
                "                    \"topLeft\":{\n" +
                "                        \"x\": 640,\n" +
                "                        \"y\": 0\n" +
                "                    },\n" +
                "                    \"bottomRight\":{\n" +
                "                        \"x\": 1280,\n" +
                "                        \"y\": 1080\n" +
                "                    }\n" +
                "                },\n" +
                "                \"bitrate\": 1000000\n" +
                "            },\n" +
                "            \"talkingAvatar\": {\n" +
                "                \"customized\": " + customAvatar + ",\n" +
                "                \"character\": \"" + avatarCharacter + "\",\n" +
                "                \"style\": \"" + avatarStyle + "\",\n" +
                "                \"background\": {\n" +
                "                    \"color\": \"#FFFFFFFF\",\n" +
                "                    \"image\": {\n" +
                "                        \"url\": \"\"\n" +
                "                    }\n" +
                "                }\n" +
                "            }\n" +
                "        }\n" +
                "    }\n" +
                "}";
    }

    private String preferCodec(String sdp, String codec, boolean isAudio) {
        final String[] lines = sdp.split("\r\n");
        final int mLineIndex = findMediaDescriptionLine(isAudio, lines);
        if (mLineIndex == -1) {
            Log.w("[WebRTC][Local SDP]", "No mediaDescription line, so can't prefer " + codec);
            return sdp;
        }

        // A list with all the payload types with name `codec`. The payload types are integers in the
        // range 96-127, but they are stored as strings here.
        final List<String> codecPayloadTypes = new ArrayList<>();
        // a=rtpmap:<payload type> <encoding name>/<clock rate> [/<encoding parameters>]
        final Pattern codecPattern = Pattern.compile("^a=rtpmap:(\\d+) " + codec + "(/\\d+)+[\r]?$");
        for (String line : lines) {
            Matcher codecMatcher = codecPattern.matcher(line);
            if (codecMatcher.matches()) {
                codecPayloadTypes.add(codecMatcher.group(1));
            }
        }

        if (codecPayloadTypes.isEmpty()) {
            Log.w("[WebRTC][Local SDP]", "No payload types with name " + codec);
            return sdp;
        }

        final String newMLine = movePayloadTypesToFront(codecPayloadTypes, lines[mLineIndex]);
        if (newMLine == null) {
            return sdp;
        }

        Log.d("[WebRTC][Local SDP]", "Change media description from: " + lines[mLineIndex] + " to " + newMLine);
        lines[mLineIndex] = newMLine;
        return joinString(Arrays.asList(lines), "\r\n", true /* delimiterAtEnd */);
    }

    // Returns the line number containing "m=audio|video", or -1 if no such line exists.
    private int findMediaDescriptionLine(boolean isAudio, String[] sdpLines) {
        final String mediaDescription = isAudio ? "m=audio " : "m=video ";
        for (int i = 0; i < sdpLines.length; ++i) {
            if (sdpLines[i].startsWith(mediaDescription)) {
                return i;
            }
        }
        return -1;
    }

    private String movePayloadTypesToFront(
            List<String> preferredPayloadTypes, String mLine) {
        // The format of the media description line should be: m=<media> <port> <proto> <fmt> ...
        final List<String> origLineParts = Arrays.asList(mLine.split(" "));
        if (origLineParts.size() <= 3) {
            Log.e("[WebRTC][Local SDP]", "Wrong SDP media description format: " + mLine);
            return null;
        }
        final List<String> header = origLineParts.subList(0, 3);
        final List<String> unpreferredPayloadTypes =
                new ArrayList<>(origLineParts.subList(3, origLineParts.size()));
        unpreferredPayloadTypes.removeAll(preferredPayloadTypes);
        // Reconstruct the line with `preferredPayloadTypes` moved to the beginning of the payload
        // types.
        final List<String> newLineParts = new ArrayList<>();
        newLineParts.addAll(header);
        newLineParts.addAll(preferredPayloadTypes);
        newLineParts.addAll(unpreferredPayloadTypes);
        return joinString(newLineParts, " ", false /* delimiterAtEnd */);
    }

    private String joinString(
            Iterable<? extends CharSequence> s, String delimiter, boolean delimiterAtEnd) {
        Iterator<? extends CharSequence> iter = s.iterator();
        if (!iter.hasNext()) {
            return "";
        }

        StringBuilder buffer = new StringBuilder(iter.next());
        while (iter.hasNext()) {
            buffer.append(delimiter).append(iter.next());
        }

        if (delimiterAtEnd) {
            buffer.append(delimiter);
        }

        return buffer.toString();
    }
}
