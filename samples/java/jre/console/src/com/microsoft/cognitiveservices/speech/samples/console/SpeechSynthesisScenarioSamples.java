package com.microsoft.cognitiveservices.speech.samples.console;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.util.EventHandler;
import org.apache.commons.pool2.PooledObject;
import org.apache.commons.pool2.impl.DefaultPooledObject;
import org.apache.commons.pool2.BasePooledObjectFactory;
import org.apache.commons.pool2.impl.GenericObjectPool;
import org.apache.commons.pool2.impl.GenericObjectPoolConfig;

import java.util.*;
import java.util.stream.IntStream;

public class SpeechSynthesisScenarioSamples {

    public static class SynthesizerPoolFactory extends BasePooledObjectFactory<SpeechSynthesizer> {

        private SpeechConfig config;

        public SynthesizerPoolFactory(){
            config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");
        }

        @Override
        public SpeechSynthesizer create() throws Exception {
            return new SpeechSynthesizer(config, null);
        }

        @Override
        public PooledObject<SpeechSynthesizer> wrap(SpeechSynthesizer synthesizer) {
            return new DefaultPooledObject<>(synthesizer);
        }

        @Override
        public void destroyObject(PooledObject<SpeechSynthesizer> p) throws Exception {
            SpeechSynthesizer synthesizer = p.getObject();
            synthesizer.close();
        }
    }

    // Speech synthesis sample for server scenario
    public static void synthesisServerScenarioAsync() throws InterruptedException {
        // For server scenario synthesizing with high concurrency, we recommend two methods to reduce the latency.
        // Firstly, reuse the synthesizers (e.g. use a synthesizer pool )to reduce the connection establish latency;
        // secondly, use AudioOutputStream or synthesizing event to streaming receive the synthesized audio to lower the first byte latency.

        GenericObjectPoolConfig<SpeechSynthesizer> poolConfig = new GenericObjectPoolConfig<>();
        poolConfig.setMinIdle(2);
        poolConfig.setMaxTotal(64);
        GenericObjectPool<SpeechSynthesizer> pool = new GenericObjectPool<>(new SynthesizerPoolFactory(),
            poolConfig);

        List<Long> latencies = new ArrayList<>();
        List<Long> processingTimes = new ArrayList<>();

        for (int turn = 0; turn < 3; turn++) {
            int finalTurn = turn;
            System.out.println(String.format("Turn: %d", finalTurn));

            IntStream.range(0, 64).parallel().forEach(i -> {
                try {
                    long start = System.currentTimeMillis();
                    SpeechSynthesizer synthesizer = pool.borrowObject();
                    final boolean[] first = {true};

                    EventHandler<SpeechSynthesisEventArgs> a = (Object o, SpeechSynthesisEventArgs e) -> {
                        // streaming receive audio data here.
                        if (!first[0]) return;
                        System.out.println(String.format("First byte latency: %s ms.", System.currentTimeMillis() - start));
                        first[0] = false;
                        if (finalTurn > 0) {
                            latencies.add(System.currentTimeMillis() - start);
                        }
                    };

                    synthesizer.Synthesizing.addEventListener(a);

                    SpeechSynthesisResult result = synthesizer.SpeakTextAsync(String.format("today is a nice day. %d%d", finalTurn, i)).get();

                    if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                        if (finalTurn > 0) {
                            processingTimes.add(System.currentTimeMillis() - start);
                        }
                        synthesizer.Synthesizing.removeEventListener(a);
                        pool.returnObject(synthesizer);
                    }
                    else {
                        SpeechSynthesisCancellationDetails speechSynthesisCancellationDetails = SpeechSynthesisCancellationDetails.fromResult(result);
                        System.out.println(speechSynthesisCancellationDetails.toString());
                        synthesizer.close();
                    }


                } catch (Exception e) {
                    e.printStackTrace();
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

        System.out.println(String.format(
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
                "95 Percentile Process Time  :%d ms\n",
            minLatency, maxLatency, avgLatency, latency90, latency95, minProcessingTime, maxProcessingTime,
            avgProcessingTime, processingTime90, processingTime95
        ));
    }

}
