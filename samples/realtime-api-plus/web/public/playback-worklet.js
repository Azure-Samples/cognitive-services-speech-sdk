class PlaybackWorklet extends AudioWorkletProcessor {
  constructor() {
    super();
    this.port.onmessage = this.handleMessage.bind(this);
    this.port.on;
    this.buffer = [];
  }

  handleMessage(event) {
    if (event.data === null) {
      this.buffer = [];
      return;
    }
    this.buffer.push(...event.data);
  }

  process(inputs, outputs, parameters) {
    const output = outputs[0];
    const channel = output[0];

    if (this.buffer.length > channel.length) {
      const toProcess = this.buffer.splice(0, channel.length);
      channel.set(toProcess.map((v) => v / 32768));
    } else {
      // console.log("buffer.length", this.buffer.length, "channel.length", channel.length, Date.now());
      channel.set(this.buffer.map((v) => v / 32768));
      this.buffer = [];
    }

    return true;
  }
}

registerProcessor("playback-worklet", PlaybackWorklet);
