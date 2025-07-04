import logging
import threading
import numpy as np
import sounddevice as sd

from collections import deque

AUDIO_SAMPLE_RATE = 24000
logger = logging.getLogger(__name__)

class AudioPlayerAsync:
    def __init__(self):
        self.queue = deque()
        self.lock = threading.Lock()
        self.stream = sd.OutputStream(
            callback=self.callback,
            samplerate=AUDIO_SAMPLE_RATE,
            channels=1,
            dtype='int16',
            blocksize=2400
        )
        self.playing = False

    def callback(self, outdata, frames, time, status):
        if status:
            logger.warning(f"Stream status: {status}")
        with self.lock:
            data = np.empty(0, dtype=np.int16)
            while len(data) < frames and self.queue:
                chunk = self.queue.popleft()
                needed = frames - len(data)
                data = np.concatenate((data, chunk[:needed]))
                if len(chunk) > needed:
                    self.queue.appendleft(chunk[needed:])
            if len(data) < frames:
                data = np.concatenate((data, np.zeros(frames - len(data), dtype=np.int16)))
        outdata[:] = data.reshape(-1, 1)

    def add_data(self, data_bytes):
        arr = np.frombuffer(data_bytes, dtype=np.int16)
        with self.lock:
            self.queue.append(arr)
        if not self.playing and len(self.queue) > 0:
            self.play()

    def play(self):
        self.stream.start()
        self.playing = True

    def stop(self):
        print("[AudioPlayer] audio stop")
        self.stream.stop()
        self.playing = False

    def terminate(self):
        self.stop()
        print("[AudioPlayer] stream close")
        self.stream.close()
