export function int16PCMToFloat32(uint8: Uint8Array): Float32Array {
  const len = uint8.length / 2;
  const output = new Float32Array(len);
  for (let i = 0; i < len; i++) {
    const lo = uint8[i * 2];
    const hi = uint8[i * 2 + 1];
    let val = (hi << 8) | lo;
    if (val >= 0x8000) val = val - 0x10000;
    output[i] = val / 0x8000;
  }
  return output;
}

export function downsampleBuffer(
  buffer: Float32Array,
  inRate: number,
  outRate: number
): Float32Array {
  const ratio = inRate / outRate;
  const len = Math.floor(buffer.length / ratio);
  const out = new Float32Array(len);
  for (let i = 0; i < len; i++) {
    out[i] = buffer[Math.floor(i * ratio)];
  }
  return out;
}

export function float32ToInt16PCM(input: Float32Array): Uint8Array {
  const output = new Uint8Array(input.length * 2);
  for (let i = 0; i < input.length; i++) {
    const s = Math.max(-1, Math.min(1, input[i]));
    const val = s < 0 ? s * 0x8000 : s * 0x7FFF;
    output[i * 2] = val & 0xff;
    output[i * 2 + 1] = (val >> 8) & 0xff;
  }
  return output;
}