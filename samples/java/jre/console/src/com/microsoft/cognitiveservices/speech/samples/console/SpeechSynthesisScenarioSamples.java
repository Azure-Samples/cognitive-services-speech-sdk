//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.console;

import com.microsoft.cognitiveservices.speech.*;
import org.apache.commons.pool2.PooledObject;
import org.apache.commons.pool2.impl.DefaultPooledObject;
import org.apache.commons.pool2.BasePooledObjectFactory;
import org.apache.commons.pool2.impl.GenericObjectPool;
import org.apache.commons.pool2.impl.GenericObjectPoolConfig;
import org.apache.commons.text.StringEscapeUtils;

import java.util.*;
import java.util.stream.IntStream;

// Scenario:
//     In scenario like voice assistant or call center,  normally TTS can be integrated from service side
//     The client app will talk to the middle layer service which will talk to ASR, dialog, and TTS.
//     In this scenario, latency is one important metrics for better user experience.
//
// Best Practice:
//      For server scenario synthesizing with high concurrency, we recommend two methods to reduce the latency.
//      Firstly, reuse the synthesizers (e.g. use a synthesizer pool ) to reduce the connection establish latency.
//      This is because new synthesizer instance need to take time to connect to the service. Reusing the instance can save time of connection.
//      Secondly, use AudioOutputStream or synthesizing event to streaming receive the synthesized audio to lower the first byte latency.
//      This middle layer service should send audio back to client app/device as soon as possible.
//      On the client side, it can start playback when enough audio bytes has been received.


public class SpeechSynthesisScenarioSamples {

    public static class SynthesizerPoolFactory extends BasePooledObjectFactory<SpeechSynthesizer> {

        private final SpeechConfig config;

        public SynthesizerPoolFactory(){
            // Creates an instance of a speech config with specified
            // subscription key and service region. Replace with your own subscription key
            // and service region (e.g., "westus").
            config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Use a compression format e.g. mp3 to save the bandwidth.
            config.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Audio24Khz48KBitRateMonoMp3);
        }

        @Override
        public SpeechSynthesizer create() {
            System.out.println("create a brand new synthesizer");
            return new SpeechSynthesizer(config, null);
        }

        @Override
        public PooledObject<SpeechSynthesizer> wrap(SpeechSynthesizer synthesizer) {
            return new DefaultPooledObject<>(synthesizer);
        }

        @Override
        public void destroyObject(PooledObject<SpeechSynthesizer> p) {
            SpeechSynthesizer synthesizer = p.getObject();
            synthesizer.close();
        }
    }

    /**
     * A synthesizer class to handle concurrent request
     */
    public static class SpeechSynthesisService {

        // Update the SSML pattern to control the synthesis voice, style and rate.
        private static final String SSML_PATTERN = "<speak xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' version='1.0' xml:lang='zh-CN'><voice name='%s' style='chat' rate='5%%'>%s</voice></speak>";
        private final GenericObjectPool<SpeechSynthesizer> pool;

        public SpeechSynthesisService() {
            // For server scenario synthesizing with high concurrency, we recommend two methods to reduce the latency.
            // Firstly, reuse the synthesizers (e.g. use a synthesizer pool )to reduce the connection establish latency;
            //          This is because new synthesizer instance need to take time to connect to the service. Reusing the instance can save time of conenction.
            // secondly, use AudioOutputStream or synthesizing event to streaming receive the synthesized audio to lower the first byte latency.
            GenericObjectPoolConfig<SpeechSynthesizer> poolConfig = new GenericObjectPoolConfig<>();
            poolConfig.setMinIdle(2);
            poolConfig.setMaxTotal(64);

            // Make sure pool is a single object instance used to handle all request.
            // You can put it as a static field of a class
            pool = new GenericObjectPool<>(new SynthesizerPoolFactory(),
                    poolConfig);
        }

        /**
         * A thread-safe method to synthesize content
         * @param content The text to synthesize
         * @param voice The voice name, e.g. en-US-AvaMultilingualNeural
         * @return The first byte latency and processing time, in millisecond.
         */
        public long[] synthesis(String content, String voice) {
            long[] latency = new long[2];
            try {
                long start = System.currentTimeMillis();
                boolean first = true;
                String ssml = String.format(SSML_PATTERN, voice, StringEscapeUtils.escapeXml11(content));
                SpeechSynthesizer synthesizer = pool.borrowObject();

                SpeechSynthesisResult result = synthesizer.StartSpeakingSsml(ssml);

                AudioDataStream audioDataStream = AudioDataStream.fromResult(result);

                // Adjust the buffer size based on your format. You use the buffer size of 50ms audio.
                byte[] buffer = new byte[1600];
                while (true) {
                    long len = audioDataStream.readData(buffer);
                    if (len == 0) {
                        break;
                    }
                    if (first) {
                        latency[0] = System.currentTimeMillis() - start;
                        System.out.printf("Speaking [%s], first byte latency: %s ms.%n", content, latency[0]);
                        first = false;
                    }
                    // Here you can save the audio or send the data to another pipeline in your service.
                }
                latency[1] = System.currentTimeMillis() - start;

                if (audioDataStream.getStatus() != StreamStatus.AllData) {
                    SpeechSynthesisCancellationDetails speechSynthesisCancellationDetails = SpeechSynthesisCancellationDetails.fromStream(audioDataStream);
                    System.out.println(speechSynthesisCancellationDetails);
                    synthesizer.close();
                }
                else {
                    pool.returnObject(synthesizer);
                }
                audioDataStream.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
            return latency;
        }
    }

    // Speech synthesis sample for server scenario
    public static void synthesisServerScenarioAsync() throws InterruptedException {
        List<Long> latencies = new ArrayList<>();
        List<Long> processingTimes = new ArrayList<>();

        SpeechSynthesisService service = new SpeechSynthesisService();

        for (int turn = 0; turn < 3; turn++) {
            int finalTurn = turn;
            System.out.printf("Turn: %d%n", finalTurn);

            IntStream.range(0, 64).parallel().forEach(i -> {
                long[] latency = service.synthesis(String.format("today is a nice day. %d%d", finalTurn, i), "en-US-AvaMultilingualNeural");
                if (finalTurn > 0) {
                    latencies.add(latency[0]);
                    processingTimes.add(latency[1]);
                }
            });

            Thread.sleep(2000);
        }

        Collections.sort(latencies);
        int avgLatency = (int) latencies.stream().mapToLong(val -> val).average().orElse(-1.0);
        Long minLatency = latencies.stream().mapToLong(val -> val).min().orElse(-1);
        Long maxLatency = latencies.stream().mapToLong(val -> val).max().orElse(-1);
        Long latency90 = latencies.get((int) Math.ceil(latencies.size() * 0.9));
        Long latency95 = latencies.get((int) Math.ceil(latencies.size() * 0.95));

        Collections.sort(processingTimes);
        int avgProcessingTime = (int) processingTimes.stream().mapToLong(val -> val).average().orElse(-1.0);
        Long minProcessingTime = processingTimes.stream().mapToLong(val -> val).min().orElse(-1);
        Long maxProcessingTime = processingTimes.stream().mapToLong(val -> val).max().orElse(-1);
        Long processingTime90 = processingTimes.get((int) Math.ceil(processingTimes.size() * 0.9));
        Long processingTime95 = processingTimes.get((int) Math.ceil(processingTimes.size() * 0.95));

        System.out.printf(
            "Min Latency                 :%d ms\n" +
            "Max Latency                 :%d ms\n" +
            "Average Latency             :%d ms\n" +
            "90 Percentile Latency       :%d ms\n" +
            "95 Percentile Latency       :%d ms\n" +
            "\n" +
            "Min Process Time            :%d ms\n" +
            "Max Process Time            :%d ms\n" +
            "Average Process Time        :%d ms\n" +
            "90 Percentile Process Time  :%d ms\n" +
            "95 Percentile Process Time  :%d ms\n%n",
            minLatency, maxLatency, avgLatency, latency90, latency95, minProcessingTime, maxProcessingTime,
            avgProcessingTime, processingTime90, processingTime95
        );
    }
}
