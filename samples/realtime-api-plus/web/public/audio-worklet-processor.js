class PCMProcessor extends AudioWorkletProcessor {
  constructor() {
    super();
  }

  process(inputs, outputs, parameters) {
    const input = inputs[0];
    if (input.length > 0) {
      const float32Buffer = input[0];
      const int16Buffer = this.convertFloat32ToInt16(float32Buffer);
      this.port.postMessage(int16Buffer);
    }
    return true;
  }

  convertFloat32ToInt16(float32Array) {
    const int16Array = new Int16Array(float32Array.length);
    for (let i = 0; i < float32Array.length; i++) {
      let val = Math.floor(float32Array[i] * 0x7fff);
      val = Math.max(-0x8000, Math.min(0x7fff, val));
      int16Array[i] = val;
    }
    return int16Array;
  }
}

registerProcessor("audio-worklet-processor", PCMProcessor);
