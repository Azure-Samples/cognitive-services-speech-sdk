//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.talkingavatar;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.graphics.Color;
import android.os.Bundle;
import android.text.Spannable;
import android.text.method.ScrollingMovementMethod;
import android.text.style.ForegroundColorSpan;
import android.util.Base64;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import com.microsoft.cognitiveservices.speech.Connection;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisCancellationDetails;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisResult;
import com.microsoft.cognitiveservices.speech.SpeechSynthesizer;

import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ExecutorService;
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

import static android.Manifest.permission.INTERNET;

public class MainActivity extends AppCompatActivity {
    private static final String AUDIO_CODEC_ISAC = "ISAC";
    private static final String VIDEO_CODEC_H264 = "H264";

    // Replace below with your own subscription key
    private static String speechSubscriptionKey = "YourSubscriptionKey";
    // Replace below with your own service region (e.g., "westus2").
    private static String serviceRegion = "YourServiceRegion";

    private SpeechConfig speechConfig;
    private SpeechSynthesizer synthesizer;
    private Connection connection;

    private PeerConnection peerConnection;
    private PeerConnection.Observer peerConnectionObserver;
    private SdpObserver sdpObserver;
    private SurfaceViewRenderer videoRenderer;

    private TextView outputMessage;

//    private SpeakingRunnable speakingRunnable;
    private ExecutorService singleThreadExecutor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Note: we need to request the permissions
        int requestCode = 5; // Unique code for the permission request
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{INTERNET}, requestCode);

        outputMessage = this.findViewById(R.id.outputMessage);
        outputMessage.setMovementMethod(new ScrollingMovementMethod());
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
    }

    public void setupWebRTC() {
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
        PeerConnectionFactory peerConnectionFactory = PeerConnectionFactory.builder()
                .setOptions(options)
                .setVideoDecoderFactory(videoDecoderFactory)
                .setVideoEncoderFactory(videoEncoderFactory)
                .setAudioDeviceModule(audioDeviceModule)
                .createPeerConnectionFactory();

        // Initialize video renderer
        videoRenderer = this.findViewById(R.id.webrtcVideoRenderer);
        videoRenderer.init(eglBase.getEglBaseContext(), null);
        videoRenderer.setScalingType(RendererCommon.ScalingType.SCALE_ASPECT_FILL);

        // Create audio track
        MediaConstraints audioConstraints = new MediaConstraints();
        audioConstraints.mandatory.add(new MediaConstraints.KeyValuePair("googEchoCancellation", "true"));
        audioConstraints.mandatory.add(new MediaConstraints.KeyValuePair("googAutoGainControl", "true"));
        audioConstraints.mandatory.add(new MediaConstraints.KeyValuePair("googHighpassFilter", "true"));
        audioConstraints.mandatory.add(new MediaConstraints.KeyValuePair("googNoiseSuppression", "true"));
        AudioSource audioSource = peerConnectionFactory.createAudioSource(audioConstraints);
        AudioTrack localAudioTrack = peerConnectionFactory.createAudioTrack("102", audioSource);

        // Create video track
        VideoSource videoSource = peerConnectionFactory.createVideoSource(false);
        VideoTrack videoTrack = peerConnectionFactory.createVideoTrack("103", videoSource);

        // Create media stream
        MediaStream mediaStream = peerConnectionFactory.createLocalMediaStream("110");
        mediaStream.addTrack(localAudioTrack);
        mediaStream.addTrack(videoTrack);

        // Create peer connection
        List<PeerConnection.IceServer> iceServers = new ArrayList<>();
        PeerConnection.IceServer iceServer = PeerConnection.IceServer
                .builder("turn:relay.communication.microsoft.com:443?transport=tcp")
                .setUsername("BQAANki+a4AB2qWVggesaxY+ZyGDLtboAAYlDGNZyI4AAAAQAxC6Y2laD2dOgrYx1Zk1SkZ91+GFWMuiNiwT/VRr+QAMdRBu660=")
                .setPassword("mM7SJk873o9FNgMPt+JARSxBxAs=")
                .createIceServer();
        iceServers.add(iceServer);

        PeerConnection.RTCConfiguration rtcConfig = new PeerConnection.RTCConfiguration(iceServers);
        rtcConfig.continualGatheringPolicy = PeerConnection.ContinualGatheringPolicy.GATHER_CONTINUALLY;
        rtcConfig.iceTransportsType = PeerConnection.IceTransportsType.RELAY;
        rtcConfig.audioJitterBufferMaxPackets = 0;

        peerConnectionObserver = new PeerConnection.Observer() {
            private int iceCandidateCount = 0;

            @Override
            public void onSignalingChange(PeerConnection.SignalingState signalingState) {

            }

            @Override
            public void onIceConnectionChange(PeerConnection.IceConnectionState iceConnectionState) {
                Log.i("[WebRTC][PeerConnectionObserver]", "WebRTC connection state: " + iceConnectionState.name());
            }

            @Override
            public void onIceConnectionReceivingChange(boolean b) {

            }

            @Override
            public void onIceGatheringChange(PeerConnection.IceGatheringState iceGatheringState) {
                Log.i("[WebRTC][PeerConnectionObserver]", "ICE gathering state: " + iceGatheringState.name());
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
                if (mediaStream.videoTracks.size() > 0) {
                    mediaStream.videoTracks.get(0).addSink(videoRenderer);
                }

                if (mediaStream.audioTracks.size() > 0) {
                    mediaStream.audioTracks.get(0).setEnabled(true);
                }
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

            }
        };

        peerConnection = peerConnectionFactory.createPeerConnection(rtcConfig, peerConnectionObserver);
        peerConnection.addStream(mediaStream);

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
//                Log.i("[WebRTC][SdpObserver]", "Offer creation succeeded: " + sessionDescription.description);
            }

            @Override
            public void onSetSuccess() {
//                Log.i("[WebRTC][SdpObserver]", "Offer set succeeded: " + peerConnection.getLocalDescription().description);
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

    private String preferCodec(String sdp, String codec, boolean isAudio) {
        final String[] lines = sdp.split("\r\n");
        final int mLineIndex = findMediaDescriptionLine(isAudio, lines);
        if (mLineIndex == -1) {
            Log.w("[WebRTC]", "No mediaDescription line, so can't prefer " + codec);
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
            Log.w("[WebRTC]", "No payload types with name " + codec);
            return sdp;
        }
        final String newMLine = movePayloadTypesToFront(codecPayloadTypes, lines[mLineIndex]);
        if (newMLine == null) {
            return sdp;
        }
        Log.d("[WebRTC]", "Change media description from: " + lines[mLineIndex] + " to " + newMLine);
        lines[mLineIndex] = newMLine;
        return joinString(Arrays.asList(lines), "\r\n", true /* delimiterAtEnd */);
    }

    /** Returns the line number containing "m=audio|video", or -1 if no such line exists. */
    private int findMediaDescriptionLine(boolean isAudio, String[] sdpLines) {
        final String mediaDescription = isAudio ? "m=audio " : "m=video ";
        for (int i = 0; i < sdpLines.length; ++i) {
            if (sdpLines[i].startsWith(mediaDescription)) {
                return i;
            }
        }
        return -1;
    }

    private @Nullable String movePayloadTypesToFront(
            List<String> preferredPayloadTypes, String mLine) {
        // The format of the media description line should be: m=<media> <port> <proto> <fmt> ...
        final List<String> origLineParts = Arrays.asList(mLine.split(" "));
        if (origLineParts.size() <= 3) {
            Log.e("[WebRTC]", "Wrong SDP media description format: " + mLine);
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

    private void connectAvatar(String localSDP) {
        speechConfig = SpeechConfig.fromEndpoint(URI.create("wss://" + serviceRegion + ".tts.speech.microsoft.com/cognitiveservices/websocket/v1?enableTalkingAvatar=true"), speechSubscriptionKey);
        synthesizer = new SpeechSynthesizer(speechConfig, null);
        connection = Connection.fromSpeechSynthesizer(synthesizer);

        String avatarCharacter = "lisa";
        String avatarStyle = "casual-sitting";
        String backgroundImageUrl = "";

        String avatarConfig = "{\n" +
                "    \"synthesis\": {\n" +
                "        \"video\": {\n" +
                "            \"protocol\": {\n" +
                "                \"name\": \"WebRTC\",\n" +
                "                \"webrtcConfig\": {\n" +
                "                    \"clientDescription\": \"" + localSDP + "\",\n" +
                "                    \"iceServers\": [{\n" +
                "                        \"urls\": [ \"turn:relay.communication.microsoft.com:3478\" ],\n" +
                "                        \"username\": \"BQAANki+a4AB2qWVggesaxY+ZyGDLtboAAYlDGNZyI4AAAAQAxC6Y2laD2dOgrYx1Zk1SkZ91+GFWMuiNiwT/VRr+QAMdRBu660=\",\n" +
                "                        \"credential\": \"mM7SJk873o9FNgMPt+JARSxBxAs=\"\n" +
                "                    }]\n" +
                "                }\n" +
                "            },\n" +
                "            \"format\":{\n" +
                "                \"crop\":{\n" +
                "                    \"topLeft\":{\n" +
                "                        \"x\": 0,\n" +
                "                        \"y\": 0\n" +
                "                    },\n" +
                "                    \"bottomRight\":{\n" +
                "                        \"x\": 1920,\n" +
                "                        \"y\": 1080\n" +
                "                    }\n" +
                "                },\n" +
                "                \"bitrate\": 1000000\n" +
                "            },\n" +
                "            \"talkingAvatar\": {\n" +
                "                \"customized\": false,\n" +
                "                \"character\": \"" + avatarCharacter + "\",\n" +
                "                \"style\": \"" + avatarStyle + "\",\n" +
                "                \"background\": {\n" +
                "                    \"color\": \"#FFFFFFFF\",\n" +
                "                    \"image\": {\n" +
                "                        \"url\": \"" + backgroundImageUrl + "\"\n" +
                "                    }\n" +
                "                }\n" +
                "            }\n" +
                "        }\n" +
                "    }\n" +
                "}";
        try {
            JSONObject avatarConfigJsonObj = new JSONObject(avatarConfig);
        } catch (org.json.JSONException e) {
            Log.e("[WebRTC]", e.toString());
        }

        connection.setMessageProperty("speech.config", "context", avatarConfig);
        synthesizer.SynthesisCanceled.addEventListener((o, e) -> {
            String cancellationDetails =
                    SpeechSynthesisCancellationDetails.fromResult(e.getResult()).toString();
            Log.e("[WebRTC]", cancellationDetails);
            e.close();
        });

        synthesizer.SpeakText("");
        String turnStartMessage = synthesizer.getProperties().getProperty("SpeechSDKInternal-ExtraTurnStartMessage");
        try {
            JSONObject turnStartMessageJsonObj = new JSONObject(turnStartMessage);
            String remoteSdpBase64 = turnStartMessageJsonObj.getJSONObject("webrtc").getString("connectionString");
            String remoteSdpJsonStr = new String(Base64.decode(remoteSdpBase64, Base64.DEFAULT), "UTF-8");
            JSONObject remoteSdpJsonObj = new JSONObject(remoteSdpJsonStr);
            String remoteSdp = remoteSdpJsonObj.getString("sdp");
//            Log.i("[WebRTC][Remote SDP]", "Remote SDP: " + remoteSdp);

            peerConnection.setRemoteDescription(sdpObserver, new SessionDescription(SessionDescription.Type.ANSWER, remoteSdp));
        } catch (JSONException e) {
            Log.e("[WebRTC][Remote SDP]", e.toString());
        } catch (UnsupportedEncodingException e) {
            Log.e("[WebRTC][Remote SDP]", e.toString());
        }
    }

    public void onStartSessionButtonClicked(View v) throws URISyntaxException {
        if (synthesizer != null) {
            speechConfig.close();
            synthesizer.close();
            connection.close();
        }

        // Reuse the synthesizer to lower the latency.
        // I.e. create one synthesizer and speak many times using it.
        clearOutputMessage();
        updateOutputMessage("Initializing synthesizer...\n");

        // Setup WebRTC
        setupWebRTC();
    }

    public void onSpeechButtonClicked(View v) {
        clearOutputMessage();

        if (synthesizer == null) {
            updateOutputMessage("Please initialize the speech synthesizer first\n", true, true);
            return;
        }

        EditText speakText = this.findViewById(R.id.speakText);
        String spokenText = speakText.getText().toString();
        try {
            SpeechSynthesisResult result = synthesizer.StartSpeakingText(spokenText);
        } catch (Exception e)
        {
        }
    }

    public void onStopButtonClicked(View v) {
        if (synthesizer == null) {
            updateOutputMessage("Please initialize the speech synthesizer first\n", true, true);
            return;
        }

        stopSynthesizing();
    }

    private void stopSynthesizing() {
        if (synthesizer != null) {
            synthesizer.StopSpeakingAsync();
        }
    }

    private void updateOutputMessage(String text) {
        updateOutputMessage(text, false, true);
    }

    private synchronized void updateOutputMessage(String text, boolean error, boolean append) {
        this.runOnUiThread(() -> {
            if (append) {
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
        });
    }

    private void clearOutputMessage() {
        updateOutputMessage("", false, false);
    }
}
