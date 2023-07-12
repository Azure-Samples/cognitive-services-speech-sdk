//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.continuous_reco

import android.Manifest.permission
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import com.microsoft.cognitiveservices.speech.SpeechConfig
import com.microsoft.cognitiveservices.speech.SpeechRecognizer
import com.microsoft.cognitiveservices.speech.audio.AudioConfig
import java.util.concurrent.Executors

class MainActivity : AppCompatActivity() {

    private var speechConfig: SpeechConfig? = null
    private var microphoneStream: MicrophoneStream? = null

    private val speechReco: SpeechRecognizer by lazy {
        speechConfig = SpeechConfig.fromSubscription(speechSubscriptionKey, speechRegion)
        destroyMicrophoneStream() // in case it was previously initialized
        microphoneStream = MicrophoneStream()

        SpeechRecognizer(
            speechConfig,
            AudioConfig.fromStreamInput(MicrophoneStream.create())
        )
    }

    private val startRecoButton: Button by lazy {
        findViewById(R.id.buttonStartContinuousReco)
    }

    private val stopRecoButton: Button by lazy {
        findViewById(R.id.buttonStopContinuousReco)
    }

    private val recognizedText: TextView by lazy {
        findViewById(R.id.recognizedText)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        requestPermission()
        setViews()
    }

    private fun setViews() {
        startRecoButton.setOnClickListener {
            startReco()
        }

        stopRecoButton.setOnClickListener {
            stopReco()
        }
    }

    private fun startReco() {
        startRecoButton.isEnabled = false

        speechReco.recognized.addEventListener { sender, e ->
            val finalResult = e.result.text
            runOnUiThread {
                recognizedText.text = (finalResult)
                stopReco()
            }
        }

        val task = speechReco.startContinuousRecognitionAsync()
        executorService.submit {
            task.get()
            Log.i(activityTag, "Continuous recognition finished. Stopping speechReco")
        }
    }

    private fun stopReco() {
        startRecoButton.isEnabled = true
        speechReco.stopContinuousRecognitionAsync()
    }

    override fun onDestroy() {
        super.onDestroy()
        destroyMicrophoneStream()
        speechReco.close()
        speechConfig?.close()
        speechConfig = null
    }

    private fun destroyMicrophoneStream() {
        if (microphoneStream != null) {
            microphoneStream?.close()
            microphoneStream = null
        }
    }

    private fun requestPermission() {
        ActivityCompat.requestPermissions(this,
            arrayOf(
                permission.RECORD_AUDIO,
                permission.INTERNET,
                permission.READ_EXTERNAL_STORAGE
            ),
            1 )
    }

    companion object {
        // Replace below with your own subscription key
        private const val speechSubscriptionKey = "YourSubscriptionKey"
        // Replace below with your own service region (e.g., "westus").
        private const val speechRegion = "YourServiceRegion"

        private const val activityTag = "MainActivity"

        private val executorService = Executors.newCachedThreadPool()
    }
}