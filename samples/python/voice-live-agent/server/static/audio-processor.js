class RingBufferProcessor extends AudioWorkletProcessor {
  constructor() {
    super();
    this.buffer = new Float32Array(0);
    this.port.onmessage = e => {
      if (e.data.pcm) {
        const next = new Float32Array(this.buffer.length + e.data.pcm.length);
        next.set(this.buffer);
        next.set(e.data.pcm, this.buffer.length);
        this.buffer = next;
      } else if (e.data.clear) {
        this.buffer = new Float32Array(0);
      }
    };
  }

  process(_, outputs) {
    const out = outputs[0][0];
    if (this.buffer.length >= out.length) {
      out.set(this.buffer.subarray(0, out.length));
      this.buffer = this.buffer.subarray(out.length);
    } else {
      out.fill(0);
      this.buffer = new Float32Array(0);
    }
    return true;
  }
}

registerProcessor('audio-processor', RingBufferProcessor);
