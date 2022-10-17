//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.speechsynthesis.pauseexample

import android.media.AudioAttributes
import android.media.AudioFormat
import android.media.AudioManager
import android.media.AudioTrack
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.EditText
import androidx.appcompat.app.AppCompatActivity
import com.microsoft.cognitiveservices.speech.*
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

class MainActivity : AppCompatActivity() {
    private var speechConfig: SpeechConfig? = null
    private var synthesizer: SpeechSynthesizer? = null

    private var audioTrack: AudioTrack? = null
    private var audioDataStream : AudioDataStream? = null
    private var singleThreadExecutor: ExecutorService? = null
    private var connection: Connection? = null

    private val synchronizedObj = Any()
    private var stopped = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        singleThreadExecutor = Executors.newSingleThreadExecutor()

        audioTrack = AudioTrack(
            AudioAttributes.Builder()
                .setUsage(AudioAttributes.USAGE_MEDIA)
                .setContentType(AudioAttributes.CONTENT_TYPE_SPEECH)
                .build(),
            AudioFormat.Builder()
                .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                .setSampleRate(24000)
                .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                .build(),
            AudioTrack.getMinBufferSize(
                24000,
                AudioFormat.CHANNEL_OUT_MONO,
                AudioFormat.ENCODING_PCM_16BIT
            ) * 2,
            AudioTrack.MODE_STREAM,
            AudioManager.AUDIO_SESSION_ID_GENERATE
        )

        findViewById<Button>(R.id.buttonInitSynth)?.let { button ->
            button.setOnClickListener {
                initSynthPressed()
            }
        }

        findViewById<Button>(R.id.buttonPlayVoice)?.let { button ->
            button.setOnClickListener {
                startStreamPlaybackPressed()
            }
        }

        findViewById<Button>(R.id.buttonPauseVoice)?.let { button ->
            button.setOnClickListener {
                pauseButtonPressed(button)
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()

        // Release speech synthesizer and its dependencies
        if (synthesizer != null) {
            synthesizer!!.close()
        }
        if (speechConfig != null) {
            speechConfig!!.close()
        }
        if (audioTrack != null) {
            singleThreadExecutor!!.shutdownNow()
            audioTrack!!.flush()
            audioTrack!!.stop()
            audioTrack!!.release()
        }
    }

    private fun initSynthPressed() {
        if (synthesizer != null) {
            speechConfig!!.close()
            synthesizer!!.close()
            connection?.close()
        }

        // Reuse the synthesizer to lower the latency.
        // I.e. create one synthesizer and speak many times using it.
        Log.d(
            "SYNTH_EVENT",
            "Initializing synthesizer"
        )
        speechConfig = SpeechConfig.fromSubscription(speechSubscriptionKey, serviceRegion)
        // Use 24k Hz format for higher quality.
        speechConfig?.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Raw24Khz16BitMonoPcm)
        // Set voice name.
        speechConfig?.speechSynthesisVoiceName = "en-US-JennyNeural"
        synthesizer = SpeechSynthesizer(speechConfig, null)
        connection = Connection.fromSpeechSynthesizer(synthesizer)

        // we force a restart audio stream on the next play button press
        if(stopped) {
            audioDataStream?.close()
            audioDataStream = null
        }
        setupLogs()
    }

    private fun startStreamPlaybackPressed() {
        if (synthesizer == null) {
            Log.e("TTS_SAMPLE", "Please initialize the speech synthesizer first")
            return
        }
        val speakText = findViewById<EditText>(R.id.speakText)

        audioTrack!!.play()
        if (audioDataStream == null) {
            singleThreadExecutor!!.execute { startAudioStream(speakText.text.toString()) }
        } else {
            synchronized(synchronizedObj) { stopped = false }
            singleThreadExecutor!!.execute { readAudioData() }
        }
    }

    private fun pauseButtonPressed(v: View?) {
        if (synthesizer == null) {
            Log.e("TTS_SAMPLE", "Please initialize the speech synthesizer first")
            return
        }
        pausePlayback()
    }

    private fun pausePlayback() {
        if (audioTrack != null) {
            synchronized(synchronizedObj) { stopped = true }
            audioTrack!!.pause()
        }
    }

    private fun startAudioStream(content: String) {
        synchronized(synchronizedObj) { stopped = false }
        val result = synthesizer!!.StartSpeakingTextAsync(content).get()
        audioDataStream = AudioDataStream.fromResult(result)

        // Set the chunk size to 50 ms. 24000 * 16 * 0.05 / 8 = 2400
        readAudioData()
        Log.d("AUDIOTRACK", "Finished reading from audio stream result")
    }

    private fun readAudioData() {
        val buffer = ByteArray(2400)
        while (!stopped) {
            val len = audioDataStream!!.readData(buffer)
            if (len == 0L) {
                break
            }
            val bytesWritten = audioTrack!!.write(buffer, 0, len.toInt())
            Log.d("AUDIOTRACK", "$bytesWritten bytes")
        }
    }

    private fun setupLogs() {
        connection?.connected?.addEventListener { _, _ ->
            Log.d(
                "SYNTH_EVENT",
                "Connection established"
            )
        }
        connection?.disconnected?.addEventListener { _, _ ->
            Log.d(
                "SYNTH_EVENT",
                "Disconnected"
            )
        }
        synthesizer!!.SynthesisStarted.addEventListener { _, e: SpeechSynthesisEventArgs ->
            Log.d(
                "SYNTH_EVENT",
                "Synthesis started. Result Id: ${e.result.resultId}"
            )
            e.close()
        }
        synthesizer!!.Synthesizing.addEventListener { _, e: SpeechSynthesisEventArgs ->
            Log.d(
                "SYNTH_EVENT",
                "Synthesizing. received ${e.result.audioLength} bytes."
            )
            e.close()
        }
        synthesizer!!.SynthesisCompleted.addEventListener { _, e: SpeechSynthesisEventArgs ->
            Log.d(
                "SYNTH_EVENT",
                "Synthesis finished"
            )
            Log.d(
                "SYNTH_EVENT",
                "First byte latency: ${e.result.properties.getProperty(
                    PropertyId.SpeechServiceResponse_SynthesisFirstByteLatencyMs
                )}"
            )
            Log.d(
                "SYNTH_EVENT",
                "Finish latency: ${e.result.properties.getProperty(
                    PropertyId.SpeechServiceResponse_SynthesisFinishLatencyMs)} ms."
            )
            e.close()
        }
        synthesizer!!.SynthesisCanceled.addEventListener { _, e: SpeechSynthesisEventArgs ->
            val cancellationDetails =
                SpeechSynthesisCancellationDetails.fromResult(e.result).toString()
            Log.e(
                "SYNTH_EVENT",
                "Error synthesizing. Result ID: ${e.result.resultId}. Error detail:$cancellationDetails"
            )
            e.close()
        }
        synthesizer!!.WordBoundary.addEventListener { _, e: SpeechSynthesisWordBoundaryEventArgs ->
            Log.d(
                "SYNTH_EVENT",
                "Word boundary. Text offset ${e.textOffset}, length ${e.wordLength}; audio offset ${e.audioOffset/10000} ms."
            )
        }
    }

    companion object {
        // Replace below with your own subscription key
        private const val speechSubscriptionKey = "YourSubscriptionKey"
        // Replace below with your own service region (e.g., "westus").
        private const val serviceRegion = "YourServiceRegion"
    }
}