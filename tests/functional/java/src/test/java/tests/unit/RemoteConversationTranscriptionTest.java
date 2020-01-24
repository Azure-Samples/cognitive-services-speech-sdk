//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import com.azure.core.util.polling.LongRunningOperationStatus;
import com.azure.core.util.polling.PollerFlux;
import com.azure.core.util.polling.SyncPoller;
import com.google.gson.JsonIOException;
import com.google.gson.JsonSyntaxException;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.ServicePropertyChannel;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioInputStream;
import com.microsoft.cognitiveservices.speech.audio.AudioStreamFormat;
import com.microsoft.cognitiveservices.speech.remoteconversation.RemoteConversationTranscriptionClient;
import com.microsoft.cognitiveservices.speech.remoteconversation.RemoteConversationTranscriptionOperation;
import com.microsoft.cognitiveservices.speech.remoteconversation.RemoteConversationTranscriptionResult;
import com.microsoft.cognitiveservices.speech.transcription.Conversation;
import com.microsoft.cognitiveservices.speech.transcription.ConversationTranscriber;
import com.microsoft.cognitiveservices.speech.transcription.ConversationTranscriptionResult;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

import tests.AudioUtterancesKeys;
import tests.DefaultSettingsKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;

import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.UUID;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeoutException;

import static junit.framework.TestCase.fail;
import static org.junit.Assert.assertNotNull;

public class RemoteConversationTranscriptionTest {
    private static ConversationTranscriber transcriber;
    private static WavFileAudioInputStream waveFilePullStream;
    private static boolean endofProcessing = false;
    private static Conversation conversation = null;

    private static String speechKey;
    private static String speechRegion;
    private static String inputDirectory;

    @BeforeClass
    public static void setupBeforeClass() {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();
        System.out.println("Current operation system: " + operatingSystem);
        boolean isMac = operatingSystem.contains("mac") || operatingSystem.contains("darwin");
        org.junit.Assume.assumeFalse(isMac);
        try {
            Settings.LoadSettings();
        } catch (JsonIOException | JsonSyntaxException | FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @Before
    public void readSystemProperties() {
        speechKey = Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key;
        speechRegion = Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region;
        inputDirectory = Settings.DefaultSettingsMap.get(DefaultSettingsKeys.INPUT_DIR);
    }

    @Ignore("TODO Fix this test!!!!")
    @Test
    public void simpleRemoteConversationTest() {
        SpeechConfig config = null;
        RemoteConversationTranscriptionResult pollResponse;
        int retryCount = 0;
        try {
            String randomUUIDString = testStartAndStopConversationTranscribingAsyncInternal(false);
            System.out.println(randomUUIDString);

            config = SpeechConfig.fromSubscription(speechKey,speechRegion);
            
            do{
                Thread.sleep((10000));
                RemoteConversationTranscriptionClient client = new RemoteConversationTranscriptionClient(config);
                PollerFlux<RemoteConversationTranscriptionOperation, RemoteConversationTranscriptionResult> remoteTranscriptionOperation = client.getTranscriptionOperation(randomUUIDString, 5);
    
                remoteTranscriptionOperation.subscribe(
                    pr -> {
                        System.out.println("Poll response status : " + pr.getStatus());
                        System.out.println("Poll response status : " + pr.getValue().getServiceStatus());
                        if(pr.getStatus() == LongRunningOperationStatus.FAILED) {
                            System.out.println("Error : " + pr.getValue().getError());
                        }
                    }
                );
    
                SyncPoller<RemoteConversationTranscriptionOperation, RemoteConversationTranscriptionResult> blockingOperation =  remoteTranscriptionOperation.getSyncPoller();
                blockingOperation.waitForCompletion();
                pollResponse = blockingOperation.getFinalResult();
    
                if(pollResponse != null) {
                    if(pollResponse.getConversationTranscriptionResults() != null) {
                        for (int i = 0; i < pollResponse.getConversationTranscriptionResults().size(); i++) {
                            ConversationTranscriptionResult result = pollResponse.getConversationTranscriptionResults().get(i);
                            System.out.println(result.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult.name()));
                            System.out.println(result.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult));
                            System.out.println(result.getOffset());
                            System.out.println(result.getDuration());
                            System.out.println(result.getUserId());
                            System.out.println(result.getReason());
                            System.out.println(result.getResultId());
                            System.out.println(result.getText());
                            System.out.println(result.toString());
                        }
                    }
                }
                retryCount++;
            } while(pollResponse == null && retryCount < 3);

            assertNotNull("Final result can not be not null", pollResponse);
            if(pollResponse.getConversationTranscriptionResults().size() < 4)
            {
                assertNotNull("Total transcription results do not match", null);
            }

            while(!endofProcessing) {
                Thread.sleep(200);
            }
        } catch (InterruptedException e) {
            fail(e.getMessage());
        } catch (ExecutionException e) {
            fail(e.getMessage());
        } catch (TimeoutException e) {
            fail(e.getMessage());
        } finally {
            if(config != null) {
                config.close();
            }
        }
    }

    private String testStartAndStopConversationTranscribingAsyncInternal(boolean destroyResource) throws InterruptedException, ExecutionException, TimeoutException
    {
        SpeechConfig config = SpeechConfig.fromSubscription(speechKey,speechRegion);
        config.setProperty("ConversationTranscriptionInRoomAndOnline", "true");

        // "Realtime" or "Async"
        config.setServiceProperty("transcriptionMode", "RealTimeAndAsync", ServicePropertyChannel.UriQueryParameter);
        String inputFile = Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.CONVERSATION_BETWEEN_TWO_PERSONS_ENGLISH).FilePath);
        waveFilePullStream = new WavFileAudioInputStream(inputFile);

        AudioStreamFormat audioStreamFormat = AudioStreamFormat.getWaveFormatPCM((long)16000, (short)16,(short)8);
        AudioInputStream audioInputStream = AudioInputStream.createPullStream(waveFilePullStream, audioStreamFormat);

        UUID uuid = UUID.randomUUID();
        String meetingId = uuid.toString();
        conversation = Conversation.createConversationAsync(config, meetingId).get();
        transcriber = new ConversationTranscriber(AudioConfig.fromStreamInput(audioInputStream));
        transcriber.joinConversationAsync(conversation);
        conversation.addParticipantAsync("xyz@example.com");
        getFirstTranscriberResult(transcriber);

        config.close();
        return meetingId;
    }

    private void getFirstTranscriberResult(ConversationTranscriber t) throws InterruptedException, ExecutionException, TimeoutException {
        getFirstTranscriberResultInternal(t, true, false);
    }

    private void getFirstTranscriberResult(ConversationTranscriber t, boolean destroyResource) throws InterruptedException, ExecutionException, TimeoutException {
        getFirstTranscriberResultInternal(t, false, destroyResource);
    }

    private void getFirstTranscriberResultInternal(ConversationTranscriber t, boolean useDefaultStopTranscribing, boolean destroyResource) throws InterruptedException, ExecutionException, TimeoutException {
        String result;
        final ArrayList<String> rEvents = new ArrayList<>();
        final ArrayList<String> cEvents = new ArrayList<>();

        t.transcribing.addEventListener((o, e) -> {
            System.out.println("On Websocket recognizing:" + e.getResult().getText());
        });

        t.transcribed.addEventListener((o, e) -> {
            rEvents.add(e.getResult().getText());
            System.out.println("On Websocket recognized:" + e.toString());
        });

        t.canceled.addEventListener((o, e) -> {
            System.out.println("Conversation transcriber Stopped:" + e.toString());
            try {
                t.stopTranscribingAsync().get();
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            } catch (ExecutionException ex) {
                ex.printStackTrace();
            }
        });

        t.sessionStopped.addEventListener((o, e) -> {
            System.out.println("Conversation transcriber Stopped:" + e.toString());

            try {
                t.stopTranscribingAsync().get();
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            } catch (ExecutionException ex) {
                ex.printStackTrace();
            }
            endofProcessing = true;
            t.close();
        });

        Future<?> future = t.startTranscribingAsync();
    }
}
