export class AudioHandler {
  private context: AudioContext;
  private workletNode: AudioWorkletNode | null = null;
  private stream: MediaStream | null = null;
  private source: MediaStreamAudioSourceNode | null = null;
  private readonly sampleRate = 24000;

  private nextPlayTime: number = 0;
  private isPlaying: boolean = false;
  private playbackQueue: AudioBufferSourceNode[] = [];

  // Timeline-based recording properties
  private isRecordingSession: boolean = false;
  private sessionStartTime: number = 0;
  private lastInputRecordedTime: number = 0;
  private lastOutputRecordedTime: number = 0;
  private audioTimeline: Array<{
    type: 'input' | 'output' | 'input-silence' | 'output-silence';
    startTime: number,
    data: Int16Array
  }> = [];
  private recordingInterval: NodeJS.Timeout | null = null;
  private silenceBuffer: Int16Array = new Int16Array(1200); // 50ms of silence at 24kHz

  // Animation properties
  private circleElement: HTMLElement | null = null;

  private analysisNode: AnalyserNode | null = null;
  private dataArray: Uint8Array | null = null;

  private recordAnimationFrameId: number | null = null;
  private playChunkAnimationFrameId: number | null = null;

  constructor() {
    this.context = new AudioContext({ sampleRate: this.sampleRate });
      }

  async initialize() {
    await this.context.audioWorklet.addModule("/audio-processor.js");

    this.analysisNode = this.context.createAnalyser();
    this.analysisNode.fftSize = 2048;
    this.analysisNode.smoothingTimeConstant = 0.85;
    this.dataArray = new Uint8Array(this.analysisNode.frequencyBinCount);
  }

  getSampleRate(): number {
    return this.sampleRate;
  }

  async startRecording(onChunk: (chunk: Uint8Array) => void) {
    try {
      if (!this.workletNode) {
        await this.initialize();
      }

      this.stream = await navigator.mediaDevices.getUserMedia({
        audio: {
          channelCount: 1,
          sampleRate: this.sampleRate,
          echoCancellation: true,
          noiseSuppression: true,
        },
      });

      await this.context.resume();
      this.source = this.context.createMediaStreamSource(this.stream);
      this.workletNode = new AudioWorkletNode(
        this.context,
        "audio-recorder-processor"
      );

      this.workletNode.port.onmessage = (event) => {
        if (event.data.eventType === "audio") {
          const float32Data = event.data.audioData;
          const int16Data = new Int16Array(float32Data.length);

          for (let i = 0; i < float32Data.length; i++) {
            const s = Math.max(-1, Math.min(1, float32Data[i]));
            int16Data[i] = s < 0 ? s * 0x8000 : s * 0x7fff;
          }

          // record user input into the session timeline
          if (this.isRecordingSession) {
            this.recordInputChunk(int16Data);
          }

          const uint8Data = new Uint8Array(int16Data.buffer);
          onChunk(uint8Data);
        }
      };

      this.source.connect(this.workletNode);
      if (this.analysisNode && this.circleElement) {
        this.source?.connect(this.analysisNode);
        this.startAnimation("record");
      }
      this.workletNode.connect(this.context.destination);

      this.workletNode.port.postMessage({ command: "START_RECORDING" });
    } catch (error) {
      console.error("Error starting recording:", error);
      throw error;
    }
  }

  stopRecording() {
    if (!this.workletNode || !this.source || !this.stream) {
      throw new Error("Recording not started");
    }

    this.workletNode.port.postMessage({ command: "STOP_RECORDING" });

    this.workletNode.disconnect();
    this.source.disconnect();
    this.stream.getTracks().forEach((track) => track.stop());
  }

  startStreamingPlayback() {
    this.isPlaying = true;
    this.nextPlayTime = this.context.currentTime;
  }

  stopStreamingPlayback() {
    this.isPlaying = false;
    this.playbackQueue.forEach((source) => source.stop());
    this.playbackQueue = [];

    // clear output in audio timeline whose start time is greater than current time
    const now = Date.now() - this.sessionStartTime;
    this.audioTimeline = this.audioTimeline.filter((chunk) => {
      return chunk.startTime <= now;
    }
    );
    // find the last output recorded time
    const lastOutput = this.audioTimeline
      .filter((chunk) => chunk.type === 'output')
      .sort((a, b) => b.startTime - a.startTime)[0];
    if (lastOutput) {
      this.lastOutputRecordedTime = lastOutput.startTime + (lastOutput.data.length / this.sampleRate) * 1000;
    } else {
      this.lastOutputRecordedTime = 0;
    }
  }

  playChunk(chunk: Uint8Array, onChunkPlayed: () => Promise<void>) {
    if (!this.isPlaying) return;

    const int16Data = new Int16Array(chunk.buffer);

    // record assistant output into the session timeline
    if (this.isRecordingSession) {
      this.recordOutputChunk(int16Data);
    }

    const float32Data = new Float32Array(int16Data.length);
    for (let i = 0; i < int16Data.length; i++) {
      float32Data[i] = int16Data[i] / (int16Data[i] < 0 ? 0x8000 : 0x7fff);
    }

    const audioBuffer = this.context.createBuffer(
      1,
      float32Data.length,
      this.sampleRate
    );
    audioBuffer.getChannelData(0).set(float32Data);

    const source = this.context.createBufferSource();
    source.buffer = audioBuffer;
    source.connect(this.context.destination);
    if (this.analysisNode && this.circleElement) {
      source?.connect(this.analysisNode);
      this.startAnimation("play-chunk");
    }

    const chunkDuration = audioBuffer.length / this.sampleRate;
    if (this.nextPlayTime < this.context.currentTime) {
      this.nextPlayTime = this.context.currentTime;
    }

    source.start(this.nextPlayTime);

    this.playbackQueue.push(source);
    source.onended = () => {
      onChunkPlayed();
      const index = this.playbackQueue.indexOf(source);
      if (index > -1) {
        this.playbackQueue.splice(index, 1);
      }
      if (
        this.playbackQueue.length === 0 &&
        this.stream &&
        this.circleElement
      ) {
        this.startAnimation("record");
      }
    };

    this.nextPlayTime += chunkDuration;
  }

  // Fill gaps in the timeline with silence
  private fillSilenceGap(currentTime: number, input: boolean) {
    const lastRecordedTime = input
      ? this.lastInputRecordedTime
      : this.lastOutputRecordedTime;
    if (lastRecordedTime < currentTime) {
      // There's a gap that needs to be filled with silence
      const gapDuration = currentTime - lastRecordedTime;
      console.log(`Filling silence gap. Current time: ${currentTime}, Last recorded time: ${lastRecordedTime}, Input: ${input}, Gap duration: ${gapDuration}ms`);
      const silenceSamplesNeeded = Math.floor((gapDuration * this.sampleRate) / 1000);

      if (silenceSamplesNeeded > 0) {
        // Add silence in chunks of our silenceBuffer size
        let remainingSamples = silenceSamplesNeeded;
        let timeOffset = lastRecordedTime;

        while (remainingSamples > 0) {
          const chunkSize = Math.min(remainingSamples, this.silenceBuffer.length);
          const silenceChunk = new Int16Array(chunkSize);

          this.audioTimeline.push({
            type: input ? 'input-silence' : 'output-silence',
            startTime: timeOffset,
            data: silenceChunk
          });

          remainingSamples -= chunkSize;
          timeOffset += (chunkSize / this.sampleRate) * 1000;
        }
      }
    }
  }

  private static readonly SILENCE_THRESHOLD_MS = 100;

  // record user input chunk into the timeline
  private recordInputChunk(int16Data: Int16Array) {
    const now = Date.now() - this.sessionStartTime;
    if (this.lastInputRecordedTime < now - AudioHandler.SILENCE_THRESHOLD_MS) {
      // Add silence if gap exists
      this.fillSilenceGap(now, true);
      this.lastInputRecordedTime = now;
    }
    // Record the input chunk
    this.audioTimeline.push({
      type: 'input',
      startTime: this.lastInputRecordedTime,
      data: new Int16Array(int16Data)
    });
    this.lastInputRecordedTime = this.lastInputRecordedTime + (int16Data.length / this.sampleRate) * 1000;
  }

  // record assistant output chunk into the timeline in 50ms sub-chunks
  private recordOutputChunk(int16Data: Int16Array) {
    const now = Date.now() - this.sessionStartTime;
    if (this.lastOutputRecordedTime < now - AudioHandler.SILENCE_THRESHOLD_MS) {
      this.fillSilenceGap(now, false);
      this.lastOutputRecordedTime = now;
    }
    const samplesPerChunk = Math.floor(this.sampleRate * 0.05); // 50ms chunks
    for (let i = 0; i < int16Data.length; i += samplesPerChunk) {
      const chunkSize = Math.min(samplesPerChunk, int16Data.length - i);
      const outputChunk = new Int16Array(chunkSize);
      outputChunk.set(int16Data.subarray(i, i + chunkSize));
      this.audioTimeline.push({
        type: 'output',
        startTime: this.lastOutputRecordedTime + (i / this.sampleRate) * 1000,
        data: outputChunk
      });
    }
    this.lastOutputRecordedTime = this.lastOutputRecordedTime + (int16Data.length / this.sampleRate) * 1000;
  }

  // New methods for session recording
  startSessionRecording() {
    this.isRecordingSession = true;
    this.sessionStartTime = Date.now();
    this.lastInputRecordedTime = 0;
    this.lastOutputRecordedTime = 0;
    this.audioTimeline = [];
        console.log("Started session recording with stereo channel separation");
  }

  stopSessionRecording() {
    this.isRecordingSession = false;

    // Add final silence to complete the recording if needed
    if (this.audioTimeline.length > 0) {
      const now = Date.now() - this.sessionStartTime;
      this.fillSilenceGap(now, false);
    }

    // Clear the silence recorder interval
    if (this.recordingInterval) {
      clearInterval(this.recordingInterval);
      this.recordingInterval = null;
    }

    console.log("Stopped session recording, timeline has", this.audioTimeline.length, "chunks");
  }

  hasRecordedAudio() {
    return this.audioTimeline.length > 0;
  }

  downloadRecording(filename = 'conversation-recording', sessionId?: string) {
    if (!this.hasRecordedAudio()) {
      console.warn('No audio recorded');
      return;
    }

    // Convert to WAV format
    const wav = this.createWavFile();

    // Create download link
    const blob = new Blob([wav], { type: 'audio/wav' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.style.display = 'none';
    a.href = url;

    // Append sessionId to the filename if provided
    const filenameWithSessionId = sessionId ? `${filename}-${sessionId}` : filename;
    a.download = `${filenameWithSessionId}.wav`;

    document.body.appendChild(a);
    a.click();

    // Clean up
    window.setTimeout(() => {
      document.body.removeChild(a);
      window.URL.revokeObjectURL(url);
    }, 100);
  }

  private createWavFile(): ArrayBuffer {
    // Sort timeline by startTime to ensure chronological order
    this.audioTimeline.sort((a, b) => a.startTime - b.startTime);

    // First, calculate total duration in samples based on the latest timestamp
    const latestTimestamp = this.audioTimeline.length > 0
      ? Math.max(...this.audioTimeline.map(chunk => chunk.startTime + (chunk.data.length / this.sampleRate) * 1000))
      : 0;

    const totalSamples = Math.ceil((latestTimestamp / 1000) * this.sampleRate);

    if (totalSamples === 0) {
      console.warn('No audio data to record');
      return new ArrayBuffer(0);
    }

    const allInputChunks = this.audioTimeline.filter(chunk => chunk.type === 'input' || chunk.type === 'input-silence');
    const allOutputChunks = this.audioTimeline.filter(chunk => chunk.type === 'output' || chunk.type === 'output-silence');

    // merge all input to one ArrayBuffer
    const inputData = new Int16Array(totalSamples);
    for (const chunk of allInputChunks) {
      const startSample = Math.floor((chunk.startTime / 1000) * this.sampleRate);
      const chunkData = chunk.data;
      for (let i = 0; i < chunkData.length; i++) {
        if (startSample + i < totalSamples) {
          inputData[startSample + i] = chunkData[i];
        }
      }
    }

    // merge all output to one ArrayBuffer
    const outputData = new Int16Array(totalSamples);
    for (const chunk of allOutputChunks) {
      const startSample = Math.floor((chunk.startTime / 1000) * this.sampleRate);
      const chunkData = chunk.data;
      for (let i = 0; i < chunkData.length; i++) {
        if (startSample + i < totalSamples) {
          outputData[startSample + i] = chunkData[i];
        }
      }
    }


    // Create a buffer for the WAV file
    const buffer = new ArrayBuffer(44 + totalSamples * 2 * 2); // header + data (16-bit stereo)
    const view = new DataView(buffer);

    // WAV header - standard format
    const writeString = (offset: number, string: string) => {
      for (let i = 0; i < string.length; i++) {
        view.setUint8(offset + i, string.charCodeAt(i));
      }
    };

    // RIFF chunk descriptor
    writeString(0, 'RIFF');
    view.setUint32(4, 36 + totalSamples * 2 * 2, true); // File size (stereo data is twice the size)
    writeString(8, 'WAVE');

    // fmt sub-chunk
    writeString(12, 'fmt ');
    view.setUint32(16, 16, true); // subchunk size
    view.setUint16(20, 1, true); // PCM format
    view.setUint16(22, 2, true); // stereo (2 channels)
    view.setUint32(24, this.sampleRate, true); // sample rate
    view.setUint32(28, this.sampleRate * 2 * 2, true); // byte rate (stereo at 2 bytes per sample per channel)
    view.setUint16(32, 4, true); // block align (stereo at 2 bytes per sample)
    view.setUint16(34, 16, true); // bits per sample

    // data sub-chunk
    writeString(36, 'data');
    view.setUint32(40, totalSamples * 2 * 2, true); // Data size (stereo data is twice the size)

    // Initialize the entire audio buffer with zeros (silence)
    for (let i = 0; i < totalSamples; i++) {
      const offset = 44 + i * 4; // 4 bytes per stereo sample
      view.setInt16(offset, 0, true);     // Left channel (silent)
      view.setInt16(offset + 2, 0, true); // Right channel (silent)
    }

    // Now write inputData and outputData into the buffer
    // inputData to left channel, outputData to right channel
    for (let i = 0; i < totalSamples; i++) {
      const offset = 44 + i * 4; // 4 bytes per stereo sample
      if (i < inputData.length) {
        view.setInt16(offset, inputData[i], true);     // Left channel
      } else {
        view.setInt16(offset, 0, true);     // Left channel (silent)
      }
      if (i < outputData.length) {
        view.setInt16(offset + 2, outputData[i], true); // Right channel
      } else {
        view.setInt16(offset + 2, 0, true); // Right channel (silent)
      }
    }

    return buffer;
  }

  setCircleElement(element: HTMLElement | null) {
    this.circleElement = element;
  }

  updateCircle(volume: number, animationType: string) {
    const circle = this.circleElement;

    if (!circle) return;
    const minSize = 160;
    const size = minSize + volume;
    circle.style.backgroundColor = animationType === "record" ? "lightgray" : "lightblue";
    circle.style.width = `${size}px`;
    circle.style.height = `${size}px`;
  }

  startAnimation(animationType: string) {
    const isRecord = animationType === "record";

    if (isRecord) {
      this.stopPlayChunkAnimation();
    } else {
      this.stopPlayChunkAnimation();
      this.stopRecordAnimation();
    }

    const calculateVolume = () => {
      if (!this.analysisNode || !this.dataArray) return;

      this.analysisNode.getByteFrequencyData(this.dataArray);

      const volume =
        Array.from(this.dataArray).reduce((acc, v) => acc + v, 0) /
        this.dataArray.length;
      this.updateCircle(volume, animationType);

      if (isRecord) {
        this.recordAnimationFrameId = requestAnimationFrame(calculateVolume);
      } else {
        this.playChunkAnimationFrameId = requestAnimationFrame(calculateVolume);
      }
    };

    calculateVolume();
  }

  stopRecordAnimation() {
    if (this.recordAnimationFrameId) {
      cancelAnimationFrame(this.recordAnimationFrameId);
    }
  }

  stopPlayChunkAnimation() {
    if (this.playChunkAnimationFrameId) {
      cancelAnimationFrame(this.playChunkAnimationFrameId);
    }
  }

  async close() {
    if (this.recordingInterval) {
      clearInterval(this.recordingInterval);
      this.recordingInterval = null;
    }

    this.workletNode?.disconnect();
    this.source?.disconnect();
    this.stream?.getTracks().forEach((track) => track.stop());
    await this.context.close();
  }
}
